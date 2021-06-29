#include "memoria/esquemas/esquema_memoria.h"
#include "map.h"

#include <tad_nivel.h>

#include <pthread.h>
#include <unistd.h>

#define SELECT_PRINTABLE_ASCII_CHAR(tid)    ((tid) + ' ')
#define RENDER_DELAY 5

typedef struct
{
    pthread_mutex_t mx;
    NIVEL*          level;
} map_t;

private map_t* p_map = NULL;

private void map_loop(void);

void map_init(void)
{
    if(p_map)
        return;

    nivel_gui_inicializar();

    p_map = u_malloc(sizeof(map_t));

    p_map->level = nivel_crear("Mi-RAM HQ");
    pthread_mutex_init(&p_map->mx, NULL);

    pthread_t map_thread;
    U_ASSERT(pthread_create(&map_thread, NULL, (void*)map_loop, NULL) != -1,
        "No se pudo inicializar el hilo del mapa");

    pthread_detach(map_thread);
}

void map_add_trip(uint32_t tid, const u_pos_t* pos)
{
    pthread_mutex_lock(&p_map->mx);
    personaje_crear(p_map->level, SELECT_PRINTABLE_ASCII_CHAR(tid), pos->x, pos->y);
    pthread_mutex_unlock(&p_map->mx);
}

void map_del_trip(uint32_t tid)
{
    pthread_mutex_lock(&p_map->mx);
    item_borrar(p_map->level, SELECT_PRINTABLE_ASCII_CHAR(tid));
    pthread_mutex_unlock(&p_map->mx);
}

private void map_loop(void)
{
    while(1)
    {
        t_list* trips = esquema_memoria_obtener_todos_los_tripulantes();
        t_list_iterator* it = list_iterator_create(trips);

        pthread_mutex_lock(&p_map->mx);

        while(list_iterator_has_next(it))
        {
            tripulantes_t* trip = list_iterator_next(it);
            item_mover(p_map->level, SELECT_PRINTABLE_ASCII_CHAR(trip->tid), trip->pos.x, trip->pos.y);
        }

        nivel_gui_dibujar(p_map->level);
        pthread_mutex_unlock(&p_map->mx);

        list_iterator_destroy(it);
        list_destroy_and_destroy_elements(trips, u_free);

        sleep(RENDER_DELAY);
    }
}

