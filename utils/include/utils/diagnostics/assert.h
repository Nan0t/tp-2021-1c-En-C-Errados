#ifndef UTILS_DIAGNOSTICS_ASSERT_H
#define UTILS_DIAGNOSTICS_ASSERT_H

/**
 * @File: assert.h
 * 
 * @Desc: Header que contiene una funcion de control de condicion que
 * abortara el programa en caso de no cumplirse la condicion. Ejemplo
 * de uso:
 * 
 *  bool is_init = initialize_memory();
 * 
 *  U_ASSERT(is_init, "The memory module couldn't initialize");
 *  .
 *  .
 *  .
 */

#ifndef NDEBUG
    #include "log.h"

    /**
     * @Macro: U_ASSERT
     * 
     * @Desc: evalua una expresion. En caso de no cumplirse, aborta el programa.
     * 
     * @Params:
     *  bool        exp      --> exprecion a ser evaluada.
     *  const char* err_msg  --> mensaje de error que se logeara en caso de que
     *                           la expresion resulte ser falso. El mensaje puede
     *                           tener un formato igual al de printf.
     *  ...                  --> argumentos que correspondan al formato del message.
     */
    #define U_ASSERT(exp, ...)\
        if(!(exp)) { _u__assert_impl(__FILE__, __FUNCTION__, __LINE__, __VA_ARGS__); }


    /**
     * @Func: _u__assert_impl
     * 
     * @Desc: funcion que logea un mensaje de error y aborta el programa.
     * 
     * @Params:
     *  const char*  file      --> archivo en el que ocurrio el error.
     *  const char*  function  --> funcion en la que ocurrio el error.
     *  int          line      --> linea en la que oucrrio el error.
     *  const char*  msg       --> mensaje que se logeara. Puede tener
     *                             un format igual al de printf.
     *  ...                    --> argumentos que correspondan al formato del message.
     * 
     */
    void _u__assert_impl(const char* file, const char* function, int line, const char* msg, ...);
#else
    #define U_ASSERT(exp, ...) (exp)
#endif

#endif