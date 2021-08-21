# Módulo Discordiador

El módulo Discordiador (al que nosotros apodamos a principio de cuatrimestre como _Discordia_) es el módulo planificador del sistema. La misma se encuentra dividida en:

- El Discordiador
- La Consola.
- Submódulo Planificador.
- Los diferentes hilos de cada tripulante.
- El Servidor.

![ArqDiscordiador](./img/ArqDiscordiador.png)

El **Discordiador** sirve como interfaz para comunicar a los distintos submódulos con la memoria y el file system (algo así como el SO). Es el encargado de mantener las conexiones de los distintos tripulantes y la consola.

La **Consola** (como ya sabrán) es el encargado de recibir los input del usuario y mostrar los resultados de los comandos por consola (si es que se debe mostrar algo).

El **Planificador** es el submódulo encargado de ejecutar los ciclos de ejecución de las CPUs y el hilo de I/O, como así también seleccionar a los tripulantes a ejecutar y controlar el estado de la planifiación (en pausa, ejecución, sabotaje).

Los **Hilos de Tripulante** son los tripulantes en sí, los cuales ejecutan tareas y realizan movimientos dentro de la nave.

Por último, el **Servidor** tiene como único objetivo recibir las distintas notificaciones de sabotaje.

## Planificador

La gran complejidad del módulo se encuentra en el **Planificador**. Este debe cumplir que:

- Cada hilo de tripulante debe ser creado _a demanda_ (es decir, deben haber al menos tantos hilos de tripulantes como tripulantes se ingresen en el sistema).
- Se debe poder pausar la planificación en cualquier momento.
- Se debe ejecutar una rutína de sabotaje (especificada en el enunciado), la cual altera la planificación de todos los tripulantes.

Con todo esto en mente, tratamos de hacer el módulo lo mas parecido a la teoría como fuera posible. Dicho esto, identificamos las siguientes entidades como parte del submódulo planificador:

- CPUs.
- Dispositivo I/O.
- Sincronizador.
- Colas del planificador.

Las interfaces de cada una se encuentran dentro de la carpeta _planificador_ en el discordiador.

La idea general es que el **Hilo Planificador** se encarge de planificar las estructuras de los tripulantes, aceptar nuevas patotas (siempre y cuando la planificación esté funcionando), mover los tripulantes entre colas, desbloquear los hilos de CPUs y buscar y eliminar a los tripulantes que se especifiquen en consola.

Los **Hilos de CPU** se encargan de obtener tripulantes de la cola de Exec y ejecutarlos. El ejecutarlos implica desbloquear el hilo del tripulante seleccionado y esperar a que este finalice su ciclo. Una vez hecho esto, la CPU notifica que el tripulante finalizo la ejecución y queda a la espera de que inicie el siguiente ciclo (el cual se iniciará una vez que todos las CPUs y el hilo de I/O finalicen, y siempre que la planificación no se encuentre pausada).

Los **Hilos de Tripulante** se encargan de ejecutar la lógica del tripulante. Es decir, realiza las acciones pertinentes a un tripulante (moverse, realizar una tarea (bloqueante o no)).

El siguiente diagrama muestra un gráfico reducido de la lógica detras de la arquitectura del planificador:

![ArqPlanificador](./img/ArqPlanificador.png)
_Nota: Las lineas puteadas representan algun mecanismo de sincronización (semaforos, condicionales, etc)._

Para ver una versión mas detallada del diagrama anterior (y capaz algo desorganizado), [este diagrama en lucidchart](https://lucid.app/lucidchart/invitations/accept/inv_bf9e5e5b-9775-494b-a634-5d3ebddb9672?viewport_loc=-78%2C328%2C4489%2C1984%2C0_0) se expande mas, incluyendo en él los hilos de los tripulantes y las condiciones para el sabotaje.

## Modelo de conexiones

Como se ve en el primer diagrama, todas las comunicaciones de los distintos submodulos del Discordiador pasan primero por el Discordiador. Esto significa que las conexiones son administradas por el Discordiador y no por los submodulos. Esto permite abstraer el modelo de conexiones de los distintos submódulos (y que fue de gran ayuda para el tramo final del TP).

Al comienzo, planteamos hacer conexiones "no vivas". Es decir, cada solicitud que se hacía a Mi-RAM HQ o al I-Mongo Store se hacia levantando un socket, enviando el mensaje y cerrando el socket una vez recibida la respuesta (en caso que aplicara).

Al principio no supuso ningun inconveniente, pero en la recta final, cuando empezamos a realizar pruebas mas integradores y bajo mas carga sobre los módulos, empezamos a tener problemas con las conexiones.

Especificamente, el error que nos encontramos es que los procesos llegaban al límite de file descriptors que podian mantener abierto (cosa que nos extrañó mucho ya que se supone que las conexiones se cerraban apenas terminaban las request). Intentamos aumentar el número de conexiones que podían recibir los servidores, pero solo logramos que el sistema terminara de crashear un rato después.

Finalmente decidimos cambiar el módelo de conexiones y realizar conexiones "vivas". Es decir, cuando se inicia un Tripulante, se crean las conexiones con Mi-RAM y el I-Mongo Store para ese tripulante y se mantienen hasta que el tripulante finaliza.

Por suerte, y gracias a que abstraimos la lógica de las conexiones dentro del Discordiador, el cambio fue bastante rápido. 

Los cambios fueron:

* En los servidores de los módulos, se cambio (literalmente) un if por un while:

_Antes_
```C
private void fs_client_handler_task(int32_t* client_sock)
{
    u_opcode_e opcode;
    
    if(u_socket_recv(*client_sock, &opcode, sizeof(uint32_t)))
        fs_client_handler_check_opcode(*client_sock, opcode);

    u_socket_close(*client_sock);
    u_free(client_sock);
}
```

_Despues_
```C
private void fs_client_handler_task(int32_t* client_sock)
{
    u_opcode_e opcode;
    
    while(u_socket_recv(*client_sock, &opcode, sizeof(uint32_t)))
        fs_client_handler_check_opcode(*client_sock, opcode);

    u_socket_close(*client_sock);
    u_free(client_sock);
}
```

* En el Discordiador, se creo un administrador de conexiones el cual mantenia los sockets tanto de la consola como de los distintos tripulantes. Cada par de sockets (uno para Mi-RAM HQ y otro para I-Mongo Store) correspondientes a un tripulante se los identifica con el TID del tripulante. Por lo que no hizo falta hacer grandes cambios dentro del Discordiador.

El cambio completo se hizo en [este commit](https://github.com/Nan0t/tp-2021-1c-En-C-Errados/commit/4579a287f2c6cf1f614e3220d6ca4aae5793504a).

Lo importante de todo esto (y que puede llegar a servir a los que estén planeando sus módelos de conexiones) es:

* En lo posible, usen conexiones vivas.
* Si quieren probar con otro modelo de conexiones, asegurencen de probarlo bajo mucho stress (y que los errores de conexión no salgan en las últimas 8 horas antes de la entrega).
* Abstraigan lo mas que puedan la lógica de las conexiones de la lógica de los módulos (de esta manera, si surge un imprevisto como el nuestro, no van a necesitar repensar todo el módulo).
