#include "bitacora.h"
#include "fs/blocks/blocks_manager.h"

struct fs_bitacora_t
{
    //TODO: Agregar los campos necesarios.
};

fs_bitacora_t* fs_bitacora_create(const char* mount_point, uint32_t tid)
{
    fs_bitacora_t* this = u_malloc(sizeof(fs_bitacora_t));

    //TODO: Inicializar los campos que sean necesarios de la bitacora.

    return this;
}

void fs_bitacora_delete(fs_bitacora_t* this)
{
    //TODO: Eliminar la estructura de la bitacora y liberar los bloques que tenga asignado.
}

void fs_bitacora_add_content(fs_bitacora_t* this, const char* content)
{
    //TODO: Se debe guardar el contenido de "content" en la bitacora. Para esto
    // se debe intentar escribir el contenido al final del ultimo bloque disponible.
    // En caso de no poder escribirse todo el contenido, pedir un nuevo bloque y escribir
    // la informacion restante en el nuevo bloque. En caso de que tampoco se pueda escribir
    // todo el contenido en el nuevo bloque, seguir pidiendo bloqueas hasta terminar de escribir
    // todo el contenido.
}

bool fs_bitacora_check_integrity(fs_bitacora_t* this)
{
    //TODO: Se debe devolver true o false dependiendo si la bitacora esta corrompida o no.
    // El criterio que se va a seguir para saber si una bitacora esta corrompida o no, se
    // especifica en el enunciado del TP.

    return false;
}

uint32_t fs_bitacora_get_tid(const fs_bitacora_t* this)
{
    //TODO: Devolver el TID del tripulante.
    return 0;
}

uint32_t fs_bitacora_get_size(const fs_bitacora_t* this)
{
    //TODO: Devolver el tamaño de la bitacora.
    return 0;
}

char* fs_bitacora_get_content(const fs_bitacora_t* this)
{
    //TODO: Devolver el contenido de la bitacora. Leyendo los bloques de la bitacora.
    return NULL;
}

