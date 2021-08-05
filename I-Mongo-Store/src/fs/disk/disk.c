#include "disk.h"

#include <pthread.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/fcntl.h>

#include <errno.h>

#include <string.h>

private pthread_mutex_t p_physical_disk_mx   = PTHREAD_MUTEX_INITIALIZER;
private void*           p_physical_disk      = NULL;
private uint64_t        p_physical_disk_size = 0;

void fs_physical_disk_init(uint64_t size, bool is_clean_initialization)
{
    if(p_physical_disk != NULL)
        return;

    char*   disk_file_dir = string_from_format("%sBlocks.ims", u_config_get_string_value("PUNTO_MONTAJE"));
    int32_t disk_file_fd  = open(disk_file_dir, O_RDWR, 0666);

    U_ASSERT(disk_file_fd != -1,
        "No se pudo inicializar el Disco Fisico del FileSystem en la ruta %s. %s",  disk_file_dir, strerror(errno));
        
    u_free(disk_file_dir);

    if(is_clean_initialization)
    {
        void* zero_memory = u_malloc(size);
        memset(zero_memory, 0, size);
        write(disk_file_fd, zero_memory, size);
        u_free(zero_memory);
    }

    p_physical_disk      = mmap(NULL, size, PROT_WRITE | PROT_READ, MAP_SHARED, disk_file_fd, 0);
    p_physical_disk_size = size;

    U_LOG_TRACE("Disco Inicializado. Tamaño: %ld", size);
}

void fs_physical_disk_write(const void* data, uint64_t size, uint64_t offset)
{
    U_ASSERT(
        offset + size < p_physical_disk_size,
        "Trying to write in OutOfBound memory disk. Offset: %ld | Size: %ld", offset, size);

    U_LOG_TRACE("Se escriben %ld bytes en el disco. Offset %ld", size, offset);

    pthread_mutex_lock(&p_physical_disk_mx);
    memcpy(p_physical_disk + offset, data, size);
    pthread_mutex_unlock(&p_physical_disk_mx);
}

void  fs_physical_disk_read(void* data, uint64_t size, uint64_t offset)
{
    U_ASSERT(
        offset + size < p_physical_disk_size,
        "Trying to read in OutOfBound memory disk. Offset: %ld | Size: %ld", offset, size);

    U_LOG_TRACE("Se leen %ld bytes en el disco. Offset %ld", size, offset);

    pthread_mutex_lock(&p_physical_disk_mx);
    memcpy(data, p_physical_disk + offset, size);
    pthread_mutex_unlock(&p_physical_disk_mx);
}

void  fs_physical_disk_flush(uint64_t offset, uint64_t size)
{
    U_ASSERT(
        offset + size < p_physical_disk_size,
        "Trying to flush in OutOfBound memory disk. Offset: %ld | Size: %ld", offset, size);

    pthread_mutex_lock(&p_physical_disk_mx);
    msync(p_physical_disk + offset, size, MS_SYNC);
    pthread_mutex_unlock(&p_physical_disk_mx);
}

uint32_t fs_physical_disk_get_size(void)
{
    char* disk_file_dir = string_from_format("%s/Blocks.ims", u_config_get_string_value("PUNTO_MONTAJE"));
    int disk_file_fd = open(disk_file_dir, O_RDONLY);
    U_ASSERT(disk_file_fd != -1,
        "No se pudo inicializar el Disco Fisico del FileSystem en la ruta %s. %s",  disk_file_dir, strerror(errno));
    
    u_free(disk_file_dir);
    uint32_t size_file = lseek(disk_file_fd, 0, SEEK_END);
    close(disk_file_fd);

    return size_file;
}