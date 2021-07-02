#include "file.h"
#include "block.h"
#include "fs/blocks/blocks_manager.h"

#include <pthread.h>

struct fs_file_t{
	char*		NOMBRE_ARCHIVO;
    t_config* 	CONFIG;
};


fs_file_t* fs_file_create(const char* mount_point, const char* file_name, char fill_char){

	char * path = u_malloc(sizeof(char*));
	path = string_from_format("%s/Files/%s",mount_point,file_name);

	FILE* file = fopen(path, "w");

		fs_file_t* this = u_malloc(sizeof(fs_file_t));

		this->CONFIG = config_create(path);

		config_set_value(this->CONFIG, "SIZE", "0");

		uint32_t block_id = fs_blocks_manager_request_block();
		char* block_list = string_from_format("[%d]", block_id);

		config_set_value(this->CONFIG, "BLOCKS", block_list);

		config_set_value(this->CONFIG, "BLOCK_COUNT", "1");

		config_set_value(this->CONFIG, "CARACTER_LLENADO", fill_char);

		config_set_value(this->CONFIG, "MD5_ARCHIVO", NULL); //TODO asociado a integridad

	fclose(file);

	u_free(path);

    return this;
}

//Elimina la estructura de la bitacora y libera los bloques que tenga asignados.
void fs_file_delete(fs_file_t* this){

	char** block_list = config_get_array_value(this->CONFIG, "BLOCKS");
	for(int i=0; i<config_get_int_value(this->CONFIG, "BLOCKS_COUNT") ; i++){
		fs_blocks_manager_release_block(block_list[i]);
	}

    u_free(block_list);
    u_free(this);

}

void fs_file_add_fill_char(fs_file_t* this, uint32_t amount){

    //TODO: Agregar la cantidad especificada por "amount" de caracteres de llenado en el file.
    // En caso de no poder escribirse todo el contenido, pedir un nuevo bloque y escribir
    // la cantidad de caracteres restantes. En caso de que tampoco se pueda escribir
    // todo el contenido en el nuevo bloque, seguir pidiendo bloqueas hasta terminar de escribir
    // la cantidad total de caracteres.

	int amount_values = config_get_int_value(this->CONFIG, "BLOCK_COUNT");
	char** values = config_get_array_value(this->CONFIG, "BLOCKS");

	uint32_t block_id =  atoi(values[amount_values-1]);

	char* fill_char = config_get_string_value(this->CONFIG, "CARACTER_LLENADO");

	char* fill = string_repeat(*fill_char, amount);

	//obtengo el offset del ultimo bloque de la lista TODO
	int offset = 0;

	int escritos = u_malloc(sizeof(int));
	escritos = fs_block_write(block_id, fill, sizeof(fill), offset);

	if(escritos!=sizeof(fill)){

		//pido un nuevo bloque
		list_add(this->CONFIG,fs_blocks_manager_request_block());
		block_id++;

		escritos = fs_block_write(block_id, fill, sizeof(fill), 0);

		while(escritos!=sizeof(fill)){
			escritos += fs_block_write(block_id, fill, sizeof(fill)-escritos, offset); //aca deberia solo tomar el content menos el ya guardado
		}
	}
	u_free(fill);
	u_free(escritos);
}

void fs_file_remove_fill_char(fs_file_t* this, uint32_t amount){
    //TODO: Eliminar la cantidad especificada por "amount" de caracteres de llenado en el file.
    // Esto se puede hacer simplemente reduciendo el tamaño del archivo. Luego, se deberia dividir
    // el tamaño actual del archivo por el tamaño de un bloque (que puede ser consultado mediante la
    // funcion "fs_blocks_manager_get_blocks_size"). El resultado sera la cantidad de bloques que el
    // archivo estaría usando. Si es menor a la cantidad de bloques que posee actualmente, liberar los
    // ultimos bloques del archivo. Por ejemplo, si despues de reducir el tamaño del archivo, la cantidad
    // de bloques que ocupa el archivo son 4 y el archivo tiene ocupados 6 bloques, se deben liberar,
    // los últimos 2 bloques (llamando a la función "fs_blocks_manager_release_block(num_bloque)")
}

bool fs_file_check_integrity(fs_file_t* this){
	//con sabotajes (MD5)
    //TODO: Se debe devolver true o false dependiendo si el archivo fue corrompido o no.
    // El criterio que se va a seguir para saber si un file fue corrompido o no, se
    // especifica en el enunciado del TP.
    return false;
}

const char* fs_file_get_name(const fs_file_t* this){
    return this->NOMBRE_ARCHIVO;
}

const char* fs_file_get_md5(const fs_file_t* this){
    return config_get_string_value(this, "MD5_ARCHIVO");
}

uint32_t fs_file_get_size(const fs_file_t* this){
    return config_get_int_value(this, "SIZE");
}

char fs_file_get_fill_char(const fs_file_t* this){
	return config_get_string_value(this, "CARACTER_LLENADO");
}

uint32_t fs_file_get_blocks_count(const fs_file_t* this){
    return config_get_int_value(this, "BLOCK_COUNT");
}
