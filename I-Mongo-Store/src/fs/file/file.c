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
private char* generate_md5(char** lista_bloques, uint32_t tamanio_a_leer, uint32_t tamanio_bloques);
private int get_offset(fs_file_t* this);
private char** list_convert_to_string(t_list* list);
private bool verificar_size_correcto(fs_file_t* this);
private bool verificar_cantidad_bloques_correcto(fs_file_t* this);
private bool verificar_md5(fs_file_t* this);


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

	int amount_values = fs_file_get_blocks_count();
	char** blocks = config_get_array_value(this->CONFIG, "BLOCKS");
	t_list* blocks_tlist = lista_id_bloques_archivo(blocks);
	char* last_block = blocks_tlist->elements_count - 1;

	//uint32_t block_id =  atoi(values[amount_values-1]); se hace por t_list en vez de strings

	char* fill_char = fs_file_get_fill_char();

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
		char* md5_actualizado = generate_md5(blocks, fs_file_get_size(), fs_blocks_manager_get_blocks_size());
		config_set_value(this->CONFIG, "MD5_ARCHIVO", md5_actualizado);

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
	int size_archivo = fs_file_get_size();
	config_set_value(this->CONFIG, "SIZE",size_archivo-amount);
	uint32_t tamanio_bloques = fs_blocks_manager_get_blocks_size();

	//guardo centinela
	fs_block_write(fs_file_get_blocks_count()-1, 0, sizeof(int), get_offset(this));

	int bloques_usados = fs_file_get_size() / tamanio_bloques;

	if(bloques_usados < fs_file_get_blocks_count()){
		int bloques_a_liberar = bloques_usados - fs_file_get_blocks_count();

		for(int i=0; i<bloques_a_liberar ; i++){
			fs_blocks_manager_release_block(fs_file_get_blocks_count()-1);
		}
	}

	char** blocks = config_get_array_value(this->CONFIG, "BLOCKS");
	t_list* blocks_tlist = lista_id_bloques_archivo(blocks);

	//actualizo config blocks, guardo tlist en string config
	config_set_value(this->CONFIG, "BLOCKS", list_convert_to_string(blocks_tlist));

	//actualizo config md5
	char* md5_actualizado = generate_md5(blocks, size_archivo, tamanio_bloques);
	config_set_value(this->CONFIG, "MD5_ARCHIVO", md5_actualizado);

	u_free(size_archivo);
	u_free(blocks);
	u_free(blocks_tlist);
	u_free(bloques_usados);
}

bool fs_file_check_integrity(fs_file_t* this){
	return verificar_size_correcto(this) && verificar_cantidad_bloques_correcto(this) && verificar_md5(this);

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


private char* generate_md5(char** lista_bloques, uint32_t tamanio_a_leer, uint32_t tamanio_bloques)
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

private int get_offset(fs_file_t* this){
	return fs_file_get_size() % fs_blocks_manager_get_blocks_size();
}

private char** list_convert_to_string(t_list* list){
	char* string_list ="[";

	for(int i=0; i<list->elements_count ; i++){
		strcat(string_list,list_get(list, i));
	}

	strcat(string_list,"]");

	return string_list;
}


private bool verificar_size_correcto(fs_file_t* this)
{
	// Acá recorro todos los bloques, y aunque encuentre el bloque con el centinela sigo recorriendo si en mi lista de bloques tengo más.
	uint32_t tamanio_archivo  = fs_file_get_size();
	uint32_t  cantidad_caracteres_file = 0;
	void _contar_cantidad_caracteres_bloque(uint32_t* id_bloque)
	{
		char* caracter_leido = malloc(sizeof(char*));
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
	{
		config_set_value(this->CONFIG, "SIZE", cantidad_caracteres_file);
		return false;
	}

	return true;
}

private bool verificar_cantidad_bloques_correcto(fs_file_t* this)
{
	char** lista_bloques = config_get_array_value(this->CONFIG, "BLOCKS");
	uint32_t cantidad_bloques = fs_file_get_blocks_count();
	t_list* lista_id_bloques = lista_id_bloques_archivo(lista_bloques);
	uint32_t cantidad_bloques_segun_lista = list_size(lista_id_bloques);

	if(cantidad_bloques_segun_lista != cantidad_bloques)
	{
		config_set_value(this->CONFIG, "BLOCKS", cantidad_bloques_segun_lista);
		return false;
	}
	
	return true;
}

private bool verificar_md5(fs_file_t* this)
{
	//esta recuperación la llevo sin intentar obtener nuevos bloques dado que en el video dicen que solo cambian de posicion los bloques. (a validar)
	char* hash_archivo = config_get_string_value(this->CONFIG, "MD5_ARCHIVO");
	uint32_t tamanio_archivo = fs_file_get_size();
	char** lista_bloques = config_get_array_value(this->CONFIG, "BLOCKS");
	uint32_t tamanio_bloques  = fs_blocks_manager_get_blocks_size();
	char* md5_bloques_archivo = generate_md5(lista_bloques, tamanio_archivo, tamanio_bloques);

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
		return false;
	}
	return true;
}
