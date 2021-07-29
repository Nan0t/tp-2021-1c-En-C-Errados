#include "blocks_manager.h"
#include "fs/disk/disk.h"
#include "fs/bitacora/bitacoras_manager.h"
#include "fs/file/files_manager.h"

#include <commons/bitarray.h>

#include <pthread.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/fcntl.h>

#include <string.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>

typedef struct
{
    uint32_t number;
    uint32_t disk_offset;
} fs_block_t;

typedef struct
{
    fs_block_t*     blocks;
    pthread_mutex_t blocks_mx;

    uint32_t blocks_size;
    uint32_t blocks_count;

    char*    mount_point;

    pthread_mutex_t blocks_bitmap_mx;
    t_bitarray*     bitmap;
} fs_blocks_manager_t;

private void     fs_block_init(fs_block_t* block, uint32_t number, uint32_t disk_offset);
private uint64_t fs_block_write_internal(const fs_block_t* this, const void* data, uint64_t data_size, uint64_t offset);
private uint64_t fs_block_read_internal(const fs_block_t* this, void* data, uint64_t data_size, uint64_t offset);

private fs_blocks_manager_t* p_blocks_manager_instance = NULL;

private void     fs_blocks_manager_init_super_bloque(const char* super_block_file_path);
private void     fs_blocks_manager_get_metadata(int32_t super_block_file);
private void     fs_blocks_manager_init_disk(void);
private void     fs_blocks_manager_init_blocks(void);
private uint32_t fs_blocks_manager_get_free_block_index(void);
private void     fs_blocks_manager_release_block_at(uint32_t index);
private bool     fs_block_manager_verificar_integridad_bitmap(void);
private bool     fs_block_manager_verficar_integridad_cantidad_bloques(void);

void fs_blocks_manager_init(const char* mount_point, bool is_clean_initialization)
{
    if(p_blocks_manager_instance != NULL)
        return;

    p_blocks_manager_instance = u_malloc(sizeof(fs_blocks_manager_t));
    p_blocks_manager_instance->mount_point = strdup(mount_point);

    pthread_mutex_init(&p_blocks_manager_instance->blocks_bitmap_mx, NULL);
    pthread_mutex_init(&p_blocks_manager_instance->blocks_mx, NULL);

    char* super_block_file_path = string_from_format("%sSuperBloque.ims", p_blocks_manager_instance->mount_point);

    if(is_clean_initialization)
    {
        fs_blocks_manager_init_super_bloque(super_block_file_path);
        char* blockims = string_from_format("%sBlocks.ims", p_blocks_manager_instance->mount_point);
        fclose(fopen(blockims, "w"));
        u_free(blockims);
    }

    int32_t super_block_file = open(super_block_file_path, O_RDWR, 0666);
    U_ASSERT(super_block_file != -1, "No se pudo abrir el archivo de SuperBloque.ims: %s", strerror(errno));

    u_free(super_block_file_path);

    fs_blocks_manager_get_metadata(super_block_file);
    fs_blocks_manager_init_disk();
    fs_blocks_manager_init_blocks();
}

uint32_t fs_blocks_manager_request_block(void)
{
    return fs_blocks_manager_get_free_block_index();
}

void fs_blocks_manager_release_block(uint32_t block_id)
{
    fs_blocks_manager_release_block_at(block_id);
}

uint64_t fs_blocks_manager_write_in_block(uint32_t block_id, const void* data, uint64_t data_size, uint64_t offset)
{
    pthread_mutex_lock(&p_blocks_manager_instance->blocks_mx);
    uint64_t bytes_written =
        fs_block_write_internal(&p_blocks_manager_instance->blocks[block_id - 1], data, data_size, offset);
    pthread_mutex_unlock(&p_blocks_manager_instance->blocks_mx);
    
    return bytes_written;
}

uint64_t fs_blocks_manager_read_in_block(uint32_t block_id, void* data, uint64_t data_size, uint64_t offset)
{
    pthread_mutex_lock(&p_blocks_manager_instance->blocks_mx);

    uint32_t bytes_written =
        fs_block_read_internal(&p_blocks_manager_instance->blocks[block_id - 1], data, data_size, offset);

    pthread_mutex_unlock(&p_blocks_manager_instance->blocks_mx);
    
    return bytes_written;
}

uint32_t fs_blocks_manager_get_disk_offset_of_block(uint32_t block_id)
{
    return p_blocks_manager_instance->blocks[block_id - 1].disk_offset;
}

uint32_t fs_blocks_manager_get_blocks_size(void)
{
    return p_blocks_manager_instance->blocks_size;
}

bool fs_blocks_manager_check_integrity(void)
{
    return fs_block_manager_verificar_integridad_bitmap() && fs_block_manager_verficar_integridad_cantidad_bloques();
}

// =======================================================
//              *** Private Functions ***
// =======================================================

private void fs_block_init(fs_block_t* this, uint32_t number, uint32_t disk_offset)
{
    this->number      = number;
    this->disk_offset = disk_offset;
}

private uint64_t fs_block_write_internal(const fs_block_t* this, const void* data, uint64_t data_size, uint64_t offset)
{
    uint64_t bytes_to_write =
        (data_size + offset < p_blocks_manager_instance->blocks_size) ?
            data_size :
            data_size - (data_size + offset - p_blocks_manager_instance->blocks_size);

    fs_physical_disk_write(data, bytes_to_write, this->disk_offset + offset);

    return bytes_to_write;
}

private uint64_t fs_block_read_internal(const fs_block_t* this, void* data, uint64_t data_size, uint64_t offset)
{
    uint64_t bytes_to_read =
        (data_size + offset < p_blocks_manager_instance->blocks_size) ?
            data_size :
            data_size - (data_size + offset - p_blocks_manager_instance->blocks_size);

    fs_physical_disk_read(data, bytes_to_read, this->disk_offset + offset);

    return bytes_to_read;
}

private void fs_blocks_manager_init_super_bloque(const char* super_block_file_path)
{
    uint32_t block_size = u_config_get_int_value("BLOCK_SIZE");
    uint32_t blocks     = u_config_get_int_value("BLOCKS");
    
    uint64_t bitmap_length = 1 + (blocks / sizeof(char));

    void* zero_memory = u_malloc(bitmap_length + 1);
    memset(zero_memory, 0, bitmap_length);
    
    fclose(fopen(super_block_file_path, "w"));

    int32_t super_block_file = open(super_block_file_path, O_RDWR, 0666);
    U_ASSERT(super_block_file != -1, "No se pudo abrir el archivo de SuperBloque.ims: %s", strerror(errno));

    write(super_block_file, &block_size, sizeof(uint32_t));
    write(super_block_file, &blocks, sizeof(uint32_t));
    write(super_block_file, zero_memory, bitmap_length);

    close(super_block_file);

    u_free(zero_memory);
}

private void fs_blocks_manager_get_metadata(int32_t super_block_file)
{
    U_ASSERT(pread(super_block_file, &p_blocks_manager_instance->blocks_size, sizeof(uint32_t), 0) != -1,
        "No se pudo obtener el tamaño de los bloques del SuperBloque.ims: %s", strerror(errno));

    U_ASSERT(pread(super_block_file, &p_blocks_manager_instance->blocks_count, sizeof(uint32_t), sizeof(uint32_t)) != -1,
        "No se pudo obtener la cantidad de los bloques del SuperBloque.ims: %s", strerror(errno));
    
    uint32_t bitmap_size = ceil(p_blocks_manager_instance->blocks_count / 8);
    void*    bitmap_mem  =
        mmap(NULL, bitmap_size, PROT_READ | PROT_WRITE, MAP_SHARED, super_block_file, 0);

    U_ASSERT(bitmap_mem != MAP_FAILED,
        "No se pudo mapear el bitmap del SuperBloque.ims: %s", strerror(errno));

    p_blocks_manager_instance->bitmap = bitarray_create(bitmap_mem + sizeof(uint32_t) * 2, bitmap_size);
}

private void fs_blocks_manager_init_disk(void)
{
    fs_physical_disk_init(p_blocks_manager_instance->blocks_size * p_blocks_manager_instance->blocks_count);
}

private void fs_blocks_manager_init_blocks(void)
{
    p_blocks_manager_instance->blocks = u_malloc(p_blocks_manager_instance->blocks_count * sizeof(fs_block_t));

    for(uint32_t i = 0; i < p_blocks_manager_instance->blocks_count; i ++)
        fs_block_init(
            &p_blocks_manager_instance->blocks[i],
            i + 1,
            i * p_blocks_manager_instance->blocks_size);
}

private uint32_t fs_blocks_manager_get_free_block_index(void)
{
    uint32_t index = 0;

    pthread_mutex_lock(&p_blocks_manager_instance->blocks_bitmap_mx);

    for(uint32_t i = 0; i < p_blocks_manager_instance->blocks_count; i ++)
    {
        if(bitarray_test_bit(p_blocks_manager_instance->bitmap, i) == 0)
        {
            index = i + 1;
            bitarray_set_bit(p_blocks_manager_instance->bitmap, i);
            break;
        }
    }

    pthread_mutex_unlock(&p_blocks_manager_instance->blocks_bitmap_mx);

    return index;
}

private void fs_blocks_manager_release_block_at(uint32_t index)
{
    U_ASSERT(index <= p_blocks_manager_instance->blocks_count, "Se intenta liberar un bloque invalido");

    pthread_mutex_lock(&p_blocks_manager_instance->blocks_bitmap_mx);
    bitarray_clean_bit(p_blocks_manager_instance->bitmap, index - 1);
    pthread_mutex_unlock(&p_blocks_manager_instance->blocks_bitmap_mx);
}

private bool fs_block_manager_verificar_integridad_bitmap(void)
{
    bool fue_saboteado = false;
    t_list* id_bloques_totales = fs_files_manager_get_blocks_id();
    t_list* id_bloques_bitacoras = fs_bitacoras_manager_get_blocks_id();
    list_add_all(id_bloques_totales, id_bloques_bitacoras);
    uint32_t numero_bloque_buscado;
    u_free(id_bloques_bitacoras);


    void _setear_bit_de_bloque_usado(uint32_t* id_bloque)
    {
        bitarray_set_bit(p_blocks_manager_instance->bitmap, *id_bloque - 1);
    }

    bool _ordenar_menor_a_mayor(uint32_t* id_bloque_uno, uint32_t* id_bloque_dos)
    {
        return *id_bloque_uno < *id_bloque_dos;
    }

    bool _tiene_valor(uint32_t* id_bloque)
    {
        return *id_bloque == numero_bloque_buscado;
    }
    


    list_sort(id_bloques_totales, (void*)_ordenar_menor_a_mayor);

    pthread_mutex_lock(&p_blocks_manager_instance->blocks_bitmap_mx);
    size_t tamanio_bitarray = bitarray_get_max_bit(p_blocks_manager_instance->bitmap);
    for(uint32_t i = 0; i < tamanio_bitarray; i++)
    {
        if(bitarray_test_bit(p_blocks_manager_instance->bitmap, i) == 1)
        {
            numero_bloque_buscado = i + 1;
            if(!list_is_empty(id_bloques_totales) && list_any_satisfy(id_bloques_totales, (void*)_tiene_valor))
            {
                list_remove_and_destroy_by_condition(id_bloques_totales, (void*)_tiene_valor, u_free);
            }
            else
            {
                bitarray_clean_bit(p_blocks_manager_instance->bitmap, i);
                fue_saboteado = true;
            }
        }
    }

    if(!list_is_empty(id_bloques_totales))
    {
        list_iterate(id_bloques_totales, (void*)_setear_bit_de_bloque_usado);
        list_iterate(id_bloques_totales, u_free);
        fue_saboteado = true;
    }

    pthread_mutex_unlock(&p_blocks_manager_instance->blocks_bitmap_mx);

    u_free(id_bloques_totales);

    return fue_saboteado;

}

private bool fs_block_manager_verficar_integridad_cantidad_bloques (void)
{
    uint32_t block_size = p_blocks_manager_instance->blocks_size;
    uint32_t blocks_count = p_blocks_manager_instance->blocks_count;
    uint32_t tamanio_archivo_bloques = fs_physical_disk_get_size();
    bool     fue_saboteado = false;
    uint32_t cant_real_bloques = tamanio_archivo_bloques/ block_size;

    if (cant_real_bloques!= blocks_count)
    {
        p_blocks_manager_instance->blocks_count = cant_real_bloques;
        fue_saboteado = true;
    }

    return fue_saboteado;
}

#ifdef UTESTS
const fs_block_t* fs_block_manager_get_block_instance(uint32_t block_indez)
{
    return p_blocks_manager_instance->blocks[block_index];
}
#endif
