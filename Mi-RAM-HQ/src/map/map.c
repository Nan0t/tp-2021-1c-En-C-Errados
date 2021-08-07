#include "memoria/esquemas/esquema_memoria.h"
#include "map.h"

#include <tad_nivel.h>

#include <pthread.h>
#include <unistd.h>

#define SELECT_PRINTABLE_ASCII_CHAR(tid)    ((tid) + ' ')
#define RENDER_DELAY 1

typedef struct
{
    uint32_t tid;
    u_pos_t  pos;
} map_entity_t;

typedef struct
{
    pthread_mutex_t mx;
    NIVEL*          level;
    t_list*         entitys;
} map_t;

private map_t* p_map = NULL;

private void map_loop(void);

void map_init(void)
{
    if(p_map)
        return;

    nivel_gui_inicializar();

    p_map = u_malloc(sizeof(map_t));

    p_map->entitys = list_create();
    p_map->level   = nivel_crear("Mi-RAM HQ");

    pthread_mutex_init(&p_map->mx, NULL);

    pthread_t map_thread;
    U_ASSERT(pthread_create(&map_thread, NULL, (void*)map_loop, NULL) != -1,
        "No se pudo inicializar el hilo del mapa");

    pthread_detach(map_thread);
}

void map_add_trip(uint32_t tid, const u_pos_t* pos)
{
    map_entity_t* entity = u_malloc(sizeof(map_entity_t));

    entity->tid   = tid;
    entity->pos.x = pos->x;
    entity->pos.y = pos->y;

    pthread_mutex_lock(&p_map->mx);
    personaje_crear(p_map->level, SELECT_PRINTABLE_ASCII_CHAR(tid), pos->x, pos->y);
    list_add(p_map->entitys, entity);
    pthread_mutex_unlock(&p_map->mx);
}

void map_move_trip(uint32_t tid, const u_pos_t* pos)
{
    pthread_mutex_lock(&p_map->mx);

    bool _is_the_entity(map_entity_t* e) {
        return e->tid == tid;
    };

    map_entity_t* trip = list_find(p_map->entitys, (void*)_is_the_entity);

    if(trip == NULL)
    {
        U_LOG_WARN("Se intento modificar la posicion de un tripulante inexistente. TID: %d", tid);
        return;
    }

    trip->pos.x = pos->x;
    trip->pos.y = pos->y;
    
    pthread_mutex_unlock(&p_map->mx);
}

void map_del_trip(uint32_t tid)
{
    pthread_mutex_lock(&p_map->mx);
    item_borrar(p_map->level, SELECT_PRINTABLE_ASCII_CHAR(tid));
    
    bool _is_the_entity(map_entity_t* e) {
        return e->tid == tid;
    };

    list_remove_and_destroy_by_condition(p_map->entitys, (void*)_is_the_entity, u_free);
    pthread_mutex_unlock(&p_map->mx);
}

private void map_loop(void)
{
    while(1)
    {
        pthread_mutex_lock(&p_map->mx);
        t_list_iterator* it = list_iterator_create(p_map->entitys);

        while(list_iterator_has_next(it))
        {
            map_entity_t* trip = list_iterator_next(it);
            item_mover(p_map->level, SELECT_PRINTABLE_ASCII_CHAR(trip->tid), trip->pos.x, trip->pos.y);
        }
        list_iterator_destroy(it);

        nivel_gui_dibujar(p_map->level);
        pthread_mutex_unlock(&p_map->mx);

        sleep(RENDER_DELAY);
    }
}

