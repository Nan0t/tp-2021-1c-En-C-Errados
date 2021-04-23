# Code Style
## Snake_Case:
* Las funciones, los parametros de funciones y las variables deben estar escritas en `snake_case`.
* El nombre de las estructuras y cualquier otro tipo definidio debe ser escrita en `snake_case` y terminar con el prefijo `_t`.
* Los tipos enumerados deben tener el prefijo **_e** en vez de `_t`.
* Los valores de los enumerados, las directivas de preprocesador y las constantes con ámbito global o de archivo deben estar escritos en mayúsculas y en snake_case.

Ejemplo:

`Ejemplo macros de preprocesador y constantes globales`

```c
.
.
.
#define INVALID_SOCKET -1
#define DEFAULT_IP "localhost"
#define DEFAULT_PORT "3000"

const char* ERROR_STR[] =
{
    "CONN_NO_ERROR",
    "CONN_INVALID_SOCKET",
    "CONN_SEND_ERROR",
    .
    .
    .
};
```

`Ejemplos de enumerados`

```c
typedef enum { CONN_SUCCESS, CONN_ERROR } conn_status_e;

conn_status_e conn_create(...)
{
.
.
.
```

`Ejemplo de variables, structs y typedefs`

```c
typedef struct
{
    int sock;

    const char* host_ip;
    const char* host_port;
} conn_t;

conn_t* conn_create(const char* host_ip, const char* host_port)
{
    conn_t* this = malloc(sizeof(conn_t));

    this->sock = conn_connect(host_ip, host_port);

    if(this->sock == INVALID_SOCKET)
    {
        free(this);
        return NULL;
    }

    this->host_ip = strdup(host_ip);
    this->host_port = strdup(host_port);

    return this;
}
```