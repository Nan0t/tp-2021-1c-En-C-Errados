#include "file.h"
#include "fs/blocks/blocks_manager.h"
#include "commons/config.h"

#include <pthread.h>
#include <openssl/md5.h>
#include <stdlib.h>
#include <stdio.h>
#define min(a,b) \
({ __typeof__ (a) _a = (a); \
	__typeof__ (b) _b = (b); \
	_a < _b ? _a : _b; })

struct fs_file_t{
	char*		NOMBRE_ARCHIVO;
    t_config* 	CONFIG;
};
private t_list* lista_id_bloques_archivo(char** lista_bloques);
private char* generate_md5(char* hash_archivo, char** lista_bloques, uint32_t tamanio_a_leer, uint32_t tamanio_bloques);

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

	char* hash_archivo        = config_get_string_value(this->CONFIG, "MD5_ARCHIVO");
	char** lista_bloques      = config_get_array_value(this->CONFIG, "BLOCKS");
	uint32_t tamanio_archivo  = config_get_int_value(this->CONFIG, "SIZE");
	uint32_t tamanio_bloques  = fs_blocks_manager_get_blocks_size();
	uint32_t cantidad_bloques = config_get_int_value(this->CONFIG, "BLOCK_COUNT");
	char* caracter_llenado    = config_get_string_value(this->CONFIG, "CARACTER_LLENADO");

	t_list* lista_id_bloques = lista_id_bloques_archivo(lista_bloques);

	//verificar si conteo de bloques coincide con la cantidad de bloques.
	uint32_t cantidad_bloques_segun_lista = list_size(lista_id_bloques);
	if(cantidad_bloques_segun_lista != cantidad_bloques)
		config_set_value(this->CONFIG, "BLOCKS", cantidad_bloques_segun_lista);

	//TODO: verificar el size del archivo recorriendo todos los bloques hasta el centinela.
	uint32_t  cantidad_caracteres_file = 0;
	void _contar_cantidad_caracteres_bloque(uint32_t* id_bloque)
	{
		char* caracter_leido = malloc(sizeof(char *));
		uint32_t contador = 0;
		do
		{
			fs_block_read(*id_bloque, caracter_leido, sizeof(char*), contador);
			contador++;

			if(*caracter_leido)
				cantidad_caracteres_file++;
		} while (*caracter_leido && contador != tamanio_bloques);
		free(caracter_leido);

	}
	list_iterate(lista_id_bloques, (void*)_contar_cantidad_caracteres_bloque);
	if(cantidad_caracteres_file != tamanio_archivo)
		config_set_value(this->CONFIG, "SIZE", cantidad_caracteres_file);
	//fin

	
	// ver si el md5 es igual
	char* md5_bloques_archivo = generate_md5(hash_archivo, lista_bloques, tamanio_archivo, tamanio_bloques);
	if(!strcmp(md5_bloques_archivo, hash_archivo))
	{
		uint32_t tamanio_aux = tamanio_archivo;
		uint32_t tamanio_a_insertar = min(tamanio_aux, tamanio_bloques);
		
		void _rellenar_bloques(uint32_t* id_bloque)
		{
			uint32_t tamanio_a_insertar = min(tamanio_aux, tamanio_bloques);
			char* caracter_a_rellenar_repetido = string_repeat(*caracter_llenado, tamanio_a_insertar);
			caracteres_escritos = fs_block_write(*id_bloque, caracter_a_rellenar_repetido, tamanio_a_insertar,0);
			tamanio_aux -= caracteres_escritos;
			free(caracter_a_rellenar_repetido);
		}
		list_iterate(lista_id_bloques, (void*)_rellenar_bloques);
		// acá por ahí podríamos verificar si el md5 está bien ahora
		//luego libero la lista.
	}


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


private char* generate_md5(char* hash_archivo, char** lista_bloques, uint32_t tamanio_a_leer, uint32_t tamanio_bloques)
{
	MD5_CTX contexto;
	char* hash = malloc(MD5_DIGEST_LENGTH);
	void* data_bloques = malloc(tamanio_bloques);
	t_list* id_bloques = lista_id_bloques_archivo(lista_bloques);

	MD5_Init(&contexto);
	void _actualizar_md5(uint32_t* id_bloque)
	{
		uint64_t cant_bytes_leidos = fs_block_read(*id_bloque, data_bloques ,tamanio_a_leer, 0);
		MD5_Update(&contexto, data_bloques, cant_bytes_leidos);
		tamanio_a_leer -= cant_bytes_leidos;
	}
	list_iterate(id_bloques, (void*)_actualizar_md5);
	MD5_Final(hash, &contexto);
	free(data_bloques);
	list_destroy_and_destroy_elements(id_bloques, free);

	return hash;
}

private t_list* lista_id_bloques_archivo(char** lista_bloques)
{
	t_list* bloques_archivo = list_create();
	for(char** bloques = lista_bloques; *bloques != NULL;  bloques ++)
    {
        uint32_t* bloque = u_malloc(sizeof(uint32_t));
        bloque = atoi(*bloques);
        list_add(bloques_archivo, bloque);
        free(*bloques);
    }
	free(lista_bloques);

	return bloques_archivo;
}
