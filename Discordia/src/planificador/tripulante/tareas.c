#include "tareas.h"
#include <stdlib.h>
#include <string.h>

private bool tarea_es_bloqueante (char* tarea);

tarea_t* parsear_tarea (char* tarea_sin_parsear){
    if(tarea_sin_parsear == NULL)
        return NULL;
    
    tarea_t* tarea = malloc(sizeof(tarea_t));
    char** tarea_separada = string_split(tarea_sin_parsear, ";");
    tarea->tarea = string_duplicate(tarea_separada[0]);
    tarea->is_blocking = tarea_es_bloqueante(tarea_separada[0]);
    tarea->is_finished = false;
    tarea->tiempo_bloqueado = (uint32_t) atoi(tarea_separada[3]);
    tarea->pos.x = (uint32_t) atoi(tarea_separada[1]);
    tarea->pos.y = (uint32_t) atoi(tarea_separada[2]);
    string_iterate_lines(tarea_separada, (void*) free);
    free(tarea_separada);

    return tarea;
}

void eliminar_tarea (tarea_t* tarea){
    free(tarea->tarea);
    free(tarea);
}

private bool tarea_es_bloqueante (char* tarea){
    return string_contains(tarea, " ") || string_contains(tarea, "DESCARTAR_BASURA");
}