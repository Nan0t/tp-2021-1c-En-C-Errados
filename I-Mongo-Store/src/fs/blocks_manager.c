#include "blocks_manager.h"
#include "disk.h"

#include <pthread.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/fcntl.h>

#include <string.h>
#include <errno.h>
#include <math.h>

#define BLOCKS_SIZE(super_block_mem)   *((uint32_t*)(super_block_mem))
#define BLOCKS_COUNT(super_block_mem)  *((uint32_t*)(super_block_mem + sizeof(uint32_t)))
#define BLOCKS_BITMAP_BYTE(super_block_mem, byte) *((uint8_t*)(super_block_mem + sizeof(uint32_t) * 2 + byte))

typedef struct
{
    uint32_t number;
    uint32_t disk_offset;
    uint32_t block_size;
} fs_block_t;

typedef struct
{
    fs_block_t*     blocks;
    pthread_mutex_t blocks_mx;

    uint32_t blocks_size;
    uint32_t blocks_count;

    pthread_mutex_t blocks_bitmap_mx;
    uint32_t        blocks_bitmap_size;
    char*           blocks_bitmap;
} fs_blocks_manager_t;

private void     fs_block_init(fs_block_t* block, uint32_t number, uint32_t disk_offset, uint32_t block_size);
private uint64_t fs_block_write_internal(const fs_block_t* this, const void* data, uint64_t data_size, uint64_t offset);
private uint64_t fs_block_read_internal(const fs_block_t* this, void* data, uint64_t data_size, uint64_t offset);

private fs_blocks_manager_t* p_blocks_manager_instance = NULL;

private void     fs_blocks_mangager_get_metadata(int32_t super_block_file);
private void     fs_blocks_manager_init_bitmap(int32_t super_block_file);
private void     fs_blocks_manager_init_disk(void);
private void     fs_blocks_manager_init_blocks(void);
private uint32_t fs_blocks_manager_get_free_block_index(void);
private void     fs_blocks_manager_release_block_at(uint32_t index);

private bool fs_blocks_manager_check_for_clean_initialization(void);

void fs_blocks_manager_init(void)
{
    if(p_blocks_manager_instance != NULL)
        return;

    p_blocks_manager_instance = u_malloc(sizeof(fs_blocks_manager_t));

    pthread_mutex_init(&p_blocks_manager_instance->blocks_bitmap_mx, NULL);
    pthread_mutex_init(&p_blocks_manager_instance->blocks_mx, NULL);

    const char* mount_point     = u_config_get_string_value("PUNTO_MONTAJE");
    char* super_block_file_path = string_from_format("%s/SuperBloque.ims", mount_point);

    int32_t super_block_file = open(super_block_file_path, O_RDWR, 0666);
    U_ASSERT(super_block_file != -1, "No se pudo abrir el archivo de SuperBloque.ims: %s", strerror(errno));

    u_free(super_block_file_path);

    fs_blocks_mangager_get_metadata(super_block_file);
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


// =======================================================
//              *** Private Functions ***
// =======================================================

private void fs_block_init(fs_block_t* this, uint32_t number, uint32_t disk_offset, uint32_t block_size)
{
    this->number      = number;
    this->disk_offset = disk_offset;
    this->block_size  = block_size;
}

private uint64_t fs_block_write_internal(const fs_block_t* this, const void* data, uint64_t data_size, uint64_t offset)
{
    uint64_t bytes_to_write =
        (data_size + offset < this->block_size) ?
            data_size :
            data_size - (data_size + offset - this->block_size);

    fs_physical_disk_write(data, bytes_to_write, this->disk_offset + offset);

    return bytes_to_write;
}

private uint64_t fs_block_read_internal(const fs_block_t* this, void* data, uint64_t data_size, uint64_t offset)
{
    uint64_t bytes_to_read =
        (data_size + offset < this->block_size) ?
            data_size :
            data_size - (data_size + offset - this->block_size);

    fs_physical_disk_read(data, bytes_to_read, this->disk_offset + offset);

    return bytes_to_read;
}

private void fs_blocks_mangager_get_metadata(int32_t super_block_file)
{
    U_ASSERT(pread(super_block_file, &p_blocks_manager_instance->blocks_size, sizeof(uint32_t), 0) != -1,
        "No se pudo obtener el tamaño de los bloques del SuperBloque.ims: %s", strerror(errno));

    U_ASSERT(pread(super_block_file, &p_blocks_manager_instance->blocks_count, sizeof(uint32_t), sizeof(uint32_t)) != -1,
        "No se pudo obtener la cantidad de los bloques del SuperBloque.ims: %s", strerror(errno));

    bool is_clean_initialization = fs_blocks_manager_check_for_clean_initialization();

    if(is_clean_initialization)
        fs_blocks_manager_init_bitmap(super_block_file);
    
    p_blocks_manager_instance->blocks_bitmap_size = ceil(p_blocks_manager_instance->blocks_count / 8);
    p_blocks_manager_instance->blocks_bitmap = mmap(
        NULL,
        p_blocks_manager_instance->blocks_bitmap_size,
        PROT_READ | PROT_WRITE, MAP_SHARED, super_block_file,
        0);

    U_ASSERT(p_blocks_manager_instance->blocks_bitmap != MAP_FAILED,
        "No se pudo mapear el bitmap del SuperBloque.ims: %s", strerror(errno));
}

private void fs_blocks_manager_init_bitmap(int32_t super_block_file)
{
    uint32_t bitmap_mem_size = ceil(p_blocks_manager_instance->blocks_count / 8);
    void* bitmap_mem = u_malloc(bitmap_mem_size);

    memset(bitmap_mem, 0, bitmap_mem_size);

    U_ASSERT(pwrite(super_block_file, bitmap_mem, bitmap_mem_size, sizeof(uint32_t) * 2) != -1,
        "No se pudo inicializar el Bitmap: %s", strerror(errno));

    u_free(bitmap_mem);
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
            i * p_blocks_manager_instance->blocks_size,
            p_blocks_manager_instance->blocks_size);
}

private uint32_t fs_blocks_manager_get_free_block_index(void)
{
    uint32_t index = 0;

    pthread_mutex_lock(&p_blocks_manager_instance->blocks_bitmap_mx);

    for(uint32_t i = 0; i < p_blocks_manager_instance->blocks_count; i ++)
    {
        uint8_t byte_in_bitmap  = i / 8;
        uint8_t neg_bitmap_byte = ~BLOCKS_BITMAP_BYTE(p_blocks_manager_instance->blocks_bitmap, byte_in_bitmap);

        if(neg_bitmap_byte)
        {
            index = (uint8_t)log2(~(neg_bitmap_byte | -neg_bitmap_byte) + 1) + 1;
            BLOCKS_BITMAP_BYTE(p_blocks_manager_instance->blocks_bitmap, byte_in_bitmap) |= (uint8_t)(1 << (index - 1));

            index += byte_in_bitmap * 8;
            
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
    
    uint32_t byte_in_bitmap = index / 8;
    uint8_t bit_selected_to_release = 1 << ((index / 8) - 1);
    BLOCKS_BITMAP_BYTE(p_blocks_manager_instance->blocks_bitmap, byte_in_bitmap) &= 0XFF^bit_selected_to_release; 

    pthread_mutex_unlock(&p_blocks_manager_instance->blocks_bitmap_mx);
}

private bool fs_blocks_manager_check_for_clean_initialization(void)
{
    char* disk_file_name = string_from_format("%s/Blocks.ims", u_config_get_string_value("PUNTO_MONTAJE"));
    bool disk_exist = access(disk_file_name, F_OK) == 0;
    u_free(disk_file_name);

    return disk_exist;
}

#ifdef UTESTS
const fs_block_t* fs_block_manager_get_block_instance(uint32_t block_indez)
{
    return p_blocks_manager_instance->blocks[block_index];
}
#endif