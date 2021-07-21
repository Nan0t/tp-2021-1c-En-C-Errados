#ifndef FS_FILE_H
#define FS_FILE_H

#include <utils/utils.h>

typedef struct fs_file_t fs_file_t;

/**
 * @NAME: fs_file_create
 * @DESC: crea un instancia de file.
 * @PARAMS:
 *  [in] const char* file_path   - ruta del archivo 
 *  [in] char        fill_char   - caracter de llenado.
 * 
 * @RETURNS: una instancia fs_file_t.
 */
fs_file_t* fs_file_create(const char* file_path, char* fill_char);

/**
 * @NAME: fs_file_delete
 * @DESC: eliminar una instancia de file.
 * @PARAMS:
 *  [in] fs_file_t* file - instancia de file que se quiere eliminar.
 */
void fs_file_delete(fs_file_t* file);

/**
 * @NAME: fs_file_add_fill_char
 * @DESC: escribe una cantidad especifica de carateres de relleno dentro del archivo.
 * @PARAMS:
 *  [in] fs_file_t* file   - file donde se quiere escribir.
 *  [in] uint32_t   amount - cantidad de carateres a escribir.
 */
void fs_file_add_fill_char(fs_file_t* file, uint32_t amount);

/**
 * @NAME: fs_file_remove_fill_char
 * @DESC: eliminar una cantidad despecifica de carateres de relleno dentro del archivo.
 * @PARAMS:
 *  [in] fs_file_t* file   - file del cual se quiere eliminar carateres.
 *  [in] uint32_t   amount - cantidad de carateres a eliminar.
 */
void fs_file_remove_fill_char(fs_file_t* file, uint32_t amount);

/**
 * @NAME: fs_file_check_integrity
 * @DESC: chequea la integridad de un file ante un sabotaje.
 * @PARAMS:
 *  [in] fs_file_t* file - archivo del cual se quiere comprobar su integridad.
 * 
 * @RETURNS: true en caso de que el archivo no este corrompido. En caso contrario false.
 */
bool fs_file_check_integrity(fs_file_t* file);

/**
 * @NAME: fs_file_get_name
 * @DESC: devuelve el nombre del archivo.
 * @PARAMS:
 *  [in] const fs_file_t* file - file del cual se quiere conocer su nombre.
 * 
 * @RETURNS: el nombre del archivo.
 */
const char* fs_file_get_name(const fs_file_t* file);

/**
 * @NAME: fs_file_get_size
 * @DESC: devuelve el tamaño del archivo.
 * @PARAMS:
 *  [in] const fs_file_t* file - file del cual se quiere obtener su tamaño.
 * 
 * @RETURNS: el tamaño del archivo.
 */
uint32_t fs_file_get_size(const fs_file_t* file);

/**
 * @NAME: fs_file_get_fill_char
 * @DESC: devuelve el caracter de llenado del archivo.
 * @PARAMS:
 *  [in] const fs_file_t* file - file del cual se quiere obtener su caracter de llenado.
 * 
 * @RETURNS: el caracter de llenado del archivo.
 */
char* fs_file_get_fill_char(const fs_file_t* file);

/**
 * @NAME: fs_file_get_md5
 * @DESC: devuelve el md5 del archivo.
 * @PARAMS:
 *  [in] const fs_file_t* file - file del cual se quiere obtener su md5.
 * 
 * @RETURNS: el md5 del archivo.
 */
const char* fs_file_get_md5(const fs_file_t* file);

/**
 * @NAME: fs_file_get_blocks_count
 * @DESC: devuelve la cantidad de bloques utilizados por un archivo.
 * @PARAMS: 
 *  [in] const fs_file_t* file - file del cual se quiere obtener la cantidad de bloques que utiliza.
 * 
 * @RETURNS: la cantidad de bloques utilizadas por el archivo.
 */
uint32_t fs_file_get_blocks_count(const fs_file_t* file);

#endif