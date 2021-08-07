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
	char*			NOMBRE_ARCHIVO;
    t_config* 		CONFIG;
	pthread_mutex_t MX;
};
private t_list* lista_id_bloques_archivo(char** lista_bloques);
private char* generate_md5(t_list* lista_bloques, uint32_t tamanio_a_leer, uint32_t tamanio_bloques);
private int get_offset(fs_file_t* this);
private char* list_convert_to_string(t_list* list);
private bool verificar_size_correcto(fs_file_t* this, t_config* config_actual);
private bool verificar_cantidad_bloques_correcto(fs_file_t* this, t_config* config_actual);
private bool verificar_md5(fs_file_t* this, t_config* config_actual);


fs_file_t* fs_file_create(const char* file_path, char* fill_char){

	fs_file_t* this = u_malloc(sizeof(fs_file_t));

	this->NOMBRE_ARCHIVO = strdup(file_path);
	this->CONFIG         = config_create((char*)file_path);

	pthread_mutex_init(&this->MX, NULL);

	if(fill_char)
	{
		config_set_value(this->CONFIG, "SIZE", "0");

		uint32_t block_id = fs_blocks_manager_request_block();
		char* block_list = string_from_format("[%d]", block_id);

		config_set_value(this->CONFIG, "BLOCKS", block_list);
		config_set_value(this->CONFIG, "BLOCK_COUNT", "1");
		config_set_value(this->CONFIG, "CARACTER_LLENADO", fill_char);

		char* hash = generate_md5(NULL, 0, fs_blocks_manager_get_blocks_size());
		config_set_value(this->CONFIG, "MD5_ARCHIVO", hash);

		char centinela = '\0';
		fs_block_write(block_id, &centinela, 1, 0);

		config_save(this->CONFIG);

		u_free(hash);
		u_free(block_list);

		U_LOG_INFO("Archivo %s creado.", this->NOMBRE_ARCHIVO);
	}
	else
		U_LOG_INFO("Archivo %s cargado.", this->NOMBRE_ARCHIVO);

    return this;
}

//Elimina la estructura de la bitacora y libera los bloques que tenga asignados.
void fs_file_delete(fs_file_t* this){
	U_LOG_INFO("Se elimina el archivo %s.", this->NOMBRE_ARCHIVO);

	char** block_list = config_get_array_value(this->CONFIG, "BLOCKS");
	for(int i=0; i<config_get_int_value(this->CONFIG, "BLOCK_COUNT") ; i++){
		fs_blocks_manager_release_block(atoi(block_list[i]));
		free(block_list[i]);
	}

	pthread_mutex_destroy(&this->MX);
	config_destroy(this->CONFIG);
	remove(this->NOMBRE_ARCHIVO);
	
	u_free(this->NOMBRE_ARCHIVO);
    u_free(block_list);
    u_free(this);

}


//Agrega la cantidad especificada por "amount" de caracteres de llenado en el file.
// En caso de no poder escribirse todo el contenido, pedir un nuevo bloque y escribir
// la cantidad de caracteres restantes. En caso de que tampoco se pueda escribir
// todo el contenido en el nuevo bloque, seguir pidiendo bloqueas hasta terminar de escribir
// la cantidad total de caracteres. Al final pone un centinela
void fs_file_add_fill_char(fs_file_t* this, uint32_t amount){
	pthread_mutex_lock(&this->MX);
	uint32_t req_amount = amount;

	if(amount == 0){
		U_LOG_WARN("Recibi un pedido para escribir cero caracteres");
		return; //En caso de que me pidan rellenar con cantidad menor 
	}
	char** blocks = config_get_array_value(this->CONFIG, "BLOCKS");
	const char* fill_char = fs_file_get_fill_char(this);
	uint32_t tamanio_archivo = fs_file_get_size(this);
	t_list* blocks_tlist = lista_id_bloques_archivo(blocks);
	uint32_t tamanio_bloques = fs_blocks_manager_get_blocks_size();
	uint32_t index_ultimo_id_bloque = fs_file_get_blocks_count(this) - 1;
	uint32_t escritos, nuevo_tamanio_archivo;
	uint32_t* last_block = list_get(blocks_tlist, index_ultimo_id_bloque);
	nuevo_tamanio_archivo = tamanio_archivo + amount;
	int offset = get_offset(this);
	char* caracter_repetido = string_repeat(*fill_char, amount);

	escritos = fs_block_write(*last_block, caracter_repetido, amount, offset);
	amount -= escritos;

	while(amount){
		last_block = malloc(sizeof(uint32_t));
		*last_block = fs_blocks_manager_request_block();
		list_add(blocks_tlist, last_block);

		escritos = fs_block_write(*last_block, caracter_repetido, amount, 0);
		amount -= escritos;

	}
	free(caracter_repetido);

	offset = nuevo_tamanio_archivo % tamanio_bloques;
	if(offset != 0)
	{
		char nulo = '\0';
		fs_block_write(*last_block, &nulo, sizeof(char), offset);
	}

	//actualizo config md5
	char* md5_actualizado = generate_md5(blocks_tlist, nuevo_tamanio_archivo, tamanio_bloques);
	config_set_value(this->CONFIG, "MD5_ARCHIVO", md5_actualizado);

	//actualizo config blocks, guardo tlist en string config
	char* lista_a_string = list_convert_to_string(blocks_tlist);
	config_set_value(this->CONFIG, "BLOCKS", lista_a_string);

	char* tamanio_en_string = string_itoa(nuevo_tamanio_archivo);
	config_set_value(this->CONFIG, "SIZE", tamanio_en_string);

	char* cantidad_bloques_string = string_itoa(list_size(blocks_tlist));
	config_set_value(this->CONFIG,"BLOCK_COUNT", cantidad_bloques_string);

	config_save(this->CONFIG);

	list_destroy_and_destroy_elements(blocks_tlist, free);

	U_LOG_INFO("Se agregaron %d caracteres %s al archivo %s", req_amount, fill_char, this->NOMBRE_ARCHIVO);
	U_LOG_INFO("MD5 del archivo: %s", md5_actualizado);

	u_free(tamanio_en_string);
	u_free(lista_a_string);
	u_free(cantidad_bloques_string);
	u_free(md5_actualizado);

	pthread_mutex_unlock(&this->MX);
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
	pthread_mutex_lock(&this->MX);

	uint32_t req_amount = amount;
	//reduzco tamaño del archivo
	uint32_t size_archivo = fs_file_get_size(this);
	if(!size_archivo)
	{
		U_LOG_WARN("pedido para remover caracteres de un file sin contenido");
		return;
	}
	uint32_t size_archivo_actualizado = max(0, (int)(size_archivo - amount));
	uint32_t tamanio_bloques = fs_blocks_manager_get_blocks_size();
	char** bloques_formato_array = config_get_array_value(this->CONFIG, "BLOCKS");
	t_list* blocks_tlist = lista_id_bloques_archivo(bloques_formato_array);
	uint32_t indice_ultimo_bloque = (size_archivo_actualizado / (tamanio_bloques + 1));
	uint32_t offset = size_archivo_actualizado % tamanio_bloques;
	if(offset || !size_archivo_actualizado)
	{
		uint32_t* id_ultimo_bloque = list_get(blocks_tlist, indice_ultimo_bloque);
		char centinela = '\0';
		fs_block_write(*id_ultimo_bloque, &centinela, 1, offset);
	}

	uint32_t indice_max_lista = list_size(blocks_tlist) - 1;
	while( indice_max_lista!= indice_ultimo_bloque)
	{
		uint32_t* id_bloque = list_remove(blocks_tlist, indice_max_lista);
		fs_blocks_manager_release_block(*id_bloque);
		free(id_bloque);
		indice_max_lista --;

	}

	char* string_de_id_bloques = list_convert_to_string(blocks_tlist);

	//actualizo config md5
	char* md5_actualizado = generate_md5(blocks_tlist, size_archivo_actualizado, tamanio_bloques);

	config_set_value(this->CONFIG, "MD5_ARCHIVO", md5_actualizado);
	//actualizo config blocks, guardo tlist en string config
	config_set_value(this->CONFIG, "BLOCKS", string_de_id_bloques);

	char* string_tamanio_archivo = string_itoa(size_archivo_actualizado);
	config_set_value(this->CONFIG, "SIZE", string_tamanio_archivo);

	char* string_cantidad_bloques = string_itoa(indice_ultimo_bloque + 1);
	config_set_value(this->CONFIG,"BLOCK_COUNT", string_cantidad_bloques);

	config_save(this->CONFIG);

	U_LOG_INFO("Se sacaron %d caracteres %s al archivo %s", req_amount, config_get_string_value(this->CONFIG, "CARACTER_LLENADO"), this->NOMBRE_ARCHIVO);
	U_LOG_INFO("MD5 del archivo: %s", md5_actualizado);

	list_destroy_and_destroy_elements(blocks_tlist, u_free);
	u_free(string_de_id_bloques);
	u_free(md5_actualizado);
	u_free(string_tamanio_archivo);
	u_free(string_cantidad_bloques);

	pthread_mutex_unlock(&this->MX);
}

bool fs_file_check_integrity(fs_file_t* this){
	t_config* config_actual = config_create(this->NOMBRE_ARCHIVO);

    bool verifica_size, verifica_block_count, verifica_bloque;
    verifica_size = verificar_size_correcto(this, config_actual);
    verifica_block_count = verificar_cantidad_bloques_correcto(this, config_actual);
    verifica_bloque = verificar_md5(this, config_actual);

	config_destroy(config_actual);
    
    return verifica_size|| verifica_block_count || verifica_bloque;
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

const char* fs_file_get_fill_char(const fs_file_t* this){
	return config_get_string_value(this->CONFIG, "CARACTER_LLENADO");
}

uint32_t fs_file_get_blocks_count(const fs_file_t* this){
    return config_get_int_value(this->CONFIG, "BLOCK_COUNT");
}

t_list* fs_file_get_blocks(const fs_file_t* this)
{
	char** blocks = config_get_array_value(this->CONFIG, "BLOCKS");
	return lista_id_bloques_archivo(blocks);
}


private char* generate_md5(t_list* id_bloques, uint32_t tamanio_a_leer, uint32_t tamanio_bloques)
{
	MD5_CTX contexto;
	char* hash = malloc(MD5_DIGEST_LENGTH + 1);
	void* data_bloques = malloc(tamanio_bloques);

	void _actualizar_md5(uint32_t* id_bloque)
	{
		uint64_t cant_bytes_leidos = fs_block_read(*id_bloque, data_bloques ,tamanio_a_leer, 0);
		MD5_Update(&contexto, data_bloques, cant_bytes_leidos);
		tamanio_a_leer -= cant_bytes_leidos;
	}

	MD5_Init(&contexto);
    if(tamanio_a_leer == 0)
    {
        char centinela = '\0';
        MD5_Update(&contexto, &centinela, 1);
    }
    else
    {
        list_iterate(id_bloques, (void*)_actualizar_md5);
    }

    MD5_Final((unsigned char*)hash, &contexto);
    char* string_hash = malloc( MD5_DIGEST_LENGTH * 2 + 1);
    for (int i = 0, j = 0; i < 16; i++, j+=2)
        sprintf(string_hash+j, "%02x", (unsigned char)hash[i]);
    string_hash[MD5_DIGEST_LENGTH * 2] = 0;

    free(data_bloques);
    free(hash);

    return string_hash;
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
	list_iterate(list, (void*)_agregar_a_string);
	string_append(&string_list,"]");

	return string_list;
}


private bool verificar_size_correcto(fs_file_t* this, t_config* config_actual)
{
	// Acá recorro todos los bloques, y aunque encuentre el bloque con el centinela sigo recorriendo si en mi lista de bloques tengo más.
	char** lista_bloques = config_get_array_value(config_actual, "BLOCKS");
	uint32_t tamanio_bloques  = fs_blocks_manager_get_blocks_size();
	t_list* lista_id_bloques = lista_id_bloques_archivo(lista_bloques);
	uint32_t tamanio_archivo  = config_get_int_value(config_actual, "SIZE");
	uint32_t  cantidad_caracteres_file = 0;
	bool estado_corrompido = false;

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
		config_set_value(config_actual, "SIZE", caracter_cantidad_caracteres_file);
		u_free(caracter_cantidad_caracteres_file);
		estado_corrompido = true;

		U_LOG_INFO("Hubo sabotaje de SIZE en el archivo %s.", this->NOMBRE_ARCHIVO);
	}
	list_destroy_and_destroy_elements(lista_id_bloques, free);

	config_save(config_actual);

	return estado_corrompido;
}

private bool verificar_cantidad_bloques_correcto(fs_file_t* this, t_config* config_actual)
{
	char** lista_bloques = config_get_array_value(config_actual, "BLOCKS");
	uint32_t cantidad_bloques = config_get_int_value(config_actual, "BLOCK_COUNT");
	t_list* lista_id_bloques = lista_id_bloques_archivo(lista_bloques);
	uint32_t cantidad_bloques_segun_lista = list_size(lista_id_bloques);
	bool estado_corrompido = false;

	if(cantidad_bloques_segun_lista != cantidad_bloques)
	{
		char* caracter_cantidad_bloques_segun_lista = string_itoa(cantidad_bloques_segun_lista);
		config_set_value(config_actual, "BLOCKS", caracter_cantidad_bloques_segun_lista);
		u_free(caracter_cantidad_bloques_segun_lista);
		estado_corrompido= true;

		U_LOG_INFO("Hubo sabotaje de BLOCK_COUNT en el archivo %s.", this->NOMBRE_ARCHIVO);
	}
	list_destroy_and_destroy_elements(lista_id_bloques, free);

	config_save(config_actual);

	return estado_corrompido;
}

private bool verificar_md5(fs_file_t* this, t_config* config_actual)
{
	//esta recuperación la llevo sin intentar obtener nuevos bloques dado que en el video dicen que solo cambian de posicion los bloques. (a validar)
	char* hash_archivo = config_get_string_value(config_actual, "MD5_ARCHIVO");
	uint32_t tamanio_archivo = config_get_int_value(config_actual, "SIZE");
	char** lista_bloques = config_get_array_value(config_actual, "BLOCKS");
	const char* caracter_llenado = fs_file_get_fill_char(this);
	uint32_t tamanio_bloques  = fs_blocks_manager_get_blocks_size();
	t_list* lista_id_bloques = lista_id_bloques_archivo(lista_bloques);
	bool estado_corrompido = false;

	uint32_t bloque_invalido;
    void _es_valido_bloque(uint32_t* id_bloque)
    {
        if(!fs_block_manager_is_block_requested_and_valid(*id_bloque))
        {
            bloque_invalido = *id_bloque;
            estado_corrompido = true;

			U_LOG_INFO("Hubo un sabotaje en el archivo %s. Se agrego un bloque invalido.", this->NOMBRE_ARCHIVO);
        }
    }

    bool _borrar_bloque_invalido( uint32_t* id_bloque)
    {
        return bloque_invalido == *id_bloque;
    }

    list_iterate(lista_id_bloques, (void*)_es_valido_bloque);
    if(estado_corrompido)
	{
        list_remove_and_destroy_by_condition(lista_id_bloques,(void*)_borrar_bloque_invalido ,u_free);
	}

	char* md5_bloques_archivo = generate_md5(lista_id_bloques, tamanio_archivo, tamanio_bloques);

	if(strcmp(md5_bloques_archivo, hash_archivo))
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
		estado_corrompido = true;

		U_LOG_INFO("Hubo un sabotaje en el archivo %s. Se cambio el orden de los bloques.", this->NOMBRE_ARCHIVO);
		char* md5_recuperado = generate_md5(lista_id_bloques, tamanio_archivo, fs_blocks_manager_get_blocks_size());
		U_LOG_INFO("MD5 Anterior: %s | MD5 Actual: %s", hash_archivo, md5_recuperado);
		u_free(md5_recuperado);
	}

	char* string_cant_bloques = string_itoa(list_size(lista_id_bloques));
    config_set_value(config_actual, "BLOCK_COUNT",string_cant_bloques);

    char* string_lista = list_convert_to_string(lista_id_bloques);
    config_set_value(config_actual, "BLOCKS", string_lista);


    config_save(config_actual);

    list_destroy_and_destroy_elements(lista_id_bloques, free);
    free(md5_bloques_archivo);
    free(string_cant_bloques);
    free(string_lista);
    return estado_corrompido;
}
