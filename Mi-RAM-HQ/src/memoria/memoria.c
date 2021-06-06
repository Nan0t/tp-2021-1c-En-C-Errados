#include "memoria.h"


void memoria_iniciar(){
    int tamanio=u_config_get_int_value("TAMANIO_MEMORIA");
    memoria_fisica = u_malloc(tamanio);
    esquema= memoria_levantar_esquema();
    listado_patotas = list_create();
    switch(esquema){
        case SEGMENTACION:;
            break;
        case PAGINACION:;
            U_LOG_TRACE("PAGINACION");
            vector_frames_memoria = list_create();
            tamanio_pagina = u_config_get_int_value("TAMANIO_PAGINA");
            int tamanio_swap = u_config_get_int_value("TAMANIO_SWAP");
            memoria_inicializar_frames_memoria(tamanio/tamanio_pagina);
            vector_frame_swap = u_malloc(tamanio_swap/tamanio_pagina*sizeof(int));
            break;
    }
}

int memoria_levantar_esquema(){
    int esquema_a_levantar;
    char* esquema_string = u_config_get_string_value("ESQUEMA_MEMORIA");
    if(string_equals_ignore_case(esquema_string,"SEGMENTACION")){
        esquema_a_levantar=SEGMENTACION;
    }else{
        esquema_a_levantar=PAGINACION;
    }
    return esquema_a_levantar;
}


void memoria_inicializar_frames_memoria(int paginas){
    int i;
    for(i=0; i<paginas; i++){
       m_frame_t* frame = u_malloc(sizeof(m_frame_t));
       frame->num_frame=i;
       frame->presencia=0;
       list_add(vector_frames_memoria, frame);
    }
}

bool memoria_frame_libre(m_frame_t* frame){ //warning
    bool respuesta=(frame->presencia)==0? true : false;
}

bool memoria_iniciar_patota(int pid, const char* lista_tareas, int cant_tripulantes, int frames_necesarios){
    //semaforo en vector_frames_memoria?
    int i;
    if(memoria_escribir_paginacion(pid, lista_tareas)){
        for(i=0; i<frames_necesarios; i++){
            m_frame_t* frame_a_escribir = list_find(vector_frames_memoria, memoria_frame_libre);//warning
            frame_a_escribir->presencia = 1;    
        }
        memoria_mostrar_frames(32);//borrar linea
        return true;
    }else{
        return false;
    }
    
}

void memoria_mostrar_frames(int paginas){
    int i;
    for(i=0; i<paginas; i++){
        m_frame_t* frame_a_mostrar = list_get(vector_frames_memoria,i);
        U_LOG_TRACE("Frame: %d, presencia: %d", i, frame_a_mostrar->presencia);
    }
}

bool memoria_escribir_paginacion(uint32_t pid, const char* lista_tareas){ //TODO deberia enviar los datos armados para guardar. 
    m_frame_t* frame_a_escribir = list_find(vector_frames_memoria, memoria_frame_libre);
    int frame = frame_a_escribir->num_frame;
    char* a_guardar = string_new();
    string_append(&a_guardar, string_itoa(pid)); 
    string_append(&a_guardar, lista_tareas);
    strcpy(memoria_fisica+(frame*tamanio_pagina),a_guardar);
    //strcpy(memoria_fisica+(frame*tamanio_pagina)+sizeof(uint32_t)+string_length(lista_tareas), string_itoa(pid));
    char* a_mostrar = string_substring(memoria_fisica+(frame*tamanio_pagina), 0, sizeof(uint32_t)+string_length(lista_tareas));
    
    U_LOG_TRACE("RECUPERACION: %s",a_mostrar);
   // memoria_recuperar_paginacion();
    return true;
}

void memoria_recuperar_paginacion(){
    U_LOG_TRACE(memoria_fisica);
}

bool memoria_tiene_frames_libres(int frames){
    t_list* lista_frame_libres = list_filter(vector_frames_memoria, memoria_frame_libre);
    int frames_libres = list_size(lista_frame_libres);
    list_destroy(lista_frame_libres);
    return frames_libres >= frames;
}

int memoria_frames_necesarios(int tamanio_data){
    return tamanio_data/tamanio_pagina; 
} 

