#include "file.h"
#include "fs/blocks/blocks_manager.h"

#include <pthread.h>

struct fs_file_t
{
    //TODO: Agregar los campos necesarios.
};

fs_file_t* fs_file_create(const char* mount_point, const char* file_name, char fill_char)
{
    fs_file_t* this = u_malloc(sizeof(fs_file_t));
    //TODO: Inicializar los campos que sean necesarios del file.

    return this;
}

void fs_file_delete(fs_file_t* this)
{
    //TODO: Eliminar estructuras faltantes
    u_free(this);
}

void fs_file_add_fill_char(fs_file_t* this, uint32_t amount)
{
    //TODO: Agregar la cantidad especificada por "amount" de caracteres de llenado en el file.
    // En caso de no poder escribirse todo el contenido, pedir un nuevo bloque y escribir
    // la cantidad de caracteres restantes. En caso de que tampoco se pueda escribir
    // todo el contenido en el nuevo bloque, seguir pidiendo bloqueas hasta terminar de escribir
    // la cantidad total de caracteres.
}

void fs_file_remove_fill_char(fs_file_t* this, uint32_t amount)
{
    //TODO: Eliminar la cantidad especificada por "amount" de caracteres de llenado en el file.
    // Esto se puede hacer simplemente reduciendo el tamaño del archivo. Luego, se deberia dividir
    // el tamaño actual del archivo por el tamaño de un bloque (que puede ser consultado mediante la
    // funcion "fs_blocks_manager_get_blocks_size"). El resultado sera la cantidad de bloques que el
    // archivo estaría usando. Si es menor a la cantidad de bloques que posee actualmente, liberar los
    // ultimos bloques del archivo. Por ejemplo, si despues de reducir el tamaño del archivo, la cantidad
    // de bloques que ocupa el archivo son 4 y el archivo tiene ocupados 6 bloques, se deben liberar,
    // los últimos 2 bloques (llamando a la función "fs_blocks_manager_release_block(num_bloque)")
}

bool fs_file_check_integrity(fs_file_t* this)
{
    //TODO: Se debe devolver true o false dependiendo si el archivo fue corrompido o no.
    // El criterio que se va a seguir para saber si un file fue corrompido o no, se
    // especifica en el enunciado del TP.
    return false;
}

const char* fs_file_get_name(const fs_file_t* this)
{
    //TODO: Devolver el nombre del archivo.
    return NULL;
}

const char* fs_file_get_md5(const fs_file_t* this)
{
    //TODO: Devolver el MD5 del archivo.
    return NULL;
}

uint32_t fs_file_get_size(const fs_file_t* this)
{
    //TODO: Devolver el tamaño del archivo.
    return 0;
}

char fs_file_get_fill_char(const fs_file_t* file)
{
    //TODO: Devolver el caracter de llenado del archivo.
    return '\0';
}

uint32_t fs_file_get_blocks_count(const fs_file_t* file)
{
    //TODO: Devolver la cantidad de bloques disponibles.
    return 0;
}