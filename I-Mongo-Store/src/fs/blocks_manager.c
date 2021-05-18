#include "blocks_manager.h"
#include "disk.h"

#include <pthread.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/fcntl.h>
#include <math.h>

#define BLOCKS_SIZE(super_block_mem)   *((uint32_t*)(super_block_mem))
#define BLOCKS_COUNT(super_block_mem)  *((uint32_t*)(super_block_mem + sizeof(uint32_t)))
#define BLOCKS_BITMAP(super_block_mem) *((uint32_t*)(super_block_mem + sizeof(uint32_t) * 2))

typedef struct
{
    fs_block_t*     blocks;
    pthread_mutex_t blocks_bitmap_mx;
    void*           super_block_metadata;
} fs_blocks_manager_t;

// Definicion de la interfaz privada del fs_block_t
struct fs_block_t
{
    uint32_t number;
    uint32_t disk_offset;
    uint32_t block_size;
};

extern void fs_block_init(fs_block_t* block, uint32_t number, uint32_t disk_offset, uint32_t block_size);
extern void fs_block_delete(fs_block_t* block);

private fs_blocks_manager_t* p_blocks_manager_instance = NULL;

private void     fs_blocks_mangager_get_metadata(void);
private void     fs_blocks_manager_init_disk(void);
private void     fs_blocks_manager_init_blocks(void);
private uint32_t fs_blocks_manager_get_free_block_index(void);
private void     fs_blocks_manager_release_block_at(uint32_t index);

void fs_blocks_manager_init(void)
{
    if(p_blocks_manager_instance != NULL)
        return;

    p_blocks_manager_instance = u_malloc(sizeof(fs_blocks_manager_t));
    pthread_mutex_init(&p_blocks_manager_instance->blocks_bitmap_mx, NULL);

    fs_blocks_mangager_get_metadata();
    fs_blocks_manager_init_disk();
    fs_blocks_manager_init_blocks();
}

const fs_block_t* fs_blocks_manager_request_block(void)
{
    uint32_t free_block_index = fs_blocks_manager_get_free_block_index();

    if(free_block_index == 0)
        return NULL;

    return &p_blocks_manager_instance->blocks[free_block_index - 1];
}

void fs_blocks_manager_release_block(const fs_block_t* block)
{
    fs_blocks_manager_release_block_at(block->number);
}

// =======================================================
//              *** Private Functions ***
// =======================================================

private void fs_blocks_mangager_get_metadata(void)
{
    const char* mount_point = u_config_get_string_value("PUNTO_MONTAJE");
    char* super_block_file_path = string_from_format("%s/SuperBloque.ims", mount_point);

    int32_t super_block_file = open(super_block_file_path, O_RDWR, 0666);
    U_ASSERT(super_block_file != -1,
        "No se pudo abrir el archivo de SuperBloque en el direcotrio: %s", super_block_file_path);

    u_free(super_block_file_path);

    p_blocks_manager_instance->super_block_metadata =
        mmap(NULL, sizeof(uint32_t) * 3, PROT_READ | PROT_WRITE, MAP_SHARED, super_block_file, 0);
    
    U_ASSERT(p_blocks_manager_instance->super_block_metadata != MAP_FAILED,
        "No se pudo mapear el archivo SuperBlock.ims");
}

private void fs_blocks_manager_init_disk(void)
{
    fs_physical_disk_init(
        BLOCKS_SIZE(p_blocks_manager_instance->super_block_metadata) *
        BLOCKS_COUNT(p_blocks_manager_instance->super_block_metadata));
}

private void fs_blocks_manager_init_blocks(void)
{
    uint32_t blocks_count = BLOCKS_COUNT(p_blocks_manager_instance->super_block_metadata);
    uint32_t block_size   = BLOCKS_SIZE(p_blocks_manager_instance->super_block_metadata);

    p_blocks_manager_instance->blocks = u_malloc(blocks_count * sizeof(fs_block_t));

    for(uint32_t i = 0; i < blocks_count; i ++)
        fs_block_init(&p_blocks_manager_instance->blocks[i], i + 1, i * block_size, block_size);
}

private uint32_t fs_blocks_manager_get_free_block_index(void)
{
    uint32_t index;
    pthread_mutex_lock(&p_blocks_manager_instance->blocks_bitmap_mx);
    uint32_t neg_bitmap = ~BLOCKS_BITMAP(p_blocks_manager_instance->super_block_metadata);

    if(!neg_bitmap)
        return 0;

    index = (uint32_t)log2(~(neg_bitmap | -neg_bitmap) + 1) + 1;
    uint32_t bit_mask_selection = 1 << (index - 1);
    BLOCKS_BITMAP(p_blocks_manager_instance->super_block_metadata) |= bit_mask_selection;

    pthread_mutex_unlock(&p_blocks_manager_instance->blocks_bitmap_mx);

    return index;
}

private void fs_blocks_manager_release_block_at(uint32_t index)
{
    pthread_mutex_lock(&p_blocks_manager_instance->blocks_bitmap_mx);

    uint32_t bit_mask_selection = 1 << (index - 1);
    BLOCKS_BITMAP(p_blocks_manager_instance->super_block_metadata) &= (0XFFFFFFFF^bit_mask_selection);

    pthread_mutex_unlock(&p_blocks_manager_instance->blocks_bitmap_mx);
}
