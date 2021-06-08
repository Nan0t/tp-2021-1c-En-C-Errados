#include "paginacion.h"

private void paginacion_inicializar_frames_memoria(int);
private void paginacion_mostrar_frames(int);
private bool paginacion_tiene_frames_libres(int frames, t_list* lista_frames);
private int paginacion_frames_necesarios(int tamanio_data);
private bool paginacion_frame_esta_libre(p_frame_t* frame);
private void paginacion_agregar_patota_a_listado(uint32_t, uint32_t);
private bool paginacion_agregar_patota_en_memoria(uint32_t, uint32_t, const char*, int);
private p_frame_t* paginacion_cambiar_frame(p_frame_t*);
private int minimo(int, int);
private void paginacion_frame_modificado(p_frame_t* frame);

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
    int tamanio_data = 2*(sizeof(uint32_t)) + 21* cant_tripulantes + strlen(tareas); 
    int frames_necesarios = paginacion_frames_necesarios(tamanio_data);
    if(!paginacion_tiene_frames_libres(frames_necesarios, lista_frames_memoria)){ 
        return false;
    }
    U_LOG_TRACE("Hay frames necesarios"); //Chequear 
    paginacion_agregar_patota_a_listado(pid, cant_tripulantes);
    return paginacion_agregar_patota_en_memoria(pid, cant_tripulantes, tareas, tamanio_data);

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
    return tamanio_data/tamanio_pagina;
}

private bool paginacion_frame_esta_libre(p_frame_t* frame){
    bool respuesta=(frame->ocupado)==0? true : false;
}

private void paginacion_agregar_patota_a_listado(uint32_t pid, uint32_t cant_tripulantes){//ver si pasar las tareas
    p_patota_y_tabla_t* patota = u_malloc(sizeof(p_patota_y_tabla_t));
    patota->pid = pid;
    patota->direccion_tareas = sizeof(uint32_t) + 21 * cant_tripulantes;
    patota->tabla = list_create();
    list_add(listado_patotas, patota);   //mutex listado de patotas 
    U_LOG_TRACE("Se agrego patota con pid: %d, al listado de patotas", pid);
}

private bool paginacion_agregar_patota_en_memoria(uint32_t pid, uint32_t cant_tripulantes, const char* tareas, int tamanio_data){
    uint32_t pid_y_direccion_tareas[2] = {pid, 21 * cant_tripulantes};
    int offset_general = 0;
    int i;
    int j;
    p_frame_t* frame_a_escribir = list_find(lista_frames_memoria, paginacion_frame_esta_libre);
    
    //escribo el pid y direccion logica inicio de tareas
    for(i = 0; i<2; i++){
        memcpy(esquema_memoria_mfisica + offset_general, &pid_y_direccion_tareas[i], sizeof(uint32_t));//ver si estoy copiando bien
        offset_general = offset_general + sizeof(uint32_t);
        if(offset_general = tamanio_pagina){ 
            frame_a_escribir = paginacion_cambiar_frame(frame_a_escribir); // esto anda bien?  OJO; 
            offset_general = 0;
        }
    }
    U_LOG_TRACE("Pid: %d, y direccion de tareas:%d copiado con exito", pid_y_direccion_tareas[0], pid_y_direccion_tareas[1]);
    //hago espacio para tripulantes y escribo proxima tarea y direccion logica del pcb
    uint32_t prox_tarea_y_direccion_pcb[2] = {0, 0}; //direccion logica del pcb es 0?
    for(i=0; i<cant_tripulantes; i++){
        for(j=0; j<13; j++){
            offset_general++;
            if(offset_general = tamanio_pagina){
                frame_a_escribir = paginacion_cambiar_frame(frame_a_escribir); 
                offset_general = 0;
            }        
        }
        for(j = 0; j < 2; j++){
            if((offset_general + sizeof(uint32_t)) > tamanio_pagina){
                frame_a_escribir = paginacion_cambiar_frame(frame_a_escribir);
                offset_general = 0;
            }    
            memcpy(esquema_memoria_mfisica + offset_general, &prox_tarea_y_direccion_pcb[j], sizeof(uint32_t));
        }
        if(offset_general = tamanio_pagina){
            frame_a_escribir = paginacion_cambiar_frame(frame_a_escribir); 
            offset_general = 0;
        } 
    }
    U_LOG_TRACE("Proxima tarea y direccion pcb escrito con exito");
    //escribo las tareas
    int offset_tareas = 0;
    while(offset_tareas < strlen(tareas)+1){
        int start = tamanio_pagina * frame_a_escribir->num_frame + offset_general; 
        int end = start + minimo(tamanio_pagina - offset_general, offset_tareas);
        for(i=start; i<end; i++){
            esquema_memoria_mfisica[i] = tareas[offset_tareas];
            offset_tareas++;
        }
        if(offset_general = tamanio_pagina){
            frame_a_escribir = paginacion_cambiar_frame(frame_a_escribir); 
            offset_general = 0;
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