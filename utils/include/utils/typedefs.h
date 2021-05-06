#ifndef UTILS_TYPEDEFS_H
#define UTILS_TYPEDEFS_H

/**
 * @File: typedefs.h
 * @Desc: Header con varias definiciones de tipos comunes (como int32_t, bools, etc).
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <commons/string.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/collections/dictionary.h>

typedef struct
{
	uint32_t x;
	uint32_t y;
} u_pos_t;

#define private static

#define UTILS_PRINT_FORMAT(...)  __attribute__((format(printf, __VA_ARGS__)))

#endif
