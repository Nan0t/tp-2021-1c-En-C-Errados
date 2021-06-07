#include "paginacion.h"

void paginacion_memoria_init(void)
{
    esquema_memoria_tamanio = u_config_get_int_value("TAMANIO_MEMORIA");
    esquema_memoria_mfisica = u_malloc(esquema_memoria_tamanio);
    listado_patotas = list_create();
    lista_frames_memoria = list_create();
    lista_frames_swap = list_create();
    tamanio_pagina = u_config_get_int_value("TAMANIO_PAGINA");
    tamanio_swap = u_config_get_int_value("TAMANIO_SWAP");
    paginacion_inicializar_frames_memoria(esquema_memoria_tamanio/tamanio_pagina);
    //inicializar swap; 
    //paginacion_mostrar_frames(esquema_memoria_tamanio/tamanio_pagina);
}

bool paginacion_memoria_inicializar_patota(uint32_t pid, uint32_t cant_tripulantes, const char* tareas)
{

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
       m_frame_t* frame = u_malloc(sizeof(m_frame_t));
       frame->num_frame=i;
       frame->ocupado=0;
       list_add(lista_frames_memoria, frame);
    }
}

private void paginacion_mostrar_frames(int paginas){
    int i;
    for(i=0; i<paginas; i++){
        m_frame_t* frame_a_mostrar = list_get(lista_frames_memoria,i);
        U_LOG_TRACE("Frame: %d, ocupado: %d", i, frame_a_mostrar->ocupado);
    }
}