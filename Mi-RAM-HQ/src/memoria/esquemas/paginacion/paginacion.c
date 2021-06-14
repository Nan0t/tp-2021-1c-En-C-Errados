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
private void mostrar_tabla_de_paginas(int pid);
private p_patota_y_tabla_t* buscar_patota_por_pid(uint32_t pid);
private void paginacion_chequear_overflow_tripulante(int tamanio_dato, int *frame, int *desplazamiento, int *pagina, p_patota_y_tabla_t*);
private int paginacion_frame_correspondiente_a_pagina(int pagina, p_patota_y_tabla_t* patota);
private tripulantes_t* paginacion_obtener_tripulante_de_patota(p_patota_y_tabla_t* patota, int numero_de_tripulante);

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
    p_patota_y_tabla_t* patota = buscar_patota_por_pid(pid);
    uint32_t escritos = patota->tripulantes_escritos;
    int base = 8 + 21 * escritos; //correspondientes a la estructura del pcb y los tripulantes escritos en memoria;  
    int pagina = base / tamanio_pagina;
    int frame = paginacion_frame_correspondiente_a_pagina(pagina, patota);
    int desplazamiento = base % tamanio_pagina;
    char estado = 'N';
    uint32_t identificador_nueva_tarea = 0; 
    uint32_t puntero_pcb = 0;
  
    paginacion_chequear_overflow_tripulante(sizeof(uint32_t), &frame, &desplazamiento, &pagina, patota);
    memcpy(esquema_memoria_mfisica + frame * tamanio_pagina + desplazamiento, &tid, sizeof(uint32_t));
    desplazamiento = desplazamiento + sizeof(uint32_t);

    paginacion_chequear_overflow_tripulante(sizeof(char), &frame, &desplazamiento, &pagina, patota);
    memcpy(esquema_memoria_mfisica + frame * tamanio_pagina + desplazamiento, &estado, sizeof(char));
    desplazamiento = desplazamiento + sizeof(char);

    
    uint32_t posicion_tarea_pcb[4] = {pos.x, pos.y, identificador_nueva_tarea, puntero_pcb};
    int i;
    for(i=0; i<4; i++){
        paginacion_chequear_overflow_tripulante(sizeof(uint32_t), &frame, &desplazamiento, &pagina, patota);
        memcpy(esquema_memoria_mfisica + frame * tamanio_pagina + desplazamiento, &posicion_tarea_pcb[i], sizeof(uint32_t));
        desplazamiento = desplazamiento + sizeof(uint32_t);
    }
    patota->tripulantes_escritos = escritos + 1;
    U_LOG_TRACE("Tripulante con tid %d guardado correctamente", tid);
    
//AGREGADO PARA PROBAR

    tripulantes_t* a_mostrar = paginacion_memoria_obtener_info_tripulante(pid, tid);
    U_LOG_TRACE("INFO RECUPERADA pid: %d, tid: %d, pos x: %d, pos y: %d, estado:%c", a_mostrar->pid, a_mostrar->tid, a_mostrar->pos.x, a_mostrar->pos.y, a_mostrar->estado);
    free(a_mostrar);

//
    return true;
}

bool paginacion_memoria_actualizar_posicion_tripulante(uint32_t pid, uint32_t tid, u_pos_t pos)
{
    p_patota_y_tabla_t* patota = buscar_patota_por_pid(pid);
    int base = 8; //correspondientes a la estructura del pcb y el inicio de los tripulantes escritos en memoria;  
    int pagina = base / tamanio_pagina;
    int frame = paginacion_frame_correspondiente_a_pagina(pagina, patota); //chequear si la pagina esta en memoria. 
    int desplazamiento = base % tamanio_pagina;
    bool tid_encontrado = false; 
    int i;
    uint32_t tid_comparado;

    while(!tid_encontrado){
        paginacion_chequear_overflow_tripulante(sizeof(uint32_t), &frame, &desplazamiento, &pagina, patota); //cuando cambio de pagina chequear si la pagina esta en memoria
        
        memcpy(&tid_comparado, esquema_memoria_mfisica + frame * tamanio_pagina + desplazamiento, sizeof(uint32_t));
        desplazamiento = desplazamiento + sizeof(uint32_t);
        if(tid_comparado == tid){
            tid_encontrado = true;

            paginacion_chequear_overflow_tripulante(1, &frame, &desplazamiento, &pagina, patota);
            desplazamiento++;

            paginacion_chequear_overflow_tripulante(sizeof(uint32_t), &frame, &desplazamiento, &pagina, patota);
            memcpy(esquema_memoria_mfisica + frame * tamanio_pagina + desplazamiento, &(pos.x), sizeof(uint32_t));
            desplazamiento = desplazamiento + sizeof(uint32_t);

            paginacion_chequear_overflow_tripulante(1, &frame, &desplazamiento, &pagina, patota);
            memcpy(esquema_memoria_mfisica + frame * tamanio_pagina + desplazamiento, &(pos.y), sizeof(uint32_t));
            
        }else{ 
            for(i=0; i<17; i++){
                paginacion_chequear_overflow_tripulante(1, &frame, &desplazamiento, &pagina, patota);
                desplazamiento++;
            }
        }
    }
    U_LOG_INFO("Actualizado posicion de tid: %d", tid);
    return true;
}

bool paginacion_memoria_actualizar_estado_tripulante(uint32_t pid, uint32_t tid, char estado)
{
    p_patota_y_tabla_t* patota = buscar_patota_por_pid(pid);
    int base = 8; //correspondientes a la estructura del pcb y el inicio de los tripulantes escritos en memoria;  
    int pagina = base / tamanio_pagina;
    int frame = paginacion_frame_correspondiente_a_pagina(pagina, patota); //chequear si la pagina esta en memoria. 
    int desplazamiento = base % tamanio_pagina;
    bool tid_encontrado = false; 
    int i;
    uint32_t tid_comparado;

    while(!tid_encontrado){
        paginacion_chequear_overflow_tripulante(sizeof(uint32_t), &frame, &desplazamiento, &pagina, patota); //cuando cambio de pagina chequear si la pagina esta en memoria
        
        memcpy(&tid_comparado, esquema_memoria_mfisica + frame * tamanio_pagina + desplazamiento, sizeof(uint32_t));
        desplazamiento = desplazamiento + sizeof(uint32_t);
        if(tid_comparado == tid){
            tid_encontrado = true;

            paginacion_chequear_overflow_tripulante(1, &frame, &desplazamiento, &pagina, patota);
            memcpy(esquema_memoria_mfisica + frame * tamanio_pagina + desplazamiento, &estado, sizeof(char));
            
        }else{ 
            for(i=0; i<17; i++){
                paginacion_chequear_overflow_tripulante(1, &frame, &desplazamiento, &pagina, patota);
                desplazamiento++;
            }
        }
    }
    U_LOG_INFO("Actualizado estado de tid: %d", tid);
    return true;
}

char* paginacion_memoria_obtener_proxima_tarea(uint32_t pid, uint32_t tid)
{

}

tripulantes_t* paginacion_memoria_obtener_info_tripulante(uint32_t pid, uint32_t tid)
{
   
    tripulantes_t* tripulante = u_malloc(sizeof(tripulantes_t));

    p_patota_y_tabla_t* patota = buscar_patota_por_pid(pid);
    int base = 8; //correspondientes a la estructura del pcb y el inicio de los tripulantes escritos en memoria;  
    int pagina = base / tamanio_pagina;
    int frame = paginacion_frame_correspondiente_a_pagina(pagina, patota);
    int desplazamiento = base % tamanio_pagina;
    bool tid_encontrado = false; 
    int i;
    uint32_t tid_comparado;

    while(!tid_encontrado){
        paginacion_chequear_overflow_tripulante(sizeof(uint32_t), &frame, &desplazamiento, &pagina, patota);
        
        memcpy(&tid_comparado, esquema_memoria_mfisica + frame * tamanio_pagina + desplazamiento, sizeof(uint32_t));
        desplazamiento = desplazamiento + sizeof(uint32_t);
        if(tid_comparado == tid){
            tid_encontrado = true;
            tripulante->tid = tid_comparado;
            tripulante->pid = pid;

            paginacion_chequear_overflow_tripulante(sizeof(char), &frame, &desplazamiento, &pagina, patota);
            memcpy(&(tripulante->estado), esquema_memoria_mfisica+ frame * tamanio_pagina + desplazamiento, sizeof(char));
            desplazamiento++;
            
            paginacion_chequear_overflow_tripulante(sizeof(uint32_t), &frame, &desplazamiento, &pagina, patota);
            memcpy(&(tripulante->pos.x), esquema_memoria_mfisica + frame * tamanio_pagina + desplazamiento, sizeof(uint32_t));
            desplazamiento = desplazamiento + sizeof(uint32_t);

            paginacion_chequear_overflow_tripulante(sizeof(uint32_t), &frame, &desplazamiento, &pagina, patota);
            memcpy(&(tripulante->pos.y), esquema_memoria_mfisica + frame * tamanio_pagina + desplazamiento, sizeof(uint32_t));
            desplazamiento = desplazamiento + sizeof(uint32_t);
            

        }else{
            for(i=0; i<17; i++){
                paginacion_chequear_overflow_tripulante(1, &frame, &desplazamiento, &pagina, patota);
                desplazamiento++;
            }
        }
    }


    return tripulante;
}

t_list*        paginacion_memoria_obtener_todos_los_tripulantes(void)
{
    t_list* lista_de_todos_los_tripulantes = list_create();
    int cantidad_patotas = list_size(listado_patotas);
    int i, j;
    for(i=0; i<cantidad_patotas; i++){
        p_patota_y_tabla_t* patota = list_get(listado_patotas, i);
        int cantidad_tripulantes = patota->tripulantes_escritos;
        for(j=0; j<cantidad_tripulantes; j++){
            tripulantes_t* tripulante = paginacion_obtener_tripulante_de_patota(patota, j);
            list_add(lista_de_todos_los_tripulantes, tripulante);
        }
    }
    return lista_de_todos_los_tripulantes;
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
    patota->tripulantes_escritos = 0;
    patota->tabla = list_create();
    for(i=0; i<paginas; i++){
        p_fila_tabla_de_paginas_t* fila = u_malloc(sizeof(p_fila_tabla_de_paginas_t));
        fila->num_pagina = i;
        list_add(patota->tabla, fila);
    }
    list_add(listado_patotas, patota);   //mutex listado de patotas 
    U_LOG_INFO("Se agrego patota con pid: %d, al listado de patotas", pid);
    return patota; 
}

private bool paginacion_agregar_patota_en_memoria(uint32_t pid, uint32_t cant_tripulantes, const char* tareas, int tamanio_data, p_patota_y_tabla_t* patota){
    uint32_t pid_y_direccion_tareas[2] = {pid, 8 + 21 * cant_tripulantes};
    int offset_general = 0;
    int i;
    int j;
    p_frame_t* frame_a_escribir = list_find(lista_frames_memoria, paginacion_frame_esta_libre);
    int numero_de_frame = frame_a_escribir->num_frame;
    int numero_de_pagina = 0;
    paginacion_modificar_tabla(patota, numero_de_frame, numero_de_pagina);
    //escribo el pid y direccion logica inicio de tareas
    for(i = 0; i<2; i++){
        memcpy(esquema_memoria_mfisica + numero_de_frame*tamanio_pagina + offset_general, &pid_y_direccion_tareas[i], sizeof(uint32_t));//ver si estoy copiando bien
        offset_general = offset_general + sizeof(uint32_t);
       
        if(offset_general == tamanio_pagina){  
            frame_a_escribir = paginacion_cambiar_frame(frame_a_escribir); 
            numero_de_frame = frame_a_escribir->num_frame;
            offset_general = 0;
            numero_de_pagina ++;
            paginacion_modificar_tabla(patota, numero_de_frame, numero_de_pagina);
        }
    }
    U_LOG_TRACE("Pid: %d, y direccion de tareas: %d copiado con exito", pid_y_direccion_tareas[0], pid_y_direccion_tareas[1]);
    //hago espacio para tripulantes y escribo proxima tarea y direccion logica del pcb
    uint32_t prox_tarea_y_direccion_pcb[2] = {0, 0}; //direccion logica del pcb es 0?
    for(i=0; i<cant_tripulantes; i++){
        for(j=0; j<21; j++){
            offset_general++;
            if(offset_general == tamanio_pagina){
                frame_a_escribir = paginacion_cambiar_frame(frame_a_escribir); 
                offset_general = 0;
                numero_de_frame = frame_a_escribir->num_frame;
                numero_de_pagina ++;
                paginacion_modificar_tabla(patota, numero_de_frame, numero_de_pagina);
            }        
        }
    }
    //escribo las tareas
    int offset_tareas = 0;
    while(offset_tareas < strlen(tareas)+1){
        int start = tamanio_pagina * numero_de_frame + offset_general; 
        int end = start + minimo(tamanio_pagina - offset_general, strlen(tareas)+1 - offset_tareas);
        for(i=start; i<end; i++){
            
            memcpy(esquema_memoria_mfisica + i, &tareas[offset_tareas], sizeof(char)); //agregado
            offset_tareas++;
            offset_general++;

        }
        if(offset_general == tamanio_pagina){
            frame_a_escribir = paginacion_cambiar_frame(frame_a_escribir); 
            offset_general = 0;
            numero_de_frame = frame_a_escribir->num_frame;
            numero_de_pagina ++;
            paginacion_modificar_tabla(patota, numero_de_frame, numero_de_pagina);
        } 
    }
    U_LOG_TRACE("Tareas escritas con exito");
    paginacion_frame_modificado(frame_a_escribir);
    paginacion_mostrar_frames(esquema_memoria_tamanio/tamanio_pagina); //sacar
    mostrar_tabla_de_paginas(pid); //sacar

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
    p_fila_tabla_de_paginas_t* fila = list_get(patota->tabla, numero_pagina);
    fila->num_pagina = numero_pagina;
    fila->frame_memoria = numero_de_frame;
    //agregar frame swap 
    U_LOG_TRACE("Se agrego pagina a tabla de paginas");
}

private void mostrar_tabla_de_paginas(int pid){
    bool comparar_pid(p_patota_y_tabla_t* patota){
        if(patota->pid==pid){
            return true;
        }
        return false;
    };
    p_patota_y_tabla_t* a_mostrar = list_find(listado_patotas, comparar_pid);
    int i;
    for(i=0; i<(list_size(a_mostrar->tabla)); i++){
        p_fila_tabla_de_paginas_t* fila = list_get(a_mostrar->tabla, i);
        U_LOG_TRACE("Mostrando tabla de pid: %d, pagina: %d, frame: %d", a_mostrar->pid, fila->num_pagina, fila->frame_memoria);
    }
}

private p_patota_y_tabla_t* buscar_patota_por_pid(uint32_t pid){
    bool buscar_patota(p_patota_y_tabla_t* patota){
        if(pid == patota->pid){
            return true;
        }
        return false;
    };
    p_patota_y_tabla_t* patota_buscada = list_find(listado_patotas, buscar_patota);
    return patota_buscada; 
}

private void paginacion_chequear_overflow_tripulante(int tamanio_dato, int *frame, int *desplazamiento, int *pagina, p_patota_y_tabla_t* patota){
    if((*desplazamiento)+tamanio_dato>tamanio_pagina){
        *frame = paginacion_frame_correspondiente_a_pagina((*pagina)+1, patota);
        *desplazamiento = 0;
        *pagina = (*pagina) + 1;
    }
}

private int paginacion_frame_correspondiente_a_pagina(int pagina, p_patota_y_tabla_t* patota){
    bool paginacion_buscar_frame_de_pagina(p_fila_tabla_de_paginas_t* fila_recibida){
        if(fila_recibida->num_pagina==pagina){
            return true;
        }
            return false;
    };
    p_fila_tabla_de_paginas_t* fila = list_find(patota->tabla, paginacion_buscar_frame_de_pagina);
    int frame = fila->frame_memoria;
    return frame;
}

private tripulantes_t* paginacion_obtener_tripulante_de_patota(p_patota_y_tabla_t* patota, int numero_de_tripulante){
    tripulantes_t* tripulante = u_malloc(sizeof(tripulantes_t));

    int base = 8 + 21 * numero_de_tripulante; //correspondientes a la estructura del pcb y los tripulantes escritos en memoria;  
    int pagina = base / tamanio_pagina;
    int frame = paginacion_frame_correspondiente_a_pagina(pagina, patota);
    int desplazamiento = base % tamanio_pagina;
    int i;
    tripulante->pid = patota->pid;
    
    paginacion_chequear_overflow_tripulante(sizeof(uint32_t), &frame, &desplazamiento, &pagina, patota);
        
    memcpy(&(tripulante->tid), esquema_memoria_mfisica + frame * tamanio_pagina + desplazamiento, sizeof(uint32_t)); //ojo aca
    desplazamiento = desplazamiento + sizeof(uint32_t);

    paginacion_chequear_overflow_tripulante(sizeof(char), &frame, &desplazamiento, &pagina, patota);
    memcpy(&(tripulante->estado), esquema_memoria_mfisica+ frame * tamanio_pagina + desplazamiento, sizeof(char));
    desplazamiento++;
            
    paginacion_chequear_overflow_tripulante(sizeof(uint32_t), &frame, &desplazamiento, &pagina, patota);
    memcpy(&(tripulante->pos.x), esquema_memoria_mfisica + frame * tamanio_pagina + desplazamiento, sizeof(uint32_t));
    desplazamiento = desplazamiento + sizeof(uint32_t);

    paginacion_chequear_overflow_tripulante(sizeof(uint32_t), &frame, &desplazamiento, &pagina, patota);
    memcpy(&(tripulante->pos.y), esquema_memoria_mfisica + frame * tamanio_pagina + desplazamiento, sizeof(uint32_t));
    desplazamiento = desplazamiento + sizeof(uint32_t);
            

    return tripulante;
}
