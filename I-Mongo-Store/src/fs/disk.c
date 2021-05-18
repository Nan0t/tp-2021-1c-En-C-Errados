#include "fs/disk.h"

#include <pthread.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/fcntl.h>

#include <errno.h>

#include <string.h>

private void*           p_physical_disk      = NULL;
private uint64_t        p_physical_disk_size = 0;

void fs_physical_disk_init(uint64_t size)
{
    if(p_physical_disk != NULL)
        return;

    char*   disk_file_dir = string_from_format("%s/Blocks.ims", u_config_get_string_value("PUNTO_MONTAJE"));
    int32_t disk_file_fd  = open(disk_file_dir, O_RDWR, 0666);

    U_ASSERT(disk_file_fd != -1, "No se pudo inicializar el Disco Fisico del FileSystem. %s", strerror(errno));

    void* zero_memory = u_malloc(size);
    memset(zero_memory, 0, size);
    write(disk_file_fd, zero_memory, size);
    u_free(zero_memory);

    p_physical_disk      = mmap(NULL, size, PROT_WRITE | PROT_READ, MAP_SHARED, disk_file_fd, 0);
    p_physical_disk_size = size;

    U_LOG_TRACE("Disco Inicializado. Tamaño: %d", size);
}

void fs_physical_disk_write(const void* data, uint64_t size, uint64_t offset)
{
    U_ASSERT(
        offset + size < p_physical_disk_size,
        "Trying to write in OutOfBound memory disk. Offset: %ld | Size: %ld", offset, size);

    U_LOG_TRACE("Se escriben %ld bytes en el disco. Offset %ld", size, offset);

    memcpy(p_physical_disk + offset, data, size);
}

void  fs_physical_disk_read(void* data, uint64_t size, uint64_t offset)
{
    U_ASSERT(
        offset + size < p_physical_disk_size,
        "Trying to read in OutOfBound memory disk. Offset: %ld | Size: %ld", offset, size);

    U_LOG_TRACE("Se leen %ld bytes en el disco. Offset %ld", size, offset);

    memcpy(data, p_physical_disk + offset, size);
}

void  fs_physical_disk_flush(uint64_t offset, uint64_t size)
{
    U_ASSERT(
        offset + size < p_physical_disk_size,
        "Trying to flush in OutOfBound memory disk. Offset: %ld | Size: %ld", offset, size);

    msync(p_physical_disk_size + offset, size, MS_SYNC);
}
