#include "file.h"
#include "fs/blocks/blocks_manager.h"
#include "commons/config.h"

#include <pthread.h>
#include <openssl/md5.h>
#include <stdlib.h>
#include <stdio.h>

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

		config_set_value(this->CONFIG, "MD5_ARCHIVO", NULL);


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


//Agrega la cantidad especificada por "amount" de caracteres de llenado en el file.
// En caso de no poder escribirse todo el contenido, pedir un nuevo bloque y escribir
// la cantidad de caracteres restantes. En caso de que tampoco se pueda escribir
// todo el contenido en el nuevo bloque, seguir pidiendo bloqueas hasta terminar de escribir
// la cantidad total de caracteres. Al final pone un centinela
void fs_file_add_fill_char(fs_file_t* this, uint32_t amount){

	int amount_values = config_get_int_value(this->CONFIG, "BLOCK_COUNT");
	char** blocks = config_get_array_value(this->CONFIG, "BLOCKS");
	t_list* blocks_tlist = lista_id_bloques_archivo(blocks);
	char* last_block = blocks_tlist->elements_count - 1;

	//uint32_t block_id =  atoi(values[amount_values-1]); se hace por t_list en vez de strings

	char* fill_char = config_get_string_value(this->CONFIG, "CARACTER_LLENADO");

	char* fill = string_repeat(*fill_char, amount);

	int escritos = u_malloc(sizeof(int));
	escritos = fs_block_write(last_block, fill, sizeof(fill), get_offset(this));

	if(escritos!=sizeof(fill)){

		list_add(blocks_tlist,fs_blocks_manager_request_block());
		last_block++;

		escritos = fs_block_write(last_block, fill, sizeof(fill), get_offset(this));

		//si no entró sigo pidiendo otros y guardando
		while(escritos!=sizeof(fill)){
			list_add(blocks_tlist,fs_blocks_manager_request_block());
			last_block++;

			escritos += fs_block_write(last_block, fill, sizeof(fill)-escritos, get_offset(this));
		}

		//guardo centinela
		fs_block_write(last_block, 0, sizeof(int), get_offset(this));

		//actualizo config blocks, guardo tlist en string config
		config_set_value(this->CONFIG, "BLOCKS", list_convert_to_string(blocks_tlist));

		//actualizo config md5
		config_set_value(this->CONFIG, "MD5_ARCHIVO", generate_md5(config_get_string_value(this->CONFIG, "MD5_ARCHIVO"), config_get_array_value(this->CONFIG, "BLOCKS"), config_get_int_value(this->CONFIG, "SIZE"), fs_blocks_manager_get_blocks_size()));

	}
	u_free(fill);
	u_free(fill_char);
	u_free(escritos);
	u_free(last_block);
	u_free(blocks);
	u_free(blocks_tlist);
	u_free(amount_values);
}

//Elimina la cantidad especificada por "amount" de caracteres de llenado en el file.
// Esto se puede hacer simplemente reduciendo el tamaño del archivo. Luego, se deberia dividir
// el tamaño actual del archivo por el tamaño de un bloque (que puede ser consultado mediante la
// funcion "fs_blocks_manager_get_blocks_size"). El resultado sera la cantidad de bloques que el
// archivo estaría usando. Si es menor a la cantidad de bloques que posee actualmente, liberar los
// ultimos bloques del archivo. Por ejemplo, si despues de reducir el tamaño del archivo, la cantidad
// de bloques que ocupa el archivo son 4 y el archivo tiene ocupados 6 bloques, se deben liberar,
// los últimos 2 bloques (llamando a la función "fs_blocks_manager_release_block(num_bloque)")
//escribo como último caracter el centinela
void fs_file_remove_fill_char(fs_file_t* this, uint32_t amount){

	//reduzco tamaño del archivo
	int size_archivo = config_get_int_value(this->CONFIG, "SIZE");
	config_set_value(this->CONFIG, "SIZE",size_archivo-amount);

	//guardo centinela
	fs_block_write(config_get_int_value(this->CONFIG, "BLOCK_COUNT")-1, 0, sizeof(int), get_offset(this));

	int bloques_usados = config_get_array_value(this->CONFIG, "SIZE") / fs_blocks_manager_get_blocks_size();

	if(bloques_usados < config_get_int_value(this->CONFIG, "BLOCK_COUNT")){
		int bloques_a_liberar = bloques_usados - config_get_int_value(this->CONFIG, "BLOCK_COUNT");

		for(int i=0; i<bloques_a_liberar ; i++){
			fs_blocks_manager_release_block(config_get_int_value(this->CONFIG, "BLOCK_COUNT")-1);
		}
	}

	char** blocks = config_get_array_value(this->CONFIG, "BLOCKS");
	t_list* blocks_tlist = lista_id_bloques_archivo(blocks);

	//actualizo config blocks, guardo tlist en string config
	config_set_value(this->CONFIG, "BLOCKS", list_convert_to_string(blocks_tlist));

	//actualizo config md5
	config_set_value(this->CONFIG, "MD5_ARCHIVO", generate_md5(config_get_string_value(this->CONFIG, "MD5_ARCHIVO"), config_get_array_value(this->CONFIG, "BLOCKS"), config_get_int_value(this->CONFIG, "SIZE"), fs_blocks_manager_get_blocks_size()));

	u_free(size_archivo);
	u_free(blocks);
	u_free(blocks_tlist);
	u_free(bloques_usados);
}

bool fs_file_check_integrity(fs_file_t* this){

	char* hash_archivo       =  config_get_string_value(this->CONFIG, "MD5_ARCHIVO");
	char** lista_bloques     =  config_get_array_value(this->CONFIG, "BLOCKS");
	uint32_t tamanio_archivo =  config_get_int_value(this->CONFIG, "SIZE");
	uint32_t tamanio_bloques =  fs_blocks_manager_get_blocks_size();

	char* md5_bloques_archivo = generate_md5(hash_archivo, lista_bloques, tamanio_archivo, tamanio_bloques);
	if(!strcmp(md5_bloques_archivo, hash_archivo))
	{
		t_list* lista_id_bloques = lista_id_bloques_archivo(lista_bloques);
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
	void _actualizarMd5(uint32_t* id_bloque)
	{
		uint64_t cant_bytes_leidos = fs_block_read(*id_bloque, data_bloques ,tamanio_a_leer, 0);
		MD5_Update(&contexto, data_bloques, cant_bytes_leidos);
		tamanio_a_leer -= cant_bytes_leidos;
	}
	list_iterate(id_bloques, (void*)_actualizarMd5);
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

private int get_offset(fs_file_t* this){
	return config_get_array_value(this->CONFIG, "SIZE") % fs_blocks_manager_get_blocks_size();
}

private char** list_convert_to_string(t_list* list){
	char* string_list ="[";

	for(int i=0; i<list->elements_count ; i++){
		strcat(string_list,list_get(list, i));
	}

	strcat(string_list,"]");

	return string_list;
}


