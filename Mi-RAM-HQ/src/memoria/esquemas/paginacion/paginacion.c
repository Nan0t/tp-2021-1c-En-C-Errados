#include "paginacion.h"

typedef enum
{
    NUEVA_ESCRITURA,
    MODIFICACION,
    ELIMINACION,
    DE_SWAP_A_REAL,
    DE_REAL_A_SWAP
}p_tipo_escritura_e;

typedef enum
{
    MEMORIA_SWAP,
    MEMORIA_FISICA
}p_tipo_memoria_e;

private void paginacion_inicializar_frames(int, t_list*);
private void paginacion_mostrar_frames(int, p_tipo_memoria_e);
private bool paginacion_tiene_frames_libres(int frames, t_list* lista_frames);
private int paginacion_frames_necesarios(int tamanio_data);
private bool paginacion_frame_esta_libre(p_frame_t* frame);
private p_patota_y_tabla_t* paginacion_agregar_patota_a_listado(uint32_t, int, int);
private bool paginacion_agregar_patota_en_memoria_swap(uint32_t, uint32_t, const char*, p_patota_y_tabla_t*);
private void paginacion_modificar_frame(int32_t, p_tipo_escritura_e, p_patota_y_tabla_t* patota);
private void paginacion_modificar_frame_de_swap(int numero_de_frame, p_tipo_escritura_e escritura, p_patota_y_tabla_t* patota); //agregado para swap
//private void paginacion_modificar_tabla(p_patota_y_tabla_t* patota, int numero_de_frame, int numero_pagina);
private void mostrar_tabla_de_paginas(uint32_t pid);
private p_patota_y_tabla_t* buscar_patota_por_pid(uint32_t pid);
private bool paginacion_chequear_overflow_de_pagina(int tamanio_dato, int *frame, int *desplazamiento, int *pagina, p_patota_y_tabla_t*, p_tipo_escritura_e, p_tipo_memoria_e);
private int32_t paginacion_frame_correspondiente_a_pagina(uint32_t pagina, p_patota_y_tabla_t* patota, p_tipo_memoria_e);
private tripulantes_t* paginacion_obtener_tripulante_de_patota(p_patota_y_tabla_t* patota, int numero_de_tripulante);
private char* paginacion_obtener_tareas_de_pid(uint32_t pid);
private tripulantes_t* paginacion_obtener_tcb(uint32_t pid, uint32_t tid);
private void paginacion_modificar_proxima_tarea_tripulante(uint32_t pid, uint32_t tid);
private p_fila_tabla_de_paginas_t* buscar_fila_por_frame(t_list* tabla, int numero_de_frame);
private void paginacion_guardar_direccion_logica_tcb(int desplazamiento, int tid, p_patota_y_tabla_t* patota);
private int paginacion_buscar_tid_en_tabla_de_paginas(uint32_t tid, p_patota_y_tabla_t* patota);
private int paginacion_page_fault(int pagina, p_patota_y_tabla_t* patota);
private int paginacion_copiar_frame_de_swap_a_memoria(int pagina, p_patota_y_tabla_t* patota);
private int paginacion_buscar_direccion_en_tabla_de_paginas(int direccion_logica, p_patota_y_tabla_t* patota);
private int paginacion_liberar_un_frame(int pagina, p_patota_y_tabla_t* patota);
private void paginacion_modificar_frame_y_tabla_de_paginas(int numero_de_frame, p_patota_y_tabla_t* patota, int pagina);
private bool paginacion_chequear_overflow_de_pagina_en_memoria(int tamanio_data, int * direccion_fisica, p_patota_y_tabla_t* patota);
private tripulantes_t* paginacion_memoria_obtener_info_tripulante_swap(int pid, int tid);

private void paginacion_escribir_uint32(uint32_t a_escribir, int* desplazamiento, int* direccion_fisica, int* direccion_logica, p_patota_y_tabla_t* patota, p_tipo_memoria_e, p_tipo_escritura_e);
private bool paginacion_chequear_overflow_de_pagina_byte_a_byte(int *direccion_fisica, int *desplazamiento, int *direccion_logica, p_patota_y_tabla_t* patota, p_tipo_escritura_e tipo_escritura, p_tipo_memoria_e tipo_de_memoria);
private bool paginacion_hay_overflow(int desplazamiento);
private void paginacion_obtener_uint32_de_memoria(uint32_t* donde_escribo, int* desplazamiento, int* direccion_fisica, int* direccion_logica, p_patota_y_tabla_t* patota, p_tipo_escritura_e tipo_escritura);
private void paginacion_obtener_char_de_memoria(char* donde_escribo, int *direccion_fisica, int * desplazamiento, int *direccion_logica, p_patota_y_tabla_t* patota, p_tipo_escritura_e tipo_escritura, p_tipo_memoria_e tipo_de_memoria);
private void paginacion_escribir_char(char a_escribir, int *direccion_fisica, int * desplazamiento, int *direccion_logica, p_patota_y_tabla_t* patota, p_tipo_escritura_e tipo_escritura, p_tipo_memoria_e tipo_de_memoria);
private p_frame_t* paginacion_encontrar_frame_libre(p_tipo_memoria_e* tipo_de_memoria);
private bool paginacion_tiene_frames_libres_totales(int frames);
private int32_t paginacion_inicializacion_frame_correspondiente_a_pagina(uint32_t pagina, p_patota_y_tabla_t* patota, p_tipo_memoria_e* tipo_de_memoria);
private void paginacion_inicializacion_modificar_frame(int numero_de_frame, p_tipo_escritura_e escritura, p_patota_y_tabla_t* patota, p_tipo_memoria_e tipo_de_memoria);
private bool paginacion_inicializacion_chequear_overflow_de_pagina_byte_a_byte(int *direccion_fisica, int *desplazamiento, int *direccion_logica, p_patota_y_tabla_t* patota, p_tipo_escritura_e tipo_escritura, p_tipo_memoria_e* tipo_de_memoria);
private void paginacion_inicializacion_escribir_char(char a_escribir, int *direccion_fisica, int * desplazamiento, int *direccion_logica, p_patota_y_tabla_t* patota, p_tipo_escritura_e tipo_escritura, p_tipo_memoria_e* tipo_de_memoria);
private void paginacion_inicializacion_escribir_uint32(uint32_t a_escribir, int* desplazamiento, int* direccion_fisica, int* direccion_logica, p_patota_y_tabla_t* patota, p_tipo_memoria_e* tipo_memoria, p_tipo_escritura_e tipo_escritura);
private p_fila_tabla_de_paginas_t* buscar_fila_en_tabla_de_paginas_segun_frame_memoria_real(p_patota_y_tabla_t* patota, int numero_de_frame);
private void paginacion_marcar_como_liberado(int numero_de_frame, t_list* lista_frames);
private void paginacion_copiar_frame_de_memoria_a_swap(p_fila_tabla_de_paginas_t* fila_de_tabla);
private bool paginacion_pagina_esta_en_memoria_real(p_fila_tabla_de_paginas_t* fila);
private p_fila_tabla_de_paginas_t* paginacion_seleccionar_pagina_por_LRU();
private p_fila_tabla_de_paginas_t* paginacion_seleccionar_pagina_por_CLOCK();

void* memoria_swap_fisica;
char* algoritmo_reemplazo;

void paginacion_memoria_init(void)
{
    esquema_memoria_tamanio = u_config_get_int_value("TAMANIO_MEMORIA");
    esquema_memoria_mfisica = u_malloc(esquema_memoria_tamanio);
    listado_patotas = list_create();
    lista_frames_memoria = list_create();
    lista_frames_swap = list_create();
    tamanio_pagina = u_config_get_int_value("TAMANIO_PAGINA");
    tamanio_swap = u_config_get_int_value("TAMANIO_SWAP");
    algoritmo_reemplazo = u_config_get_string_value("ALGORITMO_REEMPLAZO");
    memoria_swap_fisica = u_malloc(tamanio_swap);
    contador_memoria = 0;
    //inicializo frames memoria
    paginacion_inicializar_frames(esquema_memoria_tamanio/tamanio_pagina, lista_frames_memoria);
    //inicializar swap; 
    paginacion_inicializar_frames(tamanio_swap/tamanio_pagina, lista_frames_swap);
    //paginacion_mostrar_frames(esquema_memoria_tamanio/tamanio_pagina, MEMORIA_FISICA);

   // paginacion_mostrar_frames(tamanio_swap/tamanio_pagina, MEMORIA_SWAP);
}

bool paginacion_memoria_inicializar_patota(uint32_t pid, uint32_t cant_tripulantes, const char* tareas)
{
    //8 tamaño pcb + 21 por cada uno de los tripulantes + el tamaño de las tareas. 
    int tamanio_data = 2*(sizeof(uint32_t)) + 21* cant_tripulantes + strlen(tareas)+1; 
    int frames_necesarios = paginacion_frames_necesarios(tamanio_data);
    U_LOG_TRACE("Frames necesarios %d", frames_necesarios);
    if(!paginacion_tiene_frames_libres_totales(frames_necesarios)){  //modificado 
        return false;
    }
    U_LOG_TRACE("Hay frames necesarios en swap"); //Chequear 
    p_patota_y_tabla_t* patota = paginacion_agregar_patota_a_listado(pid, frames_necesarios, strlen(tareas)+1);

   // paginacion_mostrar_frames(tamanio_swap/tamanio_pagina, MEMORIA_SWAP); //agregado

    return paginacion_agregar_patota_en_memoria_swap(pid, cant_tripulantes, tareas, patota);

}

bool paginacion_memoria_inicializar_tripulante(uint32_t pid, uint32_t tid, u_pos_t pos)
{
    p_patota_y_tabla_t* patota = buscar_patota_por_pid(pid);
    uint32_t escritos = patota->tripulantes_escritos;
    int direccion_logica = 8 + 21 * escritos; //correspondientes a la estructura del pcb y los tripulantes escritos en memoria;  
    int pagina = direccion_logica / tamanio_pagina;
    int desplazamiento = direccion_logica % tamanio_pagina;
    
    p_tipo_memoria_e tipo_de_memoria;
    int frame = paginacion_inicializacion_frame_correspondiente_a_pagina(pagina, patota, &tipo_de_memoria);
    //p_tipo_memoria_e tipo_de_memoria = MEMORIA_SWAP;
   // int frame = paginacion_frame_correspondiente_a_pagina(pagina, patota, tipo_de_memoria);
    int direccion_fisica = frame * tamanio_pagina + desplazamiento;

    p_tipo_escritura_e tipo_escritura = NUEVA_ESCRITURA; //agregado
    if(!paginacion_hay_overflow(desplazamiento+1)){
        paginacion_inicializacion_modificar_frame(frame, tipo_escritura, patota, tipo_de_memoria); 
    }
    
    char estado = 'N';
    uint32_t identificador_nueva_tarea = 0; 
    uint32_t puntero_pcb = 0;
    
    paginacion_guardar_direccion_logica_tcb(direccion_logica, tid, patota);
    paginacion_inicializacion_escribir_uint32(tid, &desplazamiento, &direccion_fisica, &direccion_logica, patota, &tipo_de_memoria, tipo_escritura);
    
    /*
    paginacion_inicializacion_chequear_overflow_de_pagina_byte_a_byte(&direccion_fisica, &desplazamiento, &direccion_logica, patota, tipo_escritura, &tipo_de_memoria);
    memcpy(memoria_swap_fisica + direccion_fisica, &estado, sizeof(char));
    U_LOG_TRACE("ESCRITO %c en frame %d, direccion logica %d, direccion fisica %d", estado, direccion_fisica/tamanio_pagina, direccion_logica, direccion_fisica);
    desplazamiento = desplazamiento + sizeof(char);
    direccion_fisica = direccion_fisica + sizeof(char);
    direccion_logica = direccion_logica + sizeof(char);*/
    paginacion_inicializacion_escribir_char(estado, &direccion_fisica, &desplazamiento, &direccion_logica, patota, tipo_escritura, &tipo_de_memoria);

    
    uint32_t posicion_tarea_pcb[4] = {pos.x, pos.y, identificador_nueva_tarea, puntero_pcb};
    int i;
    for(i=0; i<4; i++){
        paginacion_inicializacion_escribir_uint32(posicion_tarea_pcb[i], &desplazamiento, &direccion_fisica, &direccion_logica, patota, &tipo_de_memoria, tipo_escritura);
    }
    patota->tripulantes_escritos = escritos + 1;
    U_LOG_TRACE("Tripulante con tid %d guardado correctamente", tid);
    
//AGREGADO PARA PROBAR
/*
    tripulantes_t* a_mostrar = paginacion_memoria_obtener_info_tripulante_swap(pid, tid);
    U_LOG_TRACE("INFO RECUPERADA pid: %d, tid: %d, pos x: %d, pos y: %d, estado:%c", a_mostrar->pid, a_mostrar->tid, a_mostrar->pos.x, a_mostrar->pos.y, a_mostrar->estado);
    free(a_mostrar);
*/

 //   paginacion_mostrar_frames(tamanio_swap/tamanio_pagina, tipo_de_memoria); //agregado
    return true;
}

bool paginacion_memoria_actualizar_posicion_tripulante(uint32_t pid, uint32_t tid, u_pos_t pos)
{
    p_patota_y_tabla_t* patota = buscar_patota_por_pid(pid);

    int direccion_logica = paginacion_buscar_tid_en_tabla_de_paginas(tid, patota);
    U_LOG_INFO("Direccion Fisica Tripulante: %d", direccion_logica);

    int pagina = direccion_logica / tamanio_pagina;
    int desplazamiento = direccion_logica % tamanio_pagina;
    int direccion_fisica = paginacion_buscar_direccion_en_tabla_de_paginas(direccion_logica, patota);
    
    p_tipo_escritura_e tipo_escritura = MODIFICACION;
    p_tipo_memoria_e tipo_memoria = MEMORIA_FISICA;
    paginacion_modificar_frame(direccion_fisica/tamanio_pagina, tipo_escritura, patota);

    int i;
    for(i=0; i<sizeof(uint32_t)+sizeof(char); i++){
        if(!paginacion_chequear_overflow_de_pagina_byte_a_byte(&direccion_fisica, &desplazamiento, &direccion_logica, patota, tipo_escritura, MEMORIA_FISICA)){
            //falta modificar frame?
            direccion_fisica++;
            direccion_logica++;
            desplazamiento++;
        } 
    }

    paginacion_escribir_uint32(pos.x, &desplazamiento, &direccion_fisica, &direccion_logica, patota, tipo_memoria, tipo_escritura);
    paginacion_escribir_uint32(pos.y, &desplazamiento, &direccion_fisica, &direccion_logica, patota, tipo_memoria, tipo_escritura);
    

    U_LOG_INFO("Actualizado posicion de tid: %d", tid);
    //paginacion_mostrar_frames(4, MEMORIA_FISICA);
    return true;
}

bool paginacion_memoria_actualizar_estado_tripulante(uint32_t pid, uint32_t tid, char estado)
{
    p_patota_y_tabla_t* patota = buscar_patota_por_pid(pid);

    int direccion_logica = paginacion_buscar_tid_en_tabla_de_paginas(tid, patota);
    U_LOG_INFO("Direccion Fisica Tripulante: %d", direccion_logica);

    int desplazamiento = direccion_logica % tamanio_pagina;
    int direccion_fisica = paginacion_buscar_direccion_en_tabla_de_paginas(direccion_logica, patota);

    
    
    p_tipo_escritura_e tipo_escritura = MODIFICACION;
    paginacion_modificar_frame(direccion_fisica/tamanio_pagina, tipo_escritura, patota);

    int i;
    for(i=0; i<sizeof(uint32_t); i++){
        if(!paginacion_chequear_overflow_de_pagina_byte_a_byte(&direccion_fisica, &desplazamiento, &direccion_logica, patota, tipo_escritura, MEMORIA_FISICA)){
            //falta modificar frame?
            direccion_fisica++;
            direccion_logica++;
            desplazamiento++;
        } 
    }

    paginacion_chequear_overflow_de_pagina_byte_a_byte(&direccion_fisica, &desplazamiento, &direccion_logica, patota, tipo_escritura, MEMORIA_FISICA);
    memcpy(esquema_memoria_mfisica + direccion_fisica, &estado, sizeof(char));
    U_LOG_TRACE("ESCRITO %c en frame %d, direccion logica %d, direccion fisica %d", estado, direccion_fisica/tamanio_pagina, direccion_logica, direccion_fisica);
    direccion_fisica++;
    direccion_logica++;
    desplazamiento++;
    

    U_LOG_INFO("Actualizado estado de tid: %d", tid);
    
    return true;
}

char* paginacion_memoria_obtener_proxima_tarea(uint32_t pid, uint32_t tid)
{
    char* todas_las_tareas = paginacion_obtener_tareas_de_pid(pid);
    U_LOG_TRACE("TAREAS RECUPERADAS: %s", todas_las_tareas);
    
    tripulantes_t* tripulante = paginacion_obtener_tcb(pid, tid);
    uint32_t numero_de_tarea = tripulante->proxima_tarea;
    free(tripulante);

    paginacion_modificar_proxima_tarea_tripulante(pid, tid);

    char** tareas_separadas = string_split(todas_las_tareas, "\n"); 
    free(todas_las_tareas);

    U_LOG_INFO("PROXIMA TAREA DE TRIPULANTE: %d, %s", tid, tareas_separadas[numero_de_tarea]);
    
    return tareas_separadas[numero_de_tarea];
}

tripulantes_t* paginacion_memoria_obtener_info_tripulante(uint32_t pid, uint32_t tid)
{
   
    tripulantes_t* tripulante = paginacion_obtener_tcb(pid, tid);

    return tripulante;
}

t_list*        paginacion_memoria_obtener_todos_los_tripulantes(void)
{
    t_list* lista_de_todos_los_tripulantes = list_create();
    int cantidad_patotas = list_size(listado_patotas);
    int i, j;

    for(i=0; i<cantidad_patotas; i++){
        p_patota_y_tabla_t* patota = list_get(listado_patotas, i);
        int direcciones_tcb = list_size(patota->direcciones_logicas);
        for(j=0; j<direcciones_tcb; j++){
            p_direcciones_logicas_t* direccion = list_get(patota->direcciones_logicas, j);
            tripulantes_t* tripulante = paginacion_obtener_tripulante_de_patota(patota, direccion->direccion_logica);
            list_add(lista_de_todos_los_tripulantes, tripulante);
        }
    }
    return lista_de_todos_los_tripulantes;
}

bool           paginacion_memoria_expulsar_tripulante(uint32_t pid, uint32_t tid)
{

    p_patota_y_tabla_t* patota = buscar_patota_por_pid(pid);
    int base = 8; //correspondientes a la estructura del pcb y el inicio de los tripulantes escritos en memoria;  
    int pagina = base / tamanio_pagina;
    int frame = paginacion_frame_correspondiente_a_pagina(pagina, patota, MEMORIA_FISICA);
    int desplazamiento = base % tamanio_pagina;
    bool tid_encontrado = false; 
    int i;
    uint32_t tid_comparado;

    p_tipo_escritura_e tipo_escritura = ELIMINACION; //agregado

    if(!paginacion_chequear_overflow_de_pagina(sizeof(uint32_t), &frame, &desplazamiento, &pagina, patota, tipo_escritura, MEMORIA_FISICA)){
        paginacion_modificar_frame(frame, MODIFICACION, patota);
    }
    
    while(!tid_encontrado){
        paginacion_chequear_overflow_de_pagina(sizeof(uint32_t), &frame, &desplazamiento, &pagina, patota, tipo_escritura, MEMORIA_FISICA);
        
        memcpy(&tid_comparado, esquema_memoria_mfisica + frame * tamanio_pagina + desplazamiento, sizeof(uint32_t));
        desplazamiento = desplazamiento + sizeof(uint32_t);
        if(tid_comparado == tid){
            tid_encontrado = true;
            paginacion_modificar_frame(frame, tipo_escritura, patota);
            for(i=0; i<17; i++){
                paginacion_chequear_overflow_de_pagina(1, &frame, &desplazamiento, &pagina, patota, tipo_escritura, MEMORIA_FISICA); //plantear una cuarta opcion que no modifique el bit de uso?
                desplazamiento++;
            }
        }else{
            for(i=0; i<17; i++){
                paginacion_chequear_overflow_de_pagina(1, &frame, &desplazamiento, &pagina, patota, MODIFICACION, MEMORIA_FISICA); //plantear una cuarta opcion que no modifique el bit de uso?
                desplazamiento++;
            }
        }
    }
    //paginacion_mostrar_frames(esquema_memoria_tamanio/tamanio_pagina); //agregado
    return true;
}

//---------------------------------------------------------------------------------------

private void paginacion_inicializar_frames(int cantidad_paginas, t_list* lista_frames){ //para frames de swap deberia usar otro?
    int i;
    for(i=0; i<cantidad_paginas; i++){
       p_frame_t* frame = u_malloc(sizeof(p_frame_t));
       frame->num_frame = i;
       frame->ocupado = 0;
       frame->ocupantes_frame = 0; //agregado para expulsar_tripulante
       list_add(lista_frames, frame);
    }
}

private void paginacion_mostrar_frames(int paginas, p_tipo_memoria_e tipo_de_memoria){
    t_list* lista;
    if(tipo_de_memoria == MEMORIA_SWAP){
        lista = lista_frames_swap;
    }else{
        lista= lista_frames_memoria;
    }
    int i;
    for(i=0; i<paginas; i++){
        p_frame_t* frame_a_mostrar = list_get(lista, i);
        U_LOG_TRACE("Frame: %d, ocupado: %d, ocupantes: %d", i, frame_a_mostrar->ocupado, frame_a_mostrar->ocupantes_frame);
    }
}

private bool paginacion_tiene_frames_libres(int frames, t_list* lista_frames){
    t_list* lista_frame_libres = list_filter(lista_frames, (void*)paginacion_frame_esta_libre);   
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

private p_patota_y_tabla_t* paginacion_agregar_patota_a_listado(uint32_t pid, int paginas, int tamanio_tareas){//ver si pasar las tareas
    int i;
    p_patota_y_tabla_t* patota = u_malloc(sizeof(p_patota_y_tabla_t));
    patota->pid = pid;
    patota->tamanio_tareas = tamanio_tareas;
    patota->tripulantes_escritos = 0;
    patota->tabla = list_create();
    patota->direcciones_logicas = list_create();

    for(i=0; i<paginas; i++){
        p_tipo_memoria_e tipo_memoria;
        p_fila_tabla_de_paginas_t* fila = u_malloc(sizeof(p_fila_tabla_de_paginas_t));
        fila->num_pagina = i;
        fila->frame_memoria = -1;
        fila->frame_swap = -1;   
        
        //p_frame_t* frame_a_escribir = list_find(lista_frames_swap, (void*)paginacion_frame_esta_libre);
        p_frame_t* frame_a_escribir = paginacion_encontrar_frame_libre(&tipo_memoria);
        
        frame_a_escribir->ocupado = 1;

        if(tipo_memoria == MEMORIA_FISICA){
            fila->frame_swap = -1;
            fila->frame_memoria = frame_a_escribir->num_frame;
            fila->uso = 1;
            fila->ingreso_en_memoria = contador_memoria;
        }else{
            fila->frame_swap = frame_a_escribir->num_frame;
            fila->frame_memoria = -1;
            fila->uso = -1;
            fila->ingreso_en_memoria = -1;
        }

        list_add(patota->tabla, fila); 
    }
    list_add(listado_patotas, patota);   //mutex listado de patotas 
    U_LOG_INFO("Se agrego patota con pid: %d, al listado de patotas", pid);
    mostrar_tabla_de_paginas(pid);
    return patota; 
}

private bool paginacion_agregar_patota_en_memoria_swap(uint32_t pid, uint32_t cant_tripulantes, const char* tareas, p_patota_y_tabla_t* patota){
    uint32_t pid_y_direccion_tareas[2] = {pid, 8 + 21 * cant_tripulantes};
    
    int direccion_logica = 0; 
    int pagina = direccion_logica / tamanio_pagina;
    p_tipo_memoria_e tipo_de_memoria;
    int frame = paginacion_inicializacion_frame_correspondiente_a_pagina(pagina, patota, &tipo_de_memoria); 
    int desplazamiento = direccion_logica % tamanio_pagina;
    int direccion_fisica = frame * tamanio_pagina + desplazamiento;
    
    uint32_t i;
    int j;

    //p_tipo_memoria_e tipo_de_memoria = MEMORIA_SWAP;
    p_tipo_escritura_e tipo_escritura = NUEVA_ESCRITURA; //agregado
    if(!paginacion_hay_overflow(desplazamiento + 1)){
        paginacion_inicializacion_modificar_frame(frame, tipo_escritura, patota, tipo_de_memoria);
    }

    for(i=0; i<2; i++){
        paginacion_inicializacion_escribir_uint32(pid_y_direccion_tareas[i], &desplazamiento, &direccion_fisica, &direccion_logica, patota, &tipo_de_memoria, tipo_escritura);
    }
    U_LOG_TRACE("PID: %d, copiado con exito pid y direccion tareas", pid);


    //tipo_escritura = MODIFICACION; 
    for(i=0; i<cant_tripulantes; i++){
        for(j=0; j<21; j++){
            paginacion_inicializacion_chequear_overflow_de_pagina_byte_a_byte(&direccion_fisica, &desplazamiento, &direccion_logica, patota, MODIFICACION, &tipo_de_memoria);
            desplazamiento++;
            direccion_fisica++;
            direccion_logica++;
        }
    }
    U_LOG_TRACE("PID: %d, reservada en memoria espacio para tripulantes", pid);


    for(i=0; i<strlen(tareas)+1; i++){
        paginacion_inicializacion_escribir_char(tareas[i], &direccion_fisica, &desplazamiento, &direccion_logica, patota, tipo_escritura, &tipo_de_memoria);
    }
    U_LOG_TRACE("PID: %d, copiadas tareas con exito", pid);
    
    return true;
}


private void paginacion_modificar_frame(int32_t numero_de_frame, p_tipo_escritura_e escritura, p_patota_y_tabla_t* patota){
    p_frame_t* frame = list_get(lista_frames_memoria, numero_de_frame);
    p_fila_tabla_de_paginas_t* fila_tabla = buscar_fila_en_tabla_de_paginas_segun_frame_memoria_real(patota, numero_de_frame);
    switch(escritura){
        case NUEVA_ESCRITURA:;
            frame->ocupantes_frame = (frame->ocupantes_frame) + 1;  //modificar a tabla de paginas 
            fila_tabla->ingreso_en_memoria = contador_memoria;
            contador_memoria++;  
            fila_tabla->uso = 1;

            U_LOG_INFO("escribiendo en frame %d", numero_de_frame);
            break;
        case MODIFICACION:;
            fila_tabla->ingreso_en_memoria = contador_memoria;
            contador_memoria++;  
            fila_tabla->uso = 1;
            U_LOG_TRACE("modificando/leyendo datos en frame %d", numero_de_frame);
            break;
        case ELIMINACION:;
            frame->ocupantes_frame = (frame->ocupantes_frame) - 1;   //modificar a tabla de paginas
            U_LOG_INFO("eliminando datos en frame %d", numero_de_frame);
            if((frame->ocupantes_frame) == 0){  
                frame->ocupado = 0;
                //frame->tiempo_en_memoria = 0;
                //frame->uso = 0;
                U_LOG_TRACE("Liberado frame %d", numero_de_frame);
                p_fila_tabla_de_paginas_t* fila_de_tabla = buscar_fila_por_frame(patota->tabla, numero_de_frame); 
                fila_de_tabla->frame_memoria = -1;
            }
            break; 
        case DE_SWAP_A_REAL:;
            //que hacer con ocupantes frame, 
            /*
            U_LOG_INFO("Se copio frame de swap en frame %d de memoria real", numero_de_frame);
            frame->ocupado = 1;
            frame->uso = 1;
            p_fila_tabla_de_paginas_t* fila_de_tabla = buscar_fila_por_frame(patota->tabla, (int)numero_de_frame); 
            fila_de_tabla->frame_memoria = numero_de_frame;*/
            break;
        case DE_REAL_A_SWAP:;

            break;
    }
    
}

private void paginacion_modificar_frame_de_swap(int numero_de_frame, p_tipo_escritura_e escritura, p_patota_y_tabla_t* patota){
    p_frame_t* frame = list_get(lista_frames_swap, numero_de_frame);

    switch(escritura){
        case NUEVA_ESCRITURA:;
           // frame->tiempo_en_memoria = contador_memoria;
           // contador_memoria++; //no va aca. 
           // frame->uso = 1;
            frame->ocupado = 1;
            frame->ocupantes_frame = (frame->ocupantes_frame) + 1;
            U_LOG_INFO("escribiendo en frame de swap %d", numero_de_frame);
            break;
        case MODIFICACION:;
            //frame->uso = 1;
            U_LOG_TRACE("modificando/leyendo datos en frame de swap %d", numero_de_frame);
            break;
        case ELIMINACION:;
            frame->ocupantes_frame = (frame->ocupantes_frame) - 1; 
            U_LOG_INFO("eliminando datos en frame de swap %d", numero_de_frame);
            if((frame->ocupantes_frame) == 0){  
                frame->ocupado = 0;
                //frame->tiempo_en_memoria = 0;
                //frame->uso = 0;
                U_LOG_TRACE("Liberado frame de swap %d", numero_de_frame);
                p_fila_tabla_de_paginas_t* fila_de_tabla = buscar_fila_por_frame(patota->tabla, numero_de_frame); 
                fila_de_tabla->frame_swap = -1;
            }
            break; 
        case DE_REAL_A_SWAP:;
            U_LOG_INFO("Frame de memoria real movido a frame swap %d", numero_de_frame);
            frame->ocupado = 0;
            break;
    }
    
}
/*
private void paginacion_modificar_tabla(p_patota_y_tabla_t* patota, int numero_de_frame, int numero_pagina){
    p_fila_tabla_de_paginas_t* fila = list_get(patota->tabla, numero_pagina);
    fila->num_pagina = numero_pagina;
    fila->frame_memoria = numero_de_frame;
    //agregar frame swap 
    U_LOG_TRACE("Se agrego pagina a tabla de paginas");
}*/

private void mostrar_tabla_de_paginas(uint32_t pid){
    bool comparar_pid(p_patota_y_tabla_t* patota){
        if(patota->pid==pid){
            return true;
        }
        return false;
    };
    p_patota_y_tabla_t* a_mostrar = list_find(listado_patotas, (void*)comparar_pid);
    int i;
    for(i=0; i<(list_size(a_mostrar->tabla)); i++){
        p_fila_tabla_de_paginas_t* fila = list_get(a_mostrar->tabla, i);
        U_LOG_TRACE("Mostrando tabla de pid: %d, pagina: %d, frame memoria: %d, frame swap: %d, bit de uso: %d, ingreso a memoria: %d", 
        a_mostrar->pid, fila->num_pagina, fila->frame_memoria, fila->frame_swap, fila->uso, fila->ingreso_en_memoria);
    }
}

private p_patota_y_tabla_t* buscar_patota_por_pid(uint32_t pid){
    bool buscar_patota(p_patota_y_tabla_t* patota){
        if(pid == patota->pid){
            return true;
        }
        return false;
    };
    p_patota_y_tabla_t* patota_buscada = list_find(listado_patotas, (void*)buscar_patota);
    return patota_buscada; 
}

private bool paginacion_chequear_overflow_de_pagina(int tamanio_dato, int *frame, int *desplazamiento, int *pagina, p_patota_y_tabla_t* patota, p_tipo_escritura_e tipo_escritura, p_tipo_memoria_e tipo_de_memoria){
    if((*desplazamiento)+tamanio_dato>tamanio_pagina){
        *frame = paginacion_frame_correspondiente_a_pagina((*pagina)+1, patota, tipo_de_memoria);
        *desplazamiento = 0;
        *pagina = (*pagina) + 1; 
       
        //agregado para la expulsion del tripulante
        if(tipo_de_memoria == MEMORIA_FISICA){
            paginacion_modificar_frame(*frame, tipo_escritura, patota);
        }else{
            paginacion_modificar_frame_de_swap(*frame, tipo_escritura, patota);
        }
        
        return true;
    }
    return false;
}

private int32_t paginacion_frame_correspondiente_a_pagina(uint32_t pagina, p_patota_y_tabla_t* patota, p_tipo_memoria_e tipo_memoria){
    bool paginacion_buscar_frame_de_pagina(p_fila_tabla_de_paginas_t* fila_recibida){
        if(fila_recibida->num_pagina==pagina){
            return true;
        }
            return false;
    };
    p_fila_tabla_de_paginas_t * fila = list_find(patota->tabla, (void*)paginacion_buscar_frame_de_pagina);
    
    if(tipo_memoria == MEMORIA_SWAP){
        return fila->frame_swap;
    }else{
        return fila->frame_memoria;
    }
}

private tripulantes_t* paginacion_obtener_tripulante_de_patota(p_patota_y_tabla_t* patota, int direccion_logica){
    U_LOG_TRACE("PEDIDO DE OBTENER TRIPULANTE PARA LISTA");
    tripulantes_t* tripulante = u_malloc(sizeof(tripulantes_t));

    //int pagina = direccion_logica / tamanio_pagina;
    //int frame = paginacion_frame_correspondiente_a_pagina(pagina, patota, MEMORIA_FISICA);
    int direccion_fisica = paginacion_buscar_direccion_en_tabla_de_paginas(direccion_logica, patota);
    int desplazamiento = direccion_logica % tamanio_pagina;
    tripulante->pid = patota->pid; //deberia buscarlo en memoria? 

    p_tipo_escritura_e tipo_escritura = MODIFICACION; //agregado
    p_tipo_memoria_e tipo_memoria = MEMORIA_FISICA;

    paginacion_obtener_uint32_de_memoria(&(tripulante->tid), &desplazamiento, &direccion_fisica, &direccion_logica, patota, tipo_escritura);

    paginacion_obtener_char_de_memoria(&(tripulante->estado), &direccion_fisica, &desplazamiento, &direccion_logica, patota, tipo_escritura, tipo_memoria);

    paginacion_obtener_uint32_de_memoria(&(tripulante->pos.x), &desplazamiento, &direccion_fisica, &direccion_logica, patota, tipo_escritura);

    paginacion_obtener_uint32_de_memoria(&(tripulante->pos.y), &desplazamiento, &direccion_fisica, &direccion_logica, patota, tipo_escritura);

    paginacion_obtener_uint32_de_memoria(&(tripulante->proxima_tarea), &desplazamiento, &direccion_fisica, &direccion_logica, patota, tipo_escritura);

    paginacion_obtener_uint32_de_memoria(&(tripulante->direccion_pcb), &desplazamiento, &direccion_fisica, &direccion_logica, patota, tipo_escritura);
    
            
    return tripulante;
}

private char* paginacion_obtener_tareas_de_pid(uint32_t pid){
    
    p_patota_y_tabla_t* patota = buscar_patota_por_pid(pid);
    
    int base = 4 ; //correspondientes a la estructura del pcb  
    int pagina = base / tamanio_pagina;
    int frame = paginacion_frame_correspondiente_a_pagina(pagina, patota, MEMORIA_FISICA);
    int desplazamiento = base % tamanio_pagina; 
     
    int direccion_tareas;
    int tamanio_tareas = patota->tamanio_tareas;
    char* tareas = u_malloc(tamanio_tareas);

    p_tipo_escritura_e tipo_escritura = MODIFICACION; //agregado
    paginacion_modificar_frame(frame, tipo_escritura, patota);
    
    //Busco la direccion de las tareas en memoria
    paginacion_chequear_overflow_de_pagina(sizeof(uint32_t), &frame, &desplazamiento, &pagina, patota, tipo_escritura, MEMORIA_FISICA);
    memcpy(&direccion_tareas, esquema_memoria_mfisica + frame * tamanio_pagina + desplazamiento, sizeof(uint32_t));

    U_LOG_TRACE("direccion tareas levantada correctamente");
    base = direccion_tareas;
    pagina = base / tamanio_pagina;
    frame = paginacion_frame_correspondiente_a_pagina(pagina, patota, MEMORIA_FISICA);
    desplazamiento = base % tamanio_pagina;

    //Recorro la memoria y copio caracter a caracter las tareas
    int i;
    for(i=0; i<tamanio_tareas; i++){
        paginacion_chequear_overflow_de_pagina(sizeof(char), &frame, &desplazamiento, &pagina, patota, tipo_escritura, MEMORIA_FISICA);
        memcpy(tareas + i, esquema_memoria_mfisica + frame * tamanio_pagina + desplazamiento, sizeof(char));
        desplazamiento++;
    }

    return tareas;
}

private tripulantes_t* paginacion_obtener_tcb(uint32_t pid, uint32_t tid){

    p_patota_y_tabla_t* patota = buscar_patota_por_pid(pid);

    bool _con_el_mismo_tid(p_direcciones_logicas_t* direccion_logica_de_lista){
        if(tid == direccion_logica_de_lista->tid){
            return true;
        }else{
            return false; 
        }
    };
    p_direcciones_logicas_t* direccion = list_find(patota->direcciones_logicas, (void*)_con_el_mismo_tid);
    tripulantes_t* tripulante = paginacion_obtener_tripulante_de_patota(patota, direccion->direccion_logica);
   
    return tripulante;
}

private void paginacion_modificar_proxima_tarea_tripulante(uint32_t pid, uint32_t tid){
    p_patota_y_tabla_t* patota = buscar_patota_por_pid(pid);
    int base = 8; //correspondientes a la estructura del pcb  
    int pagina = base / tamanio_pagina;
    int frame = paginacion_frame_correspondiente_a_pagina(pagina, patota, MEMORIA_FISICA); //chequear si la pagina esta en memoria. 
    int desplazamiento = base % tamanio_pagina;
    bool tid_encontrado = false; 
    int i;
    uint32_t tid_comparado;
    uint32_t proxima_tarea;

    p_tipo_escritura_e tipo_escritura = MODIFICACION; //agregado
    paginacion_modificar_frame(frame, tipo_escritura, patota);

    while(!tid_encontrado){
        paginacion_chequear_overflow_de_pagina(sizeof(uint32_t), &frame, &desplazamiento, &pagina, patota, tipo_escritura, MEMORIA_FISICA); //cuando cambio de pagina chequear si la pagina esta en memoria
        
        memcpy(&tid_comparado, esquema_memoria_mfisica + frame * tamanio_pagina + desplazamiento, sizeof(uint32_t));
        desplazamiento = desplazamiento + sizeof(uint32_t);
        if(tid_comparado == tid){
            tid_encontrado = true;

            paginacion_chequear_overflow_de_pagina(sizeof(char), &frame, &desplazamiento, &pagina, patota, tipo_escritura, MEMORIA_FISICA);
            desplazamiento++;

            paginacion_chequear_overflow_de_pagina(sizeof(uint32_t), &frame, &desplazamiento, &pagina, patota, tipo_escritura, MEMORIA_FISICA);
            desplazamiento = desplazamiento + sizeof(uint32_t);

            paginacion_chequear_overflow_de_pagina(sizeof(uint32_t), &frame, &desplazamiento, &pagina, patota, tipo_escritura, MEMORIA_FISICA);
            desplazamiento = desplazamiento + sizeof(uint32_t);

            paginacion_chequear_overflow_de_pagina(sizeof(uint32_t), &frame, &desplazamiento, &pagina, patota, tipo_escritura, MEMORIA_FISICA);            
            memcpy(&proxima_tarea, esquema_memoria_mfisica + frame * tamanio_pagina + desplazamiento, sizeof(uint32_t));
            proxima_tarea++;

            memcpy(esquema_memoria_mfisica + frame * tamanio_pagina + desplazamiento, &proxima_tarea, sizeof(uint32_t));
            
        }else{ 
            for(i=0; i<17; i++){
                paginacion_chequear_overflow_de_pagina(1, &frame, &desplazamiento, &pagina, patota, tipo_escritura, MEMORIA_FISICA); //mismo tema que arriba, agregar cuarta opcion?
                desplazamiento++;
            }
        }
    }
    U_LOG_INFO("Actualizado proxima tarea de tid: %d", tid);
}

private p_fila_tabla_de_paginas_t* buscar_fila_por_frame(t_list* tabla, int numero_de_frame){
    bool buscar_fila_donde_esta_asignado_el_frame(p_fila_tabla_de_paginas_t* fila){
        if(fila->frame_memoria == numero_de_frame){
            return true;
        }else{
            return false;
        }
    }
    return list_find(tabla, (void*)buscar_fila_donde_esta_asignado_el_frame);
}

private void paginacion_guardar_direccion_logica_tcb(int direccion_logica, int tid, p_patota_y_tabla_t* patota){
    p_direcciones_logicas_t* direccion = u_malloc(sizeof(p_direcciones_logicas_t));
    direccion->direccion_logica = direccion_logica;
    direccion->tid = tid;
    list_add(patota->direcciones_logicas, direccion);
    U_LOG_TRACE("Guardada direccion logica de tid: %d, direccion: %d", direccion->tid, direccion->direccion_logica);
}

private int paginacion_buscar_tid_en_tabla_de_paginas(uint32_t tid, p_patota_y_tabla_t* patota){
    bool buscar_direccion_de_tid(p_direcciones_logicas_t* direccion_a_comparar){
        if(direccion_a_comparar->tid == tid){
            return true;
        }else{
            return false;
        }
    };
    p_direcciones_logicas_t* direccion = list_find(patota->direcciones_logicas, (void*)buscar_direccion_de_tid);
    return direccion->direccion_logica;
}

private int paginacion_page_fault(int pagina, p_patota_y_tabla_t* patota){
    if(paginacion_tiene_frames_libres(1, lista_frames_memoria)){
        return paginacion_copiar_frame_de_swap_a_memoria(pagina, patota);
    }
    return paginacion_liberar_un_frame(pagina, patota); //TODO
}

private int paginacion_copiar_frame_de_swap_a_memoria(int pagina, p_patota_y_tabla_t* patota){
    int frame_swap = paginacion_frame_correspondiente_a_pagina(pagina, patota, MEMORIA_SWAP);

    p_frame_t* frame_a_escribir = list_find(lista_frames_memoria, (void*)paginacion_frame_esta_libre);
    uint32_t frame_memoria = frame_a_escribir->num_frame;

    memcpy(esquema_memoria_mfisica + frame_memoria * tamanio_pagina, memoria_swap_fisica + frame_swap * tamanio_pagina, tamanio_pagina);

    U_LOG_INFO("Copiado frame de swap %d a frame de memoria %d", frame_swap, frame_memoria);
    paginacion_modificar_frame_y_tabla_de_paginas(frame_memoria, patota, pagina);

    return paginacion_buscar_direccion_en_tabla_de_paginas(pagina * tamanio_pagina, patota);

    
    //agregado prueba 
   
    /*
    uint32_t prueba;
    memcpy(&prueba, memoria_swap_fisica + frame_swap * tamanio_pagina + 8, sizeof(uint32_t));
    U_LOG_INFO("TID: %d", prueba);
    memcpy(&prueba, esquema_memoria_mfisica + frame_memoria * tamanio_pagina + 8, sizeof(uint32_t));
    U_LOG_INFO("TID: %d", prueba);*/

    //HASTA ACA
}

private int paginacion_buscar_direccion_en_tabla_de_paginas(int direccion_logica, p_patota_y_tabla_t* patota){
    int base = direccion_logica;
    int pagina = base / tamanio_pagina;
    int desplazamiento = base % tamanio_pagina;

    int frame = paginacion_frame_correspondiente_a_pagina(pagina, patota, MEMORIA_FISICA);
    if(frame == -1){
        int direccion_logica_sin_desplazamiento = paginacion_page_fault(pagina, patota);
        return direccion_logica_sin_desplazamiento + desplazamiento;
    }
    return frame * tamanio_pagina + desplazamiento;
}

private int paginacion_buscar_direccion_en_tabla_de_paginas_swap(int direccion_logica, p_patota_y_tabla_t* patota, p_tipo_escritura_e tipo_escritura){
    int base = direccion_logica;
    int pagina = base / tamanio_pagina;
    int desplazamiento = base % tamanio_pagina;

    int frame = paginacion_frame_correspondiente_a_pagina(pagina, patota, MEMORIA_SWAP);
    paginacion_modificar_frame_de_swap(frame, tipo_escritura, patota); //ver si es nueva_escritura siempre;
    return frame * tamanio_pagina + desplazamiento; 
}


private void paginacion_modificar_frame_y_tabla_de_paginas(int numero_de_frame, p_patota_y_tabla_t* patota, int pagina){
    p_frame_t* frame = list_get(lista_frames_memoria, numero_de_frame);
    U_LOG_INFO("Se copio frame de swap en frame %d de memoria real", numero_de_frame);
    frame->ocupado = 1;
    //frame->uso = 1;
    p_fila_tabla_de_paginas_t* fila_de_tabla = list_get(patota->tabla, pagina); 
    fila_de_tabla->frame_memoria = numero_de_frame;
    fila_de_tabla->uso = 1;
    fila_de_tabla->ingreso_en_memoria = contador_memoria;
    contador_memoria++;
}

private bool paginacion_chequear_overflow_de_pagina_en_memoria(int tamanio_data, int * direccion_fisica, p_patota_y_tabla_t* patota){
    if((*direccion_fisica)+tamanio_data>tamanio_pagina){
        *direccion_fisica = paginacion_buscar_direccion_en_tabla_de_paginas((*direccion_fisica)+tamanio_data, patota);
        return true;
    }
    return false;
}
/*
private tripulantes_t* paginacion_memoria_obtener_info_tripulante_swap(int pid, int tid){
    tripulantes_t* tripulante = u_malloc(sizeof(tripulantes_t));

    p_patota_y_tabla_t* patota = buscar_patota_por_pid(pid);
    int base = 8; //correspondientes a la estructura del pcb y el inicio de los tripulantes escritos en memoria;  
    int pagina = base / tamanio_pagina;
    int frame = paginacion_frame_correspondiente_a_pagina(pagina, patota, MEMORIA_SWAP);
    int desplazamiento = base % tamanio_pagina;
    bool tid_encontrado = false; 
    int i;
    uint32_t tid_comparado;

    p_tipo_escritura_e tipo_escritura = MODIFICACION; //agregado
    paginacion_modificar_frame_de_swap(frame, tipo_escritura, patota);

    while(!tid_encontrado){
        paginacion_chequear_overflow_de_pagina(sizeof(uint32_t), &frame, &desplazamiento, &pagina, patota, tipo_escritura, MEMORIA_SWAP);
        
        memcpy(&tid_comparado, memoria_swap_fisica + frame * tamanio_pagina + desplazamiento, sizeof(uint32_t));
        desplazamiento = desplazamiento + sizeof(uint32_t);
        if(tid_comparado == tid){
            tid_encontrado = true;
            tripulante->tid = tid_comparado;
            tripulante->pid = pid;

            paginacion_chequear_overflow_de_pagina(sizeof(char), &frame, &desplazamiento, &pagina, patota, tipo_escritura, MEMORIA_SWAP);
            memcpy(&(tripulante->estado), memoria_swap_fisica+ frame * tamanio_pagina + desplazamiento, sizeof(char));
            desplazamiento++;
            
            paginacion_chequear_overflow_de_pagina(sizeof(uint32_t), &frame, &desplazamiento, &pagina, patota, tipo_escritura, MEMORIA_SWAP);
            memcpy(&(tripulante->pos.x), memoria_swap_fisica + frame * tamanio_pagina + desplazamiento, sizeof(uint32_t));
            desplazamiento = desplazamiento + sizeof(uint32_t);

            paginacion_chequear_overflow_de_pagina(sizeof(uint32_t), &frame, &desplazamiento, &pagina, patota, tipo_escritura, MEMORIA_SWAP);
            memcpy(&(tripulante->pos.y), memoria_swap_fisica + frame * tamanio_pagina + desplazamiento, sizeof(uint32_t));
            desplazamiento = desplazamiento + sizeof(uint32_t);
            
            paginacion_chequear_overflow_de_pagina(sizeof(uint32_t), &frame, &desplazamiento, &pagina, patota, tipo_escritura, MEMORIA_SWAP);
            memcpy(&(tripulante->proxima_tarea), memoria_swap_fisica + frame * tamanio_pagina + desplazamiento, sizeof(uint32_t));
            desplazamiento = desplazamiento + sizeof(uint32_t);

        }else{
            for(i=0; i<17; i++){
                paginacion_chequear_overflow_de_pagina(1, &frame, &desplazamiento, &pagina, patota, tipo_escritura, MEMORIA_SWAP); //plantear una cuarta opcion que no modifique el bit de uso?
                desplazamiento++;
            }
        }
    }

    return tripulante;
}*/

private void paginacion_escribir_uint32(uint32_t a_escribir, int* desplazamiento, int* direccion_fisica, int* direccion_logica, p_patota_y_tabla_t* patota, p_tipo_memoria_e tipo_memoria, p_tipo_escritura_e tipo_escritura){
    void* memoria;
    if(tipo_memoria == MEMORIA_SWAP){
        memoria = memoria_swap_fisica;
    }else{
        memoria = esquema_memoria_mfisica;
    }
    
    if(paginacion_hay_overflow(*desplazamiento + sizeof(uint32_t))){
        int lo_que_queda_de_pagina = tamanio_pagina - *desplazamiento;

        memcpy(memoria + *direccion_fisica, (void*)&a_escribir, lo_que_queda_de_pagina);
        U_LOG_TRACE("ESCRITO %d en frame %d, direccion logica %d, direccion fisica %d", a_escribir,*direccion_fisica/tamanio_pagina, *direccion_logica, *direccion_fisica);
        *direccion_logica = *direccion_logica + lo_que_queda_de_pagina; 
        if(tipo_memoria == MEMORIA_SWAP){
            *direccion_fisica = paginacion_buscar_direccion_en_tabla_de_paginas_swap(*direccion_logica, patota, tipo_escritura);
        }
        else{
            *direccion_fisica = paginacion_buscar_direccion_en_tabla_de_paginas(*direccion_logica, patota); //en teoria no hace page fault en el caso de la inicializacion
            paginacion_modificar_frame(*direccion_fisica/tamanio_pagina, tipo_escritura, patota);
        }

        memcpy(memoria + *direccion_fisica, ((void*)&a_escribir) + lo_que_queda_de_pagina, sizeof(uint32_t) - lo_que_queda_de_pagina); //chequear
        U_LOG_TRACE("ESCRITO %d en frame %d, direccion logica %d, direccion fisica %d", a_escribir, *direccion_fisica/tamanio_pagina, *direccion_logica, *direccion_fisica);
        *direccion_logica = *direccion_logica + sizeof(uint32_t) - lo_que_queda_de_pagina;
        *direccion_fisica = *direccion_fisica + sizeof(uint32_t) - lo_que_queda_de_pagina;
        *desplazamiento = sizeof(uint32_t) - lo_que_queda_de_pagina;       
    }
    else{
        memcpy(memoria + *direccion_fisica, &a_escribir, sizeof(uint32_t));
        U_LOG_TRACE("ESCRITO %d en frame %d, direccion logica %d, direccion fisica %d", a_escribir, *direccion_fisica/tamanio_pagina, *direccion_logica, *direccion_fisica);
        *direccion_logica = *direccion_logica + sizeof(uint32_t);
        *direccion_fisica = *direccion_fisica + sizeof(uint32_t);
        *desplazamiento = *desplazamiento + sizeof(uint32_t);
    }
}

private bool paginacion_hay_overflow(int desplazamiento){
    if(desplazamiento>=tamanio_pagina){
        return true;
    }
    return false;
}

private bool paginacion_chequear_overflow_de_pagina_byte_a_byte(int *direccion_fisica, int *desplazamiento, int *direccion_logica, p_patota_y_tabla_t* patota, p_tipo_escritura_e tipo_escritura, p_tipo_memoria_e tipo_de_memoria){
    if((*desplazamiento)+1>tamanio_pagina){
        *direccion_logica = *direccion_logica + 1; 
        if(tipo_de_memoria == MEMORIA_SWAP){
            *direccion_fisica = paginacion_buscar_direccion_en_tabla_de_paginas_swap(*direccion_logica, patota, tipo_escritura);
        }else{
            *direccion_fisica = paginacion_buscar_direccion_en_tabla_de_paginas(*direccion_logica, patota);
            int frame = *direccion_fisica/tamanio_pagina;

            paginacion_modificar_frame(frame, tipo_escritura, patota);
        }
        *desplazamiento = 0;
        return true;
    }
    return false;
}

private void paginacion_obtener_uint32_de_memoria(uint32_t* donde_escribo, int* desplazamiento, int* direccion_fisica, int* direccion_logica, p_patota_y_tabla_t* patota, p_tipo_escritura_e tipo_escritura){
    if(paginacion_hay_overflow(*desplazamiento + sizeof(uint32_t))){
        int lo_que_queda_de_pagina = tamanio_pagina - *desplazamiento;

        memcpy((void*)donde_escribo, esquema_memoria_mfisica + *direccion_fisica, lo_que_queda_de_pagina);
        U_LOG_TRACE("RECUPERADO %d en frame %d, direccion logica %d, direccion fisica %d", *donde_escribo, *direccion_fisica/tamanio_pagina, *direccion_logica, *direccion_fisica);
        *direccion_logica = *direccion_logica + lo_que_queda_de_pagina; 
        /*
        if(tipo_memoria == MEMORIA_SWAP){
            *direccion_fisica = paginacion_buscar_direccion_en_tabla_de_paginas_swap(*direccion_logica, patota, tipo_escritura);
        }
        else{*/
        *direccion_fisica = paginacion_buscar_direccion_en_tabla_de_paginas(*direccion_logica, patota);
        paginacion_modificar_frame(*direccion_fisica/tamanio_pagina, tipo_escritura, patota);
        //}

        memcpy(((void*)donde_escribo) + lo_que_queda_de_pagina, esquema_memoria_mfisica + *direccion_fisica,  sizeof(uint32_t) - lo_que_queda_de_pagina); //chequear
        U_LOG_TRACE("RECUPERADO %d en frame %d, direccion logica %d, direccion fisica %d", *donde_escribo, *direccion_fisica/tamanio_pagina, *direccion_logica, *direccion_fisica);
        *direccion_logica = *direccion_logica + sizeof(uint32_t) - lo_que_queda_de_pagina;
        *direccion_fisica = *direccion_fisica + sizeof(uint32_t) - lo_que_queda_de_pagina;
        *desplazamiento = sizeof(uint32_t) - lo_que_queda_de_pagina;       
    }
    else{
        memcpy(donde_escribo, esquema_memoria_mfisica + *direccion_fisica, sizeof(uint32_t));
        U_LOG_TRACE("RECUPERADO %d en frame %d, direccion logica %d, direccion fisica %d", *donde_escribo, *direccion_fisica/tamanio_pagina, *direccion_logica, *direccion_fisica);
        *direccion_logica = *direccion_logica + sizeof(uint32_t);
        *direccion_fisica = *direccion_fisica + sizeof(uint32_t);
        *desplazamiento = *desplazamiento + sizeof(uint32_t);
    }
}

private void paginacion_obtener_char_de_memoria(char* donde_escribo, int *direccion_fisica, int * desplazamiento, int *direccion_logica, p_patota_y_tabla_t* patota, p_tipo_escritura_e tipo_escritura, p_tipo_memoria_e tipo_de_memoria){
    paginacion_chequear_overflow_de_pagina_byte_a_byte(direccion_fisica, desplazamiento, direccion_logica, patota, tipo_escritura, tipo_de_memoria);
    memcpy(donde_escribo, esquema_memoria_mfisica + *direccion_fisica, sizeof(char)); //chequear esta linea 
    U_LOG_TRACE("RECUPERADO %c en frame %d, direccion logica %d, direccion fisica %d", *donde_escribo, *direccion_fisica/tamanio_pagina, *direccion_logica, *direccion_fisica);
    *desplazamiento = *desplazamiento + sizeof(char);
    *direccion_fisica = *direccion_fisica + sizeof(char);
    *direccion_logica = *direccion_logica + sizeof(char);
}

private void paginacion_escribir_char(char a_escribir, int *direccion_fisica, int * desplazamiento, int *direccion_logica, p_patota_y_tabla_t* patota, p_tipo_escritura_e tipo_escritura, p_tipo_memoria_e tipo_de_memoria){
    void * memoria;
    if(tipo_de_memoria == MEMORIA_SWAP){
        memoria = memoria_swap_fisica;
    }else{
        memoria = esquema_memoria_mfisica;
    }
    paginacion_chequear_overflow_de_pagina_byte_a_byte(direccion_fisica, desplazamiento, direccion_logica, patota, tipo_escritura, tipo_de_memoria);
    memcpy(memoria + *direccion_fisica, &a_escribir, sizeof(char)); 
    *desplazamiento = *desplazamiento + sizeof(char);
    *direccion_fisica = *direccion_fisica + sizeof(char);
    *direccion_logica = *direccion_logica + sizeof(char);
}//en swap

private bool paginacion_tiene_frames_libres_totales(int frames){

    t_list* lista_frame_libres_real = list_filter(lista_frames_memoria, (void*)paginacion_frame_esta_libre);   
    int frames_libres_memoria_real = list_size(lista_frame_libres_real);
    list_destroy(lista_frame_libres_real);

    if(frames_libres_memoria_real>=frames){
        return true;
    }

    frames = frames - frames_libres_memoria_real;
    t_list* lista_frame_libres_swap = list_filter(lista_frames_swap, (void*)paginacion_frame_esta_libre);
    int frames_libres_memoria_swap = list_size(lista_frame_libres_swap);
    list_destroy(lista_frame_libres_swap);

    return frames_libres_memoria_swap >= frames; 
}

private p_frame_t* paginacion_encontrar_frame_libre(p_tipo_memoria_e* tipo_de_memoria){
    p_frame_t* frame_encontrado; 
    if(list_any_satisfy(lista_frames_memoria, paginacion_frame_esta_libre)){
        frame_encontrado = list_find(lista_frames_memoria, paginacion_frame_esta_libre);
        *tipo_de_memoria = MEMORIA_FISICA;
    }else{
        frame_encontrado = list_find(lista_frames_swap, paginacion_frame_esta_libre);
        *tipo_de_memoria = MEMORIA_SWAP;
    }
    return frame_encontrado;
}

private int32_t paginacion_inicializacion_frame_correspondiente_a_pagina(uint32_t pagina, p_patota_y_tabla_t* patota, p_tipo_memoria_e* tipo_de_memoria){
    bool paginacion_buscar_frame_de_pagina(p_fila_tabla_de_paginas_t* fila_recibida){
        if(fila_recibida->num_pagina==pagina){
            return true;
        }
            return false;
    };
    p_fila_tabla_de_paginas_t * fila = list_find(patota->tabla, (void*)paginacion_buscar_frame_de_pagina);
    
    if(fila->frame_memoria == -1){
        *tipo_de_memoria = MEMORIA_SWAP;
        return fila->frame_swap;
    }else{
        *tipo_de_memoria = MEMORIA_FISICA;
        return fila->frame_memoria;
    }
}

private void paginacion_inicializacion_modificar_frame(int numero_de_frame, p_tipo_escritura_e escritura, p_patota_y_tabla_t* patota, p_tipo_memoria_e tipo_de_memoria){
    if(tipo_de_memoria == MEMORIA_FISICA){
        paginacion_modificar_frame(numero_de_frame, escritura, patota);
    }else{
        paginacion_modificar_frame_de_swap(numero_de_frame, escritura, patota);
    }
}


private void paginacion_inicializacion_escribir_uint32(uint32_t a_escribir, int* desplazamiento, int* direccion_fisica, int* direccion_logica, p_patota_y_tabla_t* patota, p_tipo_memoria_e* tipo_memoria, p_tipo_escritura_e tipo_escritura){
    void* memoria;
    if(*tipo_memoria == MEMORIA_SWAP){
        memoria = memoria_swap_fisica;
    }else{
        memoria = esquema_memoria_mfisica;
    }
    
    if(paginacion_hay_overflow(*desplazamiento + sizeof(uint32_t))){
        int lo_que_queda_de_pagina = tamanio_pagina - *desplazamiento;

        memcpy(memoria + *direccion_fisica, (void*)&a_escribir, lo_que_queda_de_pagina);
        U_LOG_TRACE("ESCRITO %d en frame %d, direccion logica %d, direccion fisica %d", a_escribir,*direccion_fisica/tamanio_pagina, *direccion_logica, *direccion_fisica);
        *direccion_logica = *direccion_logica + lo_que_queda_de_pagina; 
        if(*tipo_memoria == MEMORIA_SWAP){
            *direccion_fisica = paginacion_buscar_direccion_en_tabla_de_paginas_swap(*direccion_logica, patota, tipo_escritura);
        }
        else{//HORRIBLE ESTA PARTE, MODIFICAR.
            bool pagina_buscada(p_fila_tabla_de_paginas_t* fila){
                if (fila->num_pagina == *direccion_logica/tamanio_pagina){
                    return true;
                }else{
                    return false;
                }
            };
            p_fila_tabla_de_paginas_t* fila_tabla = list_find(patota->tabla, pagina_buscada);
            if(fila_tabla->frame_memoria == -1){
                *tipo_memoria = MEMORIA_SWAP;
                memoria = memoria_swap_fisica;
                *direccion_fisica = paginacion_buscar_direccion_en_tabla_de_paginas_swap(*direccion_logica, patota, tipo_escritura);
            }else{
                *direccion_fisica = paginacion_buscar_direccion_en_tabla_de_paginas(*direccion_logica, patota); //en teoria no hace page fault en el caso de la inicializacion
                paginacion_modificar_frame(*direccion_fisica/tamanio_pagina, tipo_escritura, patota);
            }

        }

        memcpy(memoria + *direccion_fisica, ((void*)&a_escribir) + lo_que_queda_de_pagina, sizeof(uint32_t) - lo_que_queda_de_pagina); //chequear
        U_LOG_TRACE("ESCRITO %d en frame %d, direccion logica %d, direccion fisica %d", a_escribir, *direccion_fisica/tamanio_pagina, *direccion_logica, *direccion_fisica);
        *direccion_logica = *direccion_logica + sizeof(uint32_t) - lo_que_queda_de_pagina;
        *direccion_fisica = *direccion_fisica + sizeof(uint32_t) - lo_que_queda_de_pagina;
        *desplazamiento = sizeof(uint32_t) - lo_que_queda_de_pagina;       
    }
    else{
        memcpy(memoria + *direccion_fisica, &a_escribir, sizeof(uint32_t));
        U_LOG_TRACE("ESCRITO %d en frame %d, direccion logica %d, direccion fisica %d", a_escribir, *direccion_fisica/tamanio_pagina, *direccion_logica, *direccion_fisica);
        *direccion_logica = *direccion_logica + sizeof(uint32_t);
        *direccion_fisica = *direccion_fisica + sizeof(uint32_t);
        *desplazamiento = *desplazamiento + sizeof(uint32_t);
    }
}

private void paginacion_inicializacion_escribir_char(char a_escribir, int *direccion_fisica, int * desplazamiento, int *direccion_logica, p_patota_y_tabla_t* patota, p_tipo_escritura_e tipo_escritura, p_tipo_memoria_e* tipo_de_memoria){
    void * memoria;

    paginacion_inicializacion_chequear_overflow_de_pagina_byte_a_byte(direccion_fisica, desplazamiento, direccion_logica, patota, tipo_escritura, tipo_de_memoria);
    
    if(*tipo_de_memoria == MEMORIA_SWAP){
        memoria = memoria_swap_fisica;
    }else{
        memoria = esquema_memoria_mfisica;
    }

    memcpy(memoria + *direccion_fisica, &a_escribir, sizeof(char)); 
   // U_LOG_TRACE("ESCRITO %c en frame %d, direccion logica %d, direccion fisica %d", a_escribir, *direccion_fisica/tamanio_pagina, *direccion_logica, *direccion_fisica);
    *desplazamiento = *desplazamiento + sizeof(char);
    *direccion_fisica = *direccion_fisica + sizeof(char);
    *direccion_logica = *direccion_logica + sizeof(char);
}

private bool paginacion_inicializacion_chequear_overflow_de_pagina_byte_a_byte(int *direccion_fisica, int *desplazamiento, int *direccion_logica, p_patota_y_tabla_t* patota, p_tipo_escritura_e tipo_escritura, p_tipo_memoria_e* tipo_de_memoria){
    if((*desplazamiento)+1>tamanio_pagina){
        *direccion_logica = *direccion_logica + 1; 
        if(*tipo_de_memoria == MEMORIA_SWAP){
            *direccion_fisica = paginacion_buscar_direccion_en_tabla_de_paginas_swap(*direccion_logica, patota, tipo_escritura);
        }else{
            bool pagina_buscada(p_fila_tabla_de_paginas_t* fila){
                if (fila->num_pagina == *direccion_logica/tamanio_pagina){
                    return true;
                }else{
                    return false;
                }
            };
            p_fila_tabla_de_paginas_t* fila_tabla = list_find(patota->tabla, pagina_buscada);
            if(fila_tabla->frame_memoria == -1){
                *direccion_fisica = paginacion_buscar_direccion_en_tabla_de_paginas_swap(*direccion_logica, patota, tipo_escritura);
                *tipo_de_memoria = MEMORIA_SWAP;
            }else{
                *direccion_fisica = paginacion_buscar_direccion_en_tabla_de_paginas(*direccion_logica, patota);
                int frame = *direccion_fisica/tamanio_pagina;
                paginacion_modificar_frame(frame, tipo_escritura, patota);
            }
            
        }
        *desplazamiento = 0;
        return true;
    }
    return false;
}

private p_fila_tabla_de_paginas_t* buscar_fila_en_tabla_de_paginas_segun_frame_memoria_real(p_patota_y_tabla_t* patota, int numero_de_frame){
    
    bool pagina_encontrada(p_fila_tabla_de_paginas_t* fila){
        if(numero_de_frame == fila->frame_memoria){
            return true;
        }
        return false;
    };
    p_fila_tabla_de_paginas_t* fila_encontrada = list_find(patota->tabla, pagina_encontrada);
    return fila_encontrada;
}

private int paginacion_liberar_un_frame(int pagina, p_patota_y_tabla_t* patota){
    p_fila_tabla_de_paginas_t* pagina_a_sacar;
    
    if(string_equals_ignore_case(algoritmo_reemplazo, "LRU")){
        pagina_a_sacar = paginacion_seleccionar_pagina_por_LRU();
    }else{
        pagina_a_sacar = paginacion_seleccionar_pagina_por_CLOCK();
    }

    if(!paginacion_tiene_frames_libres(1, lista_frames_swap)){
        //el camino enquilombado; 
    }
    paginacion_copiar_frame_de_memoria_a_swap(pagina_a_sacar);
    return paginacion_copiar_frame_de_swap_a_memoria(pagina, patota);
    
}

private p_fila_tabla_de_paginas_t* paginacion_seleccionar_pagina_por_LRU(){

    t_list_iterator* iterador_de_patotas = list_iterator_create(listado_patotas);
    int ingreso_en_memoria_menor = contador_memoria;
    p_fila_tabla_de_paginas_t* pagina_elegida; 
    
    while(list_iterator_has_next(iterador_de_patotas)){
        p_patota_y_tabla_t* patota = list_iterator_next(iterador_de_patotas);

        t_list* tabla_de_paginas_filtrada = list_filter(patota->tabla, paginacion_pagina_esta_en_memoria_real);
        t_list_iterator* iterador_de_tabla = list_iterator_create(tabla_de_paginas_filtrada);

        while(list_iterator_has_next(iterador_de_tabla)){
            p_fila_tabla_de_paginas_t* fila_de_tabla = list_iterator_next(iterador_de_tabla);
            if(fila_de_tabla->ingreso_en_memoria <= ingreso_en_memoria_menor){
                ingreso_en_memoria_menor = fila_de_tabla->ingreso_en_memoria;
                pagina_elegida = fila_de_tabla;
            }
        }
        list_iterator_destroy(iterador_de_tabla);
    }
    list_iterator_destroy(iterador_de_patotas);
    return pagina_elegida;
}

private bool paginacion_pagina_esta_en_memoria_real(p_fila_tabla_de_paginas_t* fila){
    if(fila->frame_memoria == -1){
        return false;
    }
    return true;
}

private p_fila_tabla_de_paginas_t* paginacion_seleccionar_pagina_por_CLOCK(){

    t_list_iterator* iterador_de_patotas = list_iterator_create(listado_patotas);
    bool pagina_encontrada = false;
    p_fila_tabla_de_paginas_t* pagina_elegida; 
    
    while(!pagina_encontrada){ //si la memoria esta llena al menos una patota hay 
        p_patota_y_tabla_t* patota = list_iterator_next(iterador_de_patotas);

        t_list* tabla_de_paginas_filtrada = list_filter(patota->tabla, paginacion_pagina_esta_en_memoria_real);
        t_list_iterator* iterador_de_tabla = list_iterator_create(tabla_de_paginas_filtrada);

        while(list_iterator_has_next(iterador_de_tabla) || !pagina_encontrada){
            p_fila_tabla_de_paginas_t* fila_de_tabla = list_iterator_next(iterador_de_tabla);
            if(fila_de_tabla->uso == 0){
                pagina_elegida = fila_de_tabla;
                pagina_encontrada = true;
            }else{
                fila_de_tabla->uso = 0;
            }
        }
        list_iterator_destroy(iterador_de_tabla);
    }
    list_iterator_destroy(iterador_de_patotas);
    return pagina_elegida;
}

private void paginacion_copiar_frame_de_memoria_a_swap(p_fila_tabla_de_paginas_t* fila_de_tabla){
    int frame_memoria = fila_de_tabla->frame_memoria;

    p_frame_t* frame_donde_escribo = list_find(lista_frames_swap, (void*)paginacion_frame_esta_libre); //
    uint32_t frame_swap = frame_donde_escribo->num_frame;

    memcpy(memoria_swap_fisica + frame_swap * tamanio_pagina, esquema_memoria_mfisica + frame_memoria * tamanio_pagina, tamanio_pagina);

    U_LOG_INFO("Copiado frame de memoria real %d a frame de swap %d", frame_memoria, frame_swap);

    fila_de_tabla->frame_memoria = -1;
    fila_de_tabla->frame_swap = frame_swap;
    frame_donde_escribo->ocupado = 1;
    paginacion_marcar_como_liberado(frame_memoria, lista_frames_memoria);
    
    //agregado prueba 
   
    /*
    uint32_t prueba;
    memcpy(&prueba, memoria_swap_fisica + frame_swap * tamanio_pagina + 8, sizeof(uint32_t));
    U_LOG_INFO("TID: %d", prueba);
    memcpy(&prueba, esquema_memoria_mfisica + frame_memoria * tamanio_pagina + 8, sizeof(uint32_t));
    U_LOG_INFO("TID: %d", prueba);*/

    //HASTA ACA
}

private void paginacion_marcar_como_liberado(int numero_de_frame, t_list* lista_frames){
    p_frame_t* frame_a_liberar = list_get(lista_frames, numero_de_frame);
    frame_a_liberar->ocupado = 0;
}