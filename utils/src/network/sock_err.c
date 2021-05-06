#include "utils/network/sock_err.h"
#include "utils/memory/allocator.h"

struct u_sock_err_t
{
    u_sock_err_type_e err_type;
    char*             description;
};

u_sock_err_t* u_sock_err_create(void)
{
    struct u_sock_err_t* this = u_malloc(sizeof(struct u_sock_err_t));

    this->err_type    = U_SOCK_NO_ERROR;
    this->description = string_duplicate("No Errors");

    return this;
}

void u_sock_err_delete(u_sock_err_t* this)
{
    u_free(this->description);
    u_free(this);
}

u_sock_err_type_e u_sock_err_get_status(const u_sock_err_t* this)
{
    return this->err_type;
}

const char* u_sock_err_get_description(const u_sock_err_t* this)
{
    return this->description;
}

// ========================================================
//      *** Funciones para uso interno de socket.c
// ========================================================

void u_sock_err_write_error(u_sock_err_t* err, u_sock_err_type_e err_type, const char* format_msg, ...)
{
    va_list args;
    va_start(args, format_msg);

    u_free(err->description);
    err->description = string_from_vformat(format_msg, args);
    va_end(args);

    err->err_type = err_type;
}
