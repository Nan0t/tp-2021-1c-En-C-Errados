#ifndef DISCORDIA_PLANIFICADOR_QUEUE_MT_H
#define DISCORDIA_PLANIFICADOR_QUEUE_MT_H

#include "planificador/tripulante/tripulante.h"

/**
 * Forward declaration. Definicion del tipo de la cola multihilo.
 */
typedef struct ds_queue_mt_t ds_queue_mt_t;

/**
 * @NAME: ds_queue_mt_iterator_t
 * @DESC: iterador de un ds_queue_mt_t
 */
typedef struct
{
    ds_queue_mt_t*   queue;
    t_list_iterator* it;
} ds_queue_mt_iterator_t;

/**
 * @NAME: ds_queue_mt_create
 * @DESC: crea una estructura ds_queue_mt_t
 * @PARAMS:
 *  [in] uint32_t max_elements - cantidad maxima de elementos que puede mantener la cola. En caso de querer
 *                                  mantener un numero ilimitado de tripulantes, se debe pasar 0.
 */
ds_queue_mt_t* ds_queue_mt_create(uint32_t max_elements);

/**
 * @NAME: ds_queue_mt_delete
 * @DESC: elimina una instancia ds_queue_mt_t
 * @PARAMS:
 *  [in] ds_queue_mt_t* queue - cola que se quiere eliminar.
 */
void ds_queue_mt_delete(ds_queue_mt_t* queue);

/**
 * @NAME: ds_queue_mt_delete_and_delete_elements
 * @DESC: elimina una instancia de ds_queue_mt_t y sus tripulantes.
 * @PARAMS:
 *  [in] ds_queue_mt_t* queue - cola que se quiere eliminar junto con sus tripulantes.
 */
void ds_queue_mt_delete_and_delete_elementes(ds_queue_mt_t* queue);

/**
 * @NAME: ds_queue_mt_is_full
 * @DESC: permite saber si una cola esta llena (en caso de que se le haya asignado un limite de tripulantes)
 * @PARAMS:
 *  [in] const ds_queue_mt_t* queue - cola de la cual se quiere consultar si esta llena.
 * 
 * @RETURNS: true en caso que la cola este llena. De lo contrario, false.
 */
bool     ds_queue_mt_is_full(const ds_queue_mt_t* queue);

/**
 * @NAME: ds_queue_mt_get_size
 * @DESC: devuelve la cantidad de tripulante en una cola.+
 * @PARAMS:
 *  [in] const ds_queue_mt_t* queue - cola de la cual se quiere obtener su tamaño.
 * 
 * @RETURNS: la cantidad de tripulantes en la cola.
 */
uint32_t ds_queue_mt_get_size(const ds_queue_mt_t* queue);

/**
 * @NAME: ds_queue_mt_push
 * @DESC: agrega una nueva referencia a un tripulante en la cola.
 * @PARAMS:
 *  [in] ds_queue_mt_t* queue - cola a la cual se va a agregar el tripulante.
 *  [in] tripulante_t*  trip  - tripulante a agregar.
 */
void ds_queue_mt_push(ds_queue_mt_t* queue, tripulante_t* trip);

/**
 * @NAME: ds_queue_mt_pop
 * @DESC: obtiene el siguiente tripulante de la cola
 * @PARAMS:
 *  [in] ds_queue_mt_t* queue - cola de la cual se quiere obtener el proximo tripulante.
 * 
 * @RETRUNS: el proximo tripulante, o NULL en caso de que la cola este vacia.
 */
tripulante_t* ds_queue_mt_pop(ds_queue_mt_t* queue);

/**
 * @NAME: ds_queue_mt_pop_by_tid
 * @DESC: obtiene un tripulante de la cola por su id.
 * @PARAMS:
 *  [in] ds_queue_mt_t* queue - cola de la cual se quiere obtener el tripulante solicitado.
 * 
 * @RETRUNS: el tripulante con el id solicitado, o NULL en caso de que no exista.
 */
tripulante_t* ds_queue_mt_pop_by_tid(ds_queue_mt_t* queue, uint32_t tid);

/**
 * @NAME: ds_queue_mt_get_iterator
 * @DESC: devuelve un iterador de la cola
 * @PARAMS:
 *  [in] const ds_queue_mt_t* queue - cola de la cual se quiere obtener el iterador.
 * 
 * @RETURNS: un iterador de la cola
 */
ds_queue_mt_iterator_t* ds_queue_mt_get_iterator(const ds_queue_mt_t* queue);

/**
 * @NAME: ds_qeuue_mt_iterator_next
 * @DESC: devuelve el tripulante referenciado por el iterador y avanza el iterador un lugar.
 * @PARAMS:
 *  [in] ds_queue_mt_iterator_t* it - iterador del cual se quiere obtener el tripulante.
 * 
 * @RETURNS: el tripulante referenciado por el iterador.
 */
tripulante_t* ds_queue_mt_iterator_next(ds_queue_mt_iterator_t* it);

/**
 * @NAME: ds_queue_mt_iterator_has_next
 * @DESC: permite saber si el iterador tiene un siguiente elemento o no.
 * @PARAMS:
 *  [in] ds_queue_mt_iterator_t* it - iterador del cual se quiere conocer si posee un proximo elemento.
 * 
 * @RETURNS: true en caso que posea un proximo elemento. De lo contrario, false.
 */
bool ds_queue_mt_iterator_has_next(ds_queue_mt_iterator_t* it);

/**
 * @NAME: ds_queue_mt_iterator_remove
 * @DESC: elimina el nodo referenciado por el iterador de la cola.
 * @PARAMS:
 *  [in] ds_queue_mt_iterator_t* it - iterador del cual se quiere eliminar el nodo al que hace referencia de la cola.
 */
void ds_queue_mt_iterator_remove(ds_queue_mt_iterator_t* it);

/**
 * @NAME: ds_queue_mt_iterator_delete
 * @DESC: elimina una instancia de iterador
 * @PARAMS:
 *  [in] ds_queue_mt_iterator_t* it - iterador que se quiere eliminar.
 */
void ds_queue_mt_iterator_delete(ds_queue_mt_iterator_t* it);

/**
 * @NAME: ds_queue_mt_sort_by_lowest_tid
 * @DESC: ordena la cola por el tid de menor a mayor
 * @PARAMS:
 *  [in] ds_queue_mt_t* queue - cola que se quiere ordenar.
 */
void ds_queue_mt_sort_by_lowest_tid(ds_queue_mt_t* queue);

/**
 * Los siguientes macros son helpers para poder iterar de mejor manera la cola.
 * Se puede usar de la siguiente manera:
 *  
 * ds_queue_mt_t* queue = ds_queue_mt_create(0);
 * .
 * .
 * .
 * DS_QUEUE_MT_FOREACH(queue)
 * 
 *  tripulante_t* trip = ds_queue_mt_iterator_next(it);
 * 
 * DS_QUEUE_MT_END_FOREACH
 * 
 * El codigo encerrado entre las macros habilita un iterador it de la cola que va recorriendo la cola hasta finalizar.
 * Existe una version DS_QUEUE_MT_FOREACH_INDEX que permite llevar los inidices de cada iteracion. En caso de quere
 * hacer un return o un break en mitad de un foreach, utilizar los macros DS_QUEUE_MT_RETURN_FOREACH(ret_value) y
 * DS_QUEUE_MT_BREAK_FOREACH
 */
#define DS_QUEUE_MT_FOREACH(queue) {\
    ds_queue_mt_iterator_t* it = ds_queue_mt_get_iterator(queue);\
    while(ds_queue_mt_iterator_has_next(it)) {

#define DS_QUEUE_MT_FOREACH_INDEX(i, queue) {\
    ds_queue_mt_iterator_t* it = ds_queue_mt_get_iterator(queue);\
    for(uint32_t i = 0; ds_queue_mt_iterator_has_next(it); i ++) {

#define DS_QUEUE_MT_END_FOREACH } ds_queue_mt_iterator_delete(it); }
#define DS_QUEUE_MT_RETURN_FOREACH(...) ds_queue_mt_iterator_delete(it); return __VA_ARGS__;
#define DS_QUEUE_MT_BREAK_FOREACH break;

#endif