#include "file.h"
#include "fs/blocks/blocks_manager.h"
#include "commons/config.h"

#include <pthread.h>
#include <openssl/md5.h>
#include <stdlib.h>
#include <stdio.h>
#define min(a, b)  (((a) < (b)) ? (a) : (b))
#define max(a, b)  (((a) > (b)) ? (a) : (b))

struct fs_file_t{
	char*		NOMBRE_ARCHIVO;
    t_config* 	CONFIG;
};
private t_list* lista_id_bloques_archivo(char** lista_bloques);
private char* generate_md5(t_list* lista_bloques, uint32_t tamanio_a_leer, uint32_t tamanio_bloques);
private int get_offset(fs_file_t* this);
private char* list_convert_to_string(t_list* list);
private bool verificar_size_correcto(fs_file_t* this);
private bool verificar_cantidad_bloques_correcto(fs_file_t* this);
private bool verificar_md5(fs_file_t* this);


fs_file_t* fs_file_create(const char* file_path, char* fill_char){

	fs_file_t* this = u_malloc(sizeof(fs_file_t));

	this->NOMBRE_ARCHIVO = strdup(file_path);
	this->CONFIG         = config_create(file_path);

	config_set_value(this->CONFIG, "SIZE", "0");

	uint32_t block_id = fs_blocks_manager_request_block();
	char* block_list = string_from_format("[%d]", block_id);

	config_set_value(this->CONFIG, "BLOCKS", block_list);
	config_set_value(this->CONFIG, "BLOCK_COUNT", "1");
	config_set_value(this->CONFIG, "CARACTER_LLENADO", fill_char);
	config_set_value(this->CONFIG, "MD5_ARCHIVO", NULL);

    return this;
}

//Elimina la estructura de la bitacora y libera los bloques que tenga asignados.
void fs_file_delete(fs_file_t* this){

	char** block_list = config_get_array_value(this->CONFIG, "BLOCKS");
	for(int i=0; i<config_get_int_value(this->CONFIG, "BLOCKS_COUNT") ; i++){
		fs_blocks_manager_release_block(atoi(block_list[i]));
		free(block_list[i]);
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

	if(amount == 0){
		U_LOG_WARN("Recibi un pedido para escribir cero caracteres");
		return; //En caso de que me pidan rellenar con cantidad menor 
	}
	char** blocks = config_get_array_value(this->CONFIG, "BLOCKS");
	char* fill_char = fs_file_get_fill_char(this);
	uint32_t tamanio_archivo = fs_file_get_size(this);
	t_list* blocks_tlist = lista_id_bloques_archivo(blocks);
	uint32_t tamanio_bloques = fs_blocks_manager_get_blocks_size();
	uint32_t index_ultimo_id_bloque = fs_file_get_blocks_count(this) - 1;
	uint32_t escritos, a_escribir_en_bloque, nuevo_tamanio_archivo;
	uint32_t* last_block = list_get(blocks_tlist, index_ultimo_id_bloque);
	nuevo_tamanio_archivo = tamanio_archivo + amount;
	int offset = get_offset(this);
	char* caracter_repetido;

	//condicion por si el ultimo bloque no está lleno
	if(offset || !tamanio_archivo){
		a_escribir_en_bloque = tamanio_bloques - offset;
		caracter_repetido = string_repeat(*fill_char, a_escribir_en_bloque);
		if(a_escribir_en_bloque> tamanio_bloques){
			a_escribir_en_bloque++;
		}
		escritos = fs_block_write(*last_block, caracter_repetido, a_escribir_en_bloque, offset);
		amount -= escritos;
		free(caracter_repetido);
	}

	while(!amount){
		uint32_t* new_bloque_id = malloc(sizeof(uint32_t));
		*new_bloque_id = fs_blocks_manager_request_block();
		list_add(blocks_tlist, new_bloque_id);
		a_escribir_en_bloque = min(amount, tamanio_bloques);
		caracter_repetido = string_repeat(*fill_char, a_escribir_en_bloque);
		if(a_escribir_en_bloque < tamanio_bloques){
			a_escribir_en_bloque++;
		}
		escritos = fs_block_write(*new_bloque_id, caracter_repetido, a_escribir_en_bloque, 0);
		amount -= escritos;
		free(caracter_repetido);
	}

	//actualizo config md5
	char* md5_actualizado = generate_md5(blocks_tlist, fs_file_get_size(this), tamanio_bloques);
	config_set_value(this->CONFIG, "MD5_ARCHIVO", md5_actualizado);

	//actualizo config blocks, guardo tlist en string config
	char* lista_a_string = list_convert_to_string(blocks_tlist);
	config_set_value(this->CONFIG, "BLOCKS", lista_a_string);

	char* tamanio_en_string = string_itoa(nuevo_tamanio_archivo);
	config_set_value(this->CONFIG, "SIZE", tamanio_en_string);

	char* cantidad_bloques_string = string_itoa(list_size(blocks_tlist));
	config_set_value(this->CONFIG,"BLOCK_COUNT", cantidad_bloques_string);


	list_destroy_and_destroy_elements(blocks_tlist, free);
	u_free(fill_char);
	u_free(tamanio_en_string);
	u_free(lista_a_string);
	u_free(cantidad_bloques_string);
	u_free(md5_actualizado);
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
	uint32_t size_archivo = fs_file_get_size(this);
	if(!size_archivo)
	{
		U_LOG_WARN("pedido para remover caracteres de un file sin contenido");
		return;
	}
	uint32_t size_archivo_actualizado = max(0, size_archivo - amount);
	uint32_t tamanio_bloques = fs_blocks_manager_get_blocks_size();
	char** bloques_formato_array = config_get_array_value(this->CONFIG, "BLOCKS");
	t_list* blocks_tlist = lista_id_bloques_archivo(bloques_formato_array);
	uint32_t indice_ultimo_bloque = size_archivo_actualizado / tamanio_bloques;
	uint32_t offset = size_archivo_actualizado % tamanio_bloques;
	if(offset || !size_archivo_actualizado)
	{
		uint32_t* id_ultimo_bloque = list_get(blocks_tlist, indice_ultimo_bloque);
		char* centinela = malloc(1);
		*centinela = '\0';
		fs_block_write(*id_ultimo_bloque, centinela, 1, offset);
		u_free(centinela);
	}

	uint32_t indice_max_lista = list_size(blocks_tlist);
	while( indice_max_lista!= indice_ultimo_bloque)
	{
		uint32_t* id_bloque = list_remove(blocks_tlist, indice_max_lista);
		fs_blocks_manager_release_block(*id_bloque);
		free(id_bloque);
		indice_max_lista --;

	}

	char* string_de_id_bloques = list_convert_to_string(blocks_tlist);
	//actualizo config blocks, guardo tlist en string config
	config_set_value(this->CONFIG, "BLOCKS", string_de_id_bloques);

	//actualizo config md5
	char* md5_actualizado = generate_md5(blocks_tlist, size_archivo_actualizado, tamanio_bloques);
	config_set_value(this->CONFIG, "MD5_ARCHIVO", md5_actualizado);

	char* string_tamanio_archivo = string_itoa(size_archivo_actualizado);
	config_set_value(this->CONFIG, "SIZE", string_tamanio_archivo);

	char* string_cantidad_bloques = string_itoa(indice_ultimo_bloque + 1);
	config_set_value(this->CONFIG,"BLOCK_COUNT", string_cantidad_bloques);

	list_destroy_and_destroy_elements(blocks_tlist, u_free);
	u_free(string_de_id_bloques);
	u_free(md5_actualizado);
	u_free(string_tamanio_archivo);
	u_free(string_cantidad_bloques);
}

bool fs_file_check_integrity(fs_file_t* this){
	return verificar_size_correcto(this) && verificar_cantidad_bloques_correcto(this) && verificar_md5(this);

}

const char* fs_file_get_name(const fs_file_t* this){
    return this->NOMBRE_ARCHIVO;
}

const char* fs_file_get_md5(const fs_file_t* this){
    return config_get_string_value(this->CONFIG, "MD5_ARCHIVO");
}

uint32_t fs_file_get_size(const fs_file_t* this){
    return config_get_int_value(this->CONFIG, "SIZE");
}

char* fs_file_get_fill_char(const fs_file_t* this){
	return config_get_string_value(this->CONFIG, "CARACTER_LLENADO");
}

uint32_t fs_file_get_blocks_count(const fs_file_t* this){
    return config_get_int_value(this->CONFIG, "BLOCK_COUNT");
}


private char* generate_md5(t_list* id_bloques, uint32_t tamanio_a_leer, uint32_t tamanio_bloques)
{
	MD5_CTX contexto;
	char* hash = malloc(MD5_DIGEST_LENGTH);
	void* data_bloques = malloc(tamanio_bloques);

	MD5_Init(&contexto);
	void _actualizar_md5(uint32_t* id_bloque)
	{
		uint64_t cant_bytes_leidos = fs_block_read(*id_bloque, data_bloques ,tamanio_a_leer, 0);
		MD5_Update(&contexto, data_bloques, cant_bytes_leidos);
		tamanio_a_leer -= cant_bytes_leidos;
	}
	list_iterate(id_bloques, (void*)_actualizar_md5);
	MD5_Final((unsigned char*)hash, &contexto);
	free(data_bloques);

	return hash;
}


private t_list* lista_id_bloques_archivo(char** lista_bloques)
{
	t_list* bloques_archivo = list_create();
	for(char** bloques = lista_bloques; *bloques != NULL;  bloques ++)
    {
        uint32_t* bloque = u_malloc(sizeof(uint32_t));
        *bloque = atoi(*bloques);
        list_add(bloques_archivo, bloque);
        free(*bloques);
    }
	free(lista_bloques);

	return bloques_archivo;
}

private int get_offset(fs_file_t* this){
	return fs_file_get_size(this) % fs_blocks_manager_get_blocks_size();
}

private char* list_convert_to_string(t_list* list){
	char* string_list = string_new();
	bool es_primero = true;
	string_append(&string_list,"[");
	
	void _agregar_a_string(uint32_t* id_bloque)
	{
		if(es_primero){
			es_primero = false;
			string_append_with_format(&string_list, "%d", *id_bloque);
		}
		else
		{
			string_append_with_format(&string_list, ",%d", *id_bloque);
		}
		
	}
	string_append(&string_list,"]");
	list_iterate(list, (void*)_agregar_a_string);

	return string_list;
}


private bool verificar_size_correcto(fs_file_t* this)
{
	// Acá recorro todos los bloques, y aunque encuentre el bloque con el centinela sigo recorriendo si en mi lista de bloques tengo más.
	char** lista_bloques = config_get_array_value(this->CONFIG, "BLOCKS");
	uint32_t tamanio_bloques  = fs_blocks_manager_get_blocks_size();
	t_list* lista_id_bloques = lista_id_bloques_archivo(lista_bloques);
	uint32_t tamanio_archivo  = fs_file_get_size(this);
	uint32_t  cantidad_caracteres_file = 0;
	bool estado_no_corrompido = true;

	void _contar_cantidad_caracteres_bloque(uint32_t* id_bloque)
	{
		char* caracter_leido = malloc(sizeof(char*));
		uint32_t contador = 0;
		do
		{
			fs_block_read(*id_bloque, caracter_leido, sizeof(char), contador);
			contador++;

			if(*caracter_leido)
				cantidad_caracteres_file++;
		} while (*caracter_leido && contador != tamanio_bloques);
		free(caracter_leido);
	}

	list_iterate(lista_id_bloques, (void*)_contar_cantidad_caracteres_bloque);
	if(cantidad_caracteres_file != tamanio_archivo)
	{
		char* caracter_cantidad_caracteres_file = string_itoa(cantidad_caracteres_file);
		config_set_value(this->CONFIG, "SIZE", caracter_cantidad_caracteres_file);
		u_free(caracter_cantidad_caracteres_file);
		estado_no_corrompido = false;
	}
	list_destroy_and_destroy_elements(lista_id_bloques, free);

	return estado_no_corrompido;
}

private bool verificar_cantidad_bloques_correcto(fs_file_t* this)
{
	char** lista_bloques = config_get_array_value(this->CONFIG, "BLOCKS");
	uint32_t cantidad_bloques = fs_file_get_blocks_count(this);
	t_list* lista_id_bloques = lista_id_bloques_archivo(lista_bloques);
	uint32_t cantidad_bloques_segun_lista = list_size(lista_id_bloques);
	bool estado_no_corrompido = true;

	if(cantidad_bloques_segun_lista != cantidad_bloques)
	{
		char* caracter_cantidad_bloques_segun_lista = string_itoa(cantidad_bloques_segun_lista);
		config_set_value(this->CONFIG, "BLOCKS", caracter_cantidad_bloques_segun_lista);
		u_free(caracter_cantidad_bloques_segun_lista);
		estado_no_corrompido= false;
	}
	list_destroy_and_destroy_elements(lista_id_bloques, free);
	return estado_no_corrompido;
}

private bool verificar_md5(fs_file_t* this)
{
	//esta recuperación la llevo sin intentar obtener nuevos bloques dado que en el video dicen que solo cambian de posicion los bloques. (a validar)
	char* hash_archivo = config_get_string_value(this->CONFIG, "MD5_ARCHIVO");
	uint32_t tamanio_archivo = fs_file_get_size(this);
	char** lista_bloques = config_get_array_value(this->CONFIG, "BLOCKS");
	char* caracter_llenado = fs_file_get_fill_char(this);
	uint32_t tamanio_bloques  = fs_blocks_manager_get_blocks_size();
	t_list* lista_id_bloques = lista_id_bloques_archivo(lista_bloques);
	bool estado_no_corrompido = true;
	char* md5_bloques_archivo = generate_md5(lista_id_bloques, tamanio_archivo, tamanio_bloques);

	if(!strcmp(md5_bloques_archivo, hash_archivo))
	{
		uint32_t tamanio_aux = tamanio_archivo;
		uint32_t caracteres_escritos;
		void _rellenar_bloques(uint32_t* id_bloque)
		{
			uint32_t tamanio_a_insertar = min(tamanio_aux, tamanio_bloques);
			char* caracter_a_rellenar_repetido = string_repeat(*caracter_llenado, tamanio_a_insertar);
			if(tamanio_a_insertar <tamanio_bloques){
				tamanio_a_insertar++;
			}
			caracteres_escritos = fs_block_write(*id_bloque, caracter_a_rellenar_repetido, tamanio_a_insertar,0);
			tamanio_aux -= caracteres_escritos;
			free(caracter_a_rellenar_repetido);
		}
		list_iterate(lista_id_bloques, (void*)_rellenar_bloques);
		// acá por ahí podríamos verificar si el md5 está bien ahora
		//luego libero la lista.
		estado_no_corrompido = false;
	}
	list_destroy_and_destroy_elements(lista_id_bloques, free);
	free(md5_bloques_archivo);
	return estado_no_corrompido;
}
