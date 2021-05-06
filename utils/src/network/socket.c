#include "utils/network/socket.h"
#include "utils/diagnostics/log.h"
#include "utils/diagnostics/assert.h"

#include <commons/string.h>

#include <sys/socket.h>
#include <netdb.h>

#include <errno.h>

extern void u_sock_err_write_error(u_sock_err_t* err, u_sock_err_type_e err_type, const char* format_msg, ...);
extern void u_sock_err_reset(u_sock_err_t* err);

int32_t u_socket_connect(const char* host, const char* port, u_sock_err_t* err)
{
    struct addrinfo* server_info;
    struct addrinfo* it;
    struct addrinfo  hints = { 0 };

    int32_t fd;
    int32_t getaddr_err;

    u_sock_err_reset(err);

    hints.ai_family   = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    getaddr_err = getaddrinfo(host, port, &hints, &server_info);
    
    if(getaddr_err != 0)
    {
        if(err)
            u_sock_err_write_error(
                err, U_SOCK_GET_ADDR_ERROR, "getaddrinfo error: %s", gai_strerror(getaddr_err));

        return -1;
    }

    for(it = server_info; it != NULL; it = it->ai_next)
    {
        fd = socket(it->ai_family, it->ai_socktype, it->ai_protocol);
        if(fd == -1)
            continue;

        if(connect(fd, it->ai_addr, it->ai_addrlen) == -1)
            continue;

        break;
    }

    freeaddrinfo(server_info);

    if(it == NULL)
    {
        if(err)
            u_sock_err_write_error(
                err, U_SOCK_CONNECT_ERROR, "connect error: %s", strerror(errno));

        return -1;
    }

    return fd;
}

int32_t u_socket_listen(const char* port, int32_t backlogs, u_sock_err_t* err)
{
    struct addrinfo* server_info;
    struct addrinfo* it;
    struct addrinfo  hints = { 0 };

    int32_t fd;
    int32_t getaddr_err;

    const int32_t yes = 1;

    u_sock_err_reset(err);

    hints.ai_family   = AF_INET;
    hints.ai_flags    = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;

    getaddr_err = getaddrinfo(NULL, port, &hints, &server_info);
    
    if(getaddr_err != 0)
    {
        if(err)
            u_sock_err_write_error(
                err, U_SOCK_GET_ADDR_ERROR, "getaddrinfo error: %s", gai_strerror(getaddr_err));

        return -1;
    }

    for(it = server_info; it != NULL; it = it->ai_next)
    {
        fd = socket(it->ai_family, it->ai_socktype, it->ai_protocol);
        if(fd == -1)
            continue;

        setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &yes, sizeof(int32_t));

        if(bind(fd, it->ai_addr, it->ai_addrlen) == -1)
            continue;

        break;
    }

    freeaddrinfo(server_info);

    if(it == NULL)
    {
        if(err)
            u_sock_err_write_error(
                err, U_SOCK_BIND_ERROR, "bind error: %s", strerror(errno));

        return -1;
    }

    if(listen(fd, backlogs) == -1)
    {
        if(err)
            u_sock_err_write_error(
                err, U_SOCK_LISTEN_ERROR, "listen error: %s", strerror(errno));

        return -1;
    }

    return fd;
}

int32_t u_socket_accept(int32_t sock, u_sock_err_t* err)
{
    int32_t new_conn = accept(sock, NULL, NULL);

    u_sock_err_reset(err);

    if(new_conn == -1)
        if(err)
            u_sock_err_write_error(err, U_SOCK_ACCEPT_ERROR, "accept error: %s", strerror(errno));

    return new_conn;
}

void u_socket_close(int32_t sock)
{
    U_ASSERT(sock != -1, "Trying to close an invalid socket");
    shutdown(sock, SHUT_RDWR);
}

bool u_socket_send(int32_t sock, const void* data, int64_t size)
{
    int64_t bytes_to_send = size;
    int64_t bytes_sended = 0;

    U_ASSERT(data, "data is NULL");

    do
    {
        bytes_sended = send(sock, data + bytes_sended, bytes_to_send, MSG_NOSIGNAL);

        if(bytes_sended == -1)
        {
            if(errno == EPIPE || errno == ENOTCONN)
                return false;

            U_ASSERT(false, "send error: %s", strerror(errno));
            return false;
        }

        bytes_to_send -= bytes_sended;
    } while(bytes_to_send > 0);

    return true;
}

bool u_socket_recv(int32_t sock, void* data, int64_t size)
{
    int64_t bytes;
    U_ASSERT(data, "data is NULL");

    bytes = recv(sock, data, size, 0);

    if(bytes <= 0)
    {
        if(bytes == 0)
            return false;
        U_ASSERT(false, "recv error: %s", strerror(errno));
        return false;
    }

    return true;
}