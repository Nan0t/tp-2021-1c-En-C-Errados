#ifndef UTILS_TYPEDEFS_H
#define UTILS_TYPEDEFS_H

/**
 * @File: typedefs.h
 * @Desc: Header con varias definiciones de tipos comunes (como int32_t, bools, etc).
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define private static

#define UTILS_PRINT_FORMAT(...)  __attribute__((format(printf, __VA_ARGS__)))

#endif