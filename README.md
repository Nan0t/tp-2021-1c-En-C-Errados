# TP S.O 2021: I-MongOS
Trabajo práctico de Sistemas Operativos, asignatura dada en la UTN FRBA. El TP no esta exentos de errores que podrían haber quedado consecuencia del último tramo de la cursada, donde el tiempo apremia... y las cosas las atamos con alambre.

## Motivación del repo y la documentación
La principal motivación de hacer esta doc es para _orientar_ aquellas personas que estén cursando la asignatura a como se vería un TP _aprobable_, ademas de mostrar la resolución de uno de los tantos TPs que hay dando vuelta. La idea es que puedan llevarse conceptos de cómo (y cuanto) de la teoría se puede trasladar al trabajo práctico, como así también ver cosas mas técnicas como sintaxis de C y el uso de diferentes APIs de Linux (sockets, pthreads, etc).

Dicho esto, la idea no es que el que llegue a este repo copie y pegue código que no entiende y que, muy seguramente, no le vaya a servir para su caso de uso. Como se dijo al principio, la idea es que el repo sirva como _orientación_ sobre como se puede llevar la teoría a código C.

## Disclaimer: Mi-RAM HQ
Si bien el TP fue aprobado, los módulos mas probados son el **Discordiador** y el **I-Mongos Store**. El módulo **Mi-RAM HQ** tuvo un esfuerzo grande por parte de dos de nuestros compañeros, pero poseen demasiadas prácticas _oscuras_, por lo que es dificil documentar algo que de por sí no es muy recomendable hacer. Ademas de que existe un error (no demasiado grabe, pero que de haberse notado en nuestra entrega nos podría haber costado la cursada) el cual consiste en que el área de swaping no esta mapeado a un archivo en disco.
No esta en los planes hacer un refactory del módulo, aunque si se llega a dar se documentará que es lo que se hizo.

## Contenido
* [Módulo Discordiador](./doc/Discordiador.md)
* Módulo I-Mongo Store (proximamente).
* Módulo Mi-RAM HQ (Posiblemente nunca).