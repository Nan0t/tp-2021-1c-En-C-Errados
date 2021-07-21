#ifndef FS_FILE_FILES_MANAGER_H
#define FS_FILE_FILES_MANAGER_H

#include "file.h"

/**
 * @NAME: fs_files_manager_init
 * @DESC: inicia el administrador del archivos del sistema.
 * @PARAMS:
 *  [in] const char* mount_point - punto de montaje.
 */
void fs_files_manager_init(const char* mount_point, bool is_clean_initialization);

/**
 * @NAME: fs_files_manager_create_file
 * @DESC: crea un archivo dentro del sistema.
 * @PARAMS:
 *  [in] const char* name      - nombre del archivo, sin la extension.
 *  [in] char        fill_char - caracter de llenado del archivo.
 */
void fs_files_manager_create_file(const char* name, char fill_char);

/**
 * @NAME: fs_files_manager_hold_file
 * @DESC: devuelve una instancia fs_file_t con el nombre indicado por "name". La instancia debe
 *          ser liberada posteriormente llamando a fs_files_manager_release_file
 * @PARAMS:
 *  [in] const char* name - nombre del archivo que se quiere obtener.
 * 
 * @RETURNS: una instancia fs_file_t del archivo pedido, o NULL en caso de que no exista ningun archivo
 *             con el nombre especificado en el sistema.
 */
fs_file_t* fs_files_manager_hold_file(const char* name);

/**
 * @NAME: fs_files_manager_release_file
 * @DESC: libera una referencia a un file pedido anteriormente con fs_files_manager_hold_file.
 * @PARAMS:
 *  [in] const char* name - nombre del archivo que se quiere liberar.
 */
void fs_files_manager_release_file(const char* name);

/**
 * @NAME: fs_files_manager_delete_file
 * @DESC: elimina un archivo del sistema.
 * @PARAMS:
 *  [in] const char* name - nombre del archivo que se quiere eliminar del sistema.
 */
void fs_files_manager_delete_file(const char* name);

bool fs_files_manager_check_files_integrity(void);

#endif