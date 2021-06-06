#ifndef MEMORIA_H
#define MEMORIA_H

#include <utils/utils.h>


typedef struct
{
    uint32_t pid;
    t_list* tabla;
}m_pid_con_tabla_t; 

typedef struct
{
    uint32_t frame_memoria;
    uint32_t frame_swap;
}m_fila_tabla_paginas_t;

typedef struct
{
    uint32_t num_frame;
    uint32_t presencia;
    uint32_t tiempo_en_memoria;
    uint32_t uso;
    //uint32_t pid;
}m_frame_t; //estos bits pueden ir en la tabla de paginas directamente? y hacer un vector de frames libres nomas?

typedef enum
{
    SEGMENTACION,
    PAGINACION
}m_esquema_e;

char* memoria_fisica;
int tamanio_pagina;
t_list* listado_patotas;
int esquema;
t_list* vector_frames_memoria; 
int* vector_frame_swap;//lista tmb?

void memoria_iniciar();
int memoria_levantar_esquema();
void memoria_inicializar_frames_memoria(int paginas);
bool memoria_frame_libre(m_frame_t* frame);
bool memoria_iniciar_patota(int pid, const char* lista_tareas, int cant_tripulantes, int frames_necesarios);
void memoria_mostrar_frames(int paginas);
bool memoria_escribir_paginacion(uint32_t pid, const char* lista_tareas);
void memoria_recuperar_paginacion();
bool memoria_tiene_frames_libres(int frames); //ver si se puede usar para swap tmb
int memoria_frames_necesarios(int tamanio_data);
#endif