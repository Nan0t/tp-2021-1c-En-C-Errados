#include "bitacora.h"
#include "fs/blocks/blocks_manager.h"
#include <commons/config.h>
#include <stdlib.h>
#include <stdio.h>

struct fs_bitacora_t{
    uint32_t	TID;
    t_config* 	CONFIG;
    char *		PATH;
};

private t_list* lista_id_bloques_bitacora(char** lista_bloques);
private char* list_convert_to_string(t_list* list);
private int get_offset(fs_bitacora_t* this);


fs_bitacora_t* fs_bitacora_create(const char* path, uint32_t tid)
{
	fs_bitacora_t* this = u_malloc(sizeof(fs_bitacora_t));

	this->TID    = tid;
	this->CONFIG = config_create((char*)path);
	this->PATH   = strdup(path);

	if(tid != 0)
	{
		config_set_value(this->CONFIG, "SIZE", "0");

		uint32_t block_id = fs_blocks_manager_request_block();
		char* block_list = string_from_format("[%d]", block_id);

		config_set_value(this->CONFIG, "BLOCKS", block_list);
		config_save(this->CONFIG);

		u_free(block_list);
	}


    return this;
}

//Elimina la estructura de la bitacora y libera los bloques que tenga asignados.
void fs_bitacora_delete(fs_bitacora_t* this){

	char** block_list = config_get_array_value(this->CONFIG, "BLOCKS");
	t_list* tblock = lista_id_bloques_bitacora(block_list);

	void _releasa_blocks(uint32_t* block_id) {
		fs_blocks_manager_release_block(*block_id);
		u_free(block_id);
	}

	list_destroy_and_destroy_elements(tblock, (void*)_releasa_blocks);
	config_destroy(this->CONFIG);

	remove(this->PATH);

	u_free(this->PATH);
    u_free(this);
}

//Se guarda el contenido de "content" en la bitacora. Para esto
// se debe intentar escribir el contenido al final del ultimo bloque disponible.
// En caso de no poder escribirse todo el contenido, pedir un nuevo bloque y escribir
// la informacion restante en el nuevo bloque. En caso de que tampoco se pueda escribir
// todo el contenido en el nuevo bloque, seguir pidiendo bloqueas hasta terminar de escribir
// todo el contenido.
void fs_bitacora_add_content(fs_bitacora_t* this, const char* content){
	char** blocks = config_get_array_value(this->CONFIG, "BLOCKS");
	t_list* blocks_tlist = lista_id_bloques_bitacora(blocks);
	
	uint32_t* last_block = list_get(blocks_tlist, list_size(blocks_tlist) - 1);

	U_LOG_TRACE("Se actualiza la bitacora del tripulante %d: %s", this->TID, content);

	int offset = get_offset(this);

	if(!offset && fs_bitacora_get_size(this))
	{
		last_block = u_malloc(sizeof(uint32_t));
		*last_block = fs_blocks_manager_request_block();
		list_add(blocks_tlist, last_block);
	}

	uint32_t cantidad_a_escribir = strlen(content);
	uint32_t escritos;
	uint32_t cantidad_escrito = 0;
	escritos = fs_block_write(*last_block, content, strlen(content), offset);
	cantidad_escrito += escritos;
	cantidad_a_escribir -= escritos;
	while(cantidad_a_escribir != 0){
		uint32_t* id_new_block = malloc(sizeof(uint32_t));
		*id_new_block = fs_blocks_manager_request_block();
		list_add(blocks_tlist, id_new_block);

		escritos = fs_block_write(*id_new_block, content + cantidad_escrito, cantidad_a_escribir, 0);
		cantidad_escrito += escritos;
		cantidad_a_escribir -= escritos;
	}
	char* string_de_bloques = list_convert_to_string(blocks_tlist);
	char* size = string_itoa(strlen(content) + fs_bitacora_get_size(this));

	config_set_value(this->CONFIG, "BLOCKS", string_de_bloques);
	config_set_value(this->CONFIG, "SIZE", size);
	config_save(this->CONFIG);

	u_free(size);
	u_free(string_de_bloques);

	list_destroy_and_destroy_elements(blocks_tlist, free);
}


uint32_t fs_bitacora_get_tid(const fs_bitacora_t* this){
    return this->TID;
}

uint32_t fs_bitacora_get_size(const fs_bitacora_t* this){
    return config_get_int_value(this->CONFIG, "SIZE");
}


t_list* fs_bitacora_get_blocks(const fs_bitacora_t* this)
{
	char** blocks = config_get_array_value(this->CONFIG, "BLOCKS");
	return lista_id_bloques_bitacora(blocks);
}

//Devuelve contenido de la bitácora leyendo los bloques de la bitácora.
char* fs_bitacora_get_content(const fs_bitacora_t* this){ //devuelve un string enorme basicamente
	char** blocks = config_get_array_value(this->CONFIG, "BLOCKS");
	uint32_t tamanio_bitacora = fs_bitacora_get_size(this);
	t_list* lista_id_bloques = lista_id_bloques_bitacora(blocks);
	char* bitacora = malloc(tamanio_bitacora + 1);
	uint32_t bytes_leidos = 0;
	void _append_from_log(uint32_t* block_id)
	{
		bytes_leidos += fs_block_read(*block_id, bitacora + bytes_leidos, tamanio_bitacora - bytes_leidos, 0);
	}
	list_iterate(lista_id_bloques, (void*)_append_from_log);
	list_destroy_and_destroy_elements(lista_id_bloques, free);
	bitacora[tamanio_bitacora] = '\0';
	
    return bitacora;
}

private int get_offset(fs_bitacora_t* this){
	return fs_bitacora_get_size(this) % fs_blocks_manager_get_blocks_size();
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

private t_list* lista_id_bloques_bitacora(char** lista_bloques)
{
	t_list* bloques_bitacora = list_create();
	for(char** bloques = lista_bloques; *bloques != NULL;  bloques ++)
    {
        uint32_t* bloque = u_malloc(sizeof(uint32_t));
        *bloque = atoi(*bloques);
        list_add(bloques_bitacora, bloque);
        free(*bloques);
    }
	free(lista_bloques);

	return bloques_bitacora;
}