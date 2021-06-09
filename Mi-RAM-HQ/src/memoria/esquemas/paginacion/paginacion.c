#include "paginacion.h"

private void paginacion_inicializar_frames_memoria(int);
private void paginacion_mostrar_frames(int);
private bool paginacion_tiene_frames_libres(int frames, t_list* lista_frames);
private int paginacion_frames_necesarios(int tamanio_data);
private bool paginacion_frame_esta_libre(p_frame_t* frame);
private p_patota_y_tabla_t* paginacion_agregar_patota_a_listado(uint32_t, uint32_t, int);
private bool paginacion_agregar_patota_en_memoria(uint32_t, uint32_t, const char*, int, p_patota_y_tabla_t*);
private p_frame_t* paginacion_cambiar_frame(p_frame_t*);
private int minimo(int, int);
private void paginacion_frame_modificado(p_frame_t* frame);
private void paginacion_modificar_tabla(p_patota_y_tabla_t* patota, int numero_de_frame, int numero_pagina);

void paginacion_memoria_init(void)
{
    esquema_memoria_tamanio = u_config_get_int_value("TAMANIO_MEMORIA");
    esquema_memoria_mfisica = u_malloc(esquema_memoria_tamanio);
    listado_patotas = list_create();
    lista_frames_memoria = list_create();
    lista_frames_swap = list_create();
    tamanio_pagina = u_config_get_int_value("TAMANIO_PAGINA");
    tamanio_swap = u_config_get_int_value("TAMANIO_SWAP");
    contador_memoria = 0;
    paginacion_inicializar_frames_memoria(esquema_memoria_tamanio/tamanio_pagina);
    //inicializar swap; 
    paginacion_mostrar_frames(esquema_memoria_tamanio/tamanio_pagina);
}

bool paginacion_memoria_inicializar_patota(uint32_t pid, uint32_t cant_tripulantes, const char* tareas)
{
    //8 tamaño pcb + 21 por cada uno de los tripulantes + el tamaño de las tareas. 
    int tamanio_data = 2*(sizeof(uint32_t)) + 21* cant_tripulantes + strlen(tareas)+1; 
    int frames_necesarios = paginacion_frames_necesarios(tamanio_data);
    U_LOG_TRACE("Frames necesarios %d", frames_necesarios);
    if(!paginacion_tiene_frames_libres(frames_necesarios, lista_frames_memoria)){ 
        return false;
    }
    U_LOG_TRACE("Hay frames necesarios"); //Chequear 
    p_patota_y_tabla_t* patota = paginacion_agregar_patota_a_listado(pid, cant_tripulantes, frames_necesarios);
    return paginacion_agregar_patota_en_memoria(pid, cant_tripulantes, tareas, tamanio_data, patota);

}

bool paginacion_memoria_inicializar_tripulante(uint32_t pid, uint32_t tid, u_pos_t pos)
{
    return true;
}

bool paginacion_memoria_actualizar_posicion_tripulante(uint32_t pid, uint32_t tid, u_pos_t pos)
{

}

bool paginacion_memoria_actualizar_estado_tripulante(uint32_t pid, uint32_t tid, char estado)
{

}

char* paginacion_memoria_obtener_proxima_tarea(uint32_t pid, uint32_t tid)
{

}

tripulantes_t* paginacion_memoria_obtener_info_tripulante(uint32_t pid, uint32_t tid)
{

}

t_list*        paginacion_memoria_obtener_todos_los_tripulantes(void)
{

}

bool           paginacion_memoria_expulsar_tripulante(uint32_t pid, uint32_t tid)
{

}

//---------------------------------------------------------------------------------------

private void paginacion_inicializar_frames_memoria(int cantidad_paginas){
    int i;
    for(i=0; i<cantidad_paginas; i++){
       p_frame_t* frame = u_malloc(sizeof(p_frame_t));
       frame->num_frame=i;
       frame->ocupado=0;
       list_add(lista_frames_memoria, frame);
    }
}

private void paginacion_mostrar_frames(int paginas){
    int i;
    for(i=0; i<paginas; i++){
        p_frame_t* frame_a_mostrar = list_get(lista_frames_memoria,i);
        U_LOG_TRACE("Frame: %d, ocupado: %d", i, frame_a_mostrar->ocupado);
    }
}

private bool paginacion_tiene_frames_libres(int frames, t_list* lista_frames){
    t_list* lista_frame_libres = list_filter(lista_frames, paginacion_frame_esta_libre);   
    int frames_libres = list_size(lista_frame_libres);
    list_destroy(lista_frame_libres);
    return frames_libres >= frames;
}

private int paginacion_frames_necesarios(int tamanio_data){
    if(tamanio_data%tamanio_pagina>0){
        return tamanio_data/tamanio_pagina+1;
    }
    return tamanio_data/tamanio_pagina;
}

private bool paginacion_frame_esta_libre(p_frame_t* frame){
    bool respuesta=(frame->ocupado)==0? true : false;
}

private p_patota_y_tabla_t* paginacion_agregar_patota_a_listado(uint32_t pid, uint32_t cant_tripulantes, int paginas){//ver si pasar las tareas
    int i;
    p_patota_y_tabla_t* patota = u_malloc(sizeof(p_patota_y_tabla_t));
    patota->pid = pid;
    patota->direccion_tareas = sizeof(uint32_t) + 21 * cant_tripulantes;
    patota->tabla = list_create();
    for(i=0; i<paginas; i++){
        p_fila_tabla_de_paginas* fila = u_malloc(sizeof(p_fila_tabla_de_paginas));
        fila->num_pagina = i;
        list_add(patota->tabla, fila);
    }
    list_add(listado_patotas, patota);   //mutex listado de patotas 
    U_LOG_TRACE("Se agrego patota con pid: %d, al listado de patotas", pid);
    return patota; 
}

private bool paginacion_agregar_patota_en_memoria(uint32_t pid, uint32_t cant_tripulantes, const char* tareas, int tamanio_data, p_patota_y_tabla_t* patota){
    uint32_t pid_y_direccion_tareas[2] = {pid, 8 + 21 * cant_tripulantes};
    int offset_general = 0;
    int i;
    int j;
    p_frame_t* frame_a_escribir = list_find(lista_frames_memoria, paginacion_frame_esta_libre);
    int numero_de_frame = frame_a_escribir->num_frame;
    int numero_de_pagina = -1;
    //escribo el pid y direccion logica inicio de tareas
    for(i = 0; i<2; i++){
        memcpy(esquema_memoria_mfisica + numero_de_frame*tamanio_pagina + offset_general, &pid_y_direccion_tareas[i], sizeof(uint32_t));//ver si estoy copiando bien
        offset_general = offset_general + sizeof(uint32_t);
        if(offset_general == tamanio_pagina){  
            frame_a_escribir = paginacion_cambiar_frame(frame_a_escribir); // esto anda bien?  OJO; 
            numero_de_frame = frame_a_escribir->num_frame;
            offset_general = 0;
            paginacion_modificar_tabla(patota, numero_de_frame, numero_de_pagina+1);
        }
    }
    U_LOG_TRACE("Pid: %d, y direccion de tareas: %d copiado con exito", pid_y_direccion_tareas[0], pid_y_direccion_tareas[1]);
    //hago espacio para tripulantes y escribo proxima tarea y direccion logica del pcb
    uint32_t prox_tarea_y_direccion_pcb[2] = {0, 0}; //direccion logica del pcb es 0?
    for(i=0; i<cant_tripulantes; i++){
        for(j=0; j<13; j++){
            offset_general++;
            if(offset_general == tamanio_pagina){
                frame_a_escribir = paginacion_cambiar_frame(frame_a_escribir); 
                offset_general = 0;
                numero_de_frame = frame_a_escribir->num_frame;
                paginacion_modificar_tabla(patota, numero_de_frame, numero_de_pagina+1);
            }        
        }
        for(j = 0; j < 2; j++){
            if((offset_general + sizeof(uint32_t)) > tamanio_pagina){
                frame_a_escribir = paginacion_cambiar_frame(frame_a_escribir);
                offset_general = 0;
                numero_de_frame = frame_a_escribir->num_frame;
                paginacion_modificar_tabla(patota, numero_de_frame, numero_de_pagina+1);
            }    
            memcpy(esquema_memoria_mfisica + numero_de_frame*tamanio_pagina + offset_general, &prox_tarea_y_direccion_pcb[j], sizeof(uint32_t));
            offset_general = offset_general + sizeof(uint32_t);
        }
        if(offset_general == tamanio_pagina){
            frame_a_escribir = paginacion_cambiar_frame(frame_a_escribir); 
            offset_general = 0;
            numero_de_frame = frame_a_escribir->num_frame;
            paginacion_modificar_tabla(patota, numero_de_frame, numero_de_pagina+1);
        } 
    }
    U_LOG_TRACE("Proxima tarea y direccion pcb escrito con exito");
    //escribo las tareas
    int offset_tareas = 0;
    while(offset_tareas < strlen(tareas)+1){
        int start = tamanio_pagina * numero_de_frame + offset_general; 
        int end = start + minimo(tamanio_pagina - offset_general, strlen(tareas)+1 - offset_tareas);
        for(i=start; i<end; i++){
            esquema_memoria_mfisica[i] = tareas[offset_tareas];
            offset_tareas++;
            offset_general++;
            U_LOG_TRACE("Escrito caracter: %d", i);
        }
        if(offset_general == tamanio_pagina){
            frame_a_escribir = paginacion_cambiar_frame(frame_a_escribir); 
            offset_general = 0;
            paginacion_modificar_tabla(patota, numero_de_frame, numero_de_pagina+1);
        } 
    }
    U_LOG_TRACE("Tareas escritas con exito");
    paginacion_frame_modificado(frame_a_escribir);
    return true;
}

private p_frame_t* paginacion_cambiar_frame(p_frame_t* frame_a_cambiar){
    paginacion_frame_modificado(frame_a_cambiar);
    frame_a_cambiar = list_find(lista_frames_memoria, paginacion_frame_esta_libre);
    return frame_a_cambiar;
}

private int minimo(int x, int y){
    int respuesta = x<y ? x: y; 
}

private void paginacion_frame_modificado(p_frame_t* frame){
    frame->ocupado = 1;  //esta modificacion va aca? 
    frame->tiempo_en_memoria = contador_memoria + 1;
    frame->uso = 1; 
}

private void paginacion_modificar_tabla(p_patota_y_tabla_t* patota, int numero_de_frame, int numero_pagina){
    p_fila_tabla_de_paginas* fila = list_get(patota->tabla, numero_pagina);
    fila->num_pagina = numero_pagina;
    fila->frame_memoria = numero_de_frame;
    U_LOG_TRACE("Se agrego pagina a tabla de paginas");
}