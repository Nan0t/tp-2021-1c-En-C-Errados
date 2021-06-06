#include "admin_memoria.h"
#include <stdlib.h>
#include <time.h>

//iniciar patota
bool admin_memoria_iniciar_patota(uint32_t pid, const char* lista_tareas)
{

}

bool admin_memoria_paginacion_iniciar_patota(uint32_t pid, const char* lista_tareas){ //TODO esto va a recibir la cantidad de tripulantes, para calcular los frames
    int cantidad_de_tripulantes; // en realidad se recibe por parametro
    int tamanio_data = 2 + 21* cantidad_de_tripulantes; 
    int frames_necesarios = memoria_frames_necesarios(tamanio_data);
    if(memoria_tiene_frames_libres(frames_necesarios)){ //TODO aca primero tendria q armar los frames necesarios para esa patota
        m_pid_con_tabla_t* patota = u_malloc(sizeof(patota));
        patota->pid = pid;
        patota->tabla = list_create();
        list_add(listado_patotas, patota);
        //return true;   
        return  memoria_iniciar_patota(pid, lista_tareas, cantidad_de_tripulantes, frames_necesarios);
    }else{
        return false;
    }
    
    //return memoria_escribir_paginacion(pid, lista_tareas);
    //return true;
}

bool admin_memoria_iniciar_tripulante(uint32_t pid, uint32_t tid,  u_pos_t pos)//cambiado const u_pos_t*
{
    return true;
}

void admin_memoria_mover_tripulante(uint32_t tid, u_pos_t pos)//cambiado const u_pos_t*
{

}

char* admin_memoria_obtener_proxima_tarea(uint32_t tid)
{
    return string_duplicate("GENERAR_OXIGENO 12;2;3;5");
}

void admin_memoria_tripulante_nuevo_estado(uint32_t tid, char estado)
{

}

void admin_memoria_eliminar_tripulante(uint32_t tid)
{

}

t_list* admin_memoria_obtener_tripulantes(void)
{
    t_list* tripulantes = list_create();
    tripulantes_t* trip = NULL;

    srand(time(NULL));

    const char states[] = { 'R', 'B', 'E' };

    for(uint32_t i = 0; i < 5; i ++)
    {
        trip = u_malloc(sizeof(tripulantes_t));

        trip->pid = 10;
        trip->tid = 10 + i;
        trip->pos.x = rand() % 10;
        trip->pos.y = rand() % 10;
        trip->estado = states[rand() % 3];

        list_add(tripulantes, trip);
    }

    return tripulantes;
}

