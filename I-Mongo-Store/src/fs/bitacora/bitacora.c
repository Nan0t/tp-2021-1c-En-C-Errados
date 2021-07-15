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


fs_bitacora_t* fs_bitacora_create(const char* mount_point, uint32_t tid){

	char * path = string_from_format("%s/Files/Bitacoras/Tripulante%d",mount_point,tid);

	FILE* file = fopen(path, "w");

		fs_bitacora_t* this = u_malloc(sizeof(fs_bitacora_t));

		this = config_create(path);

		this->PATH = path;

		config_set_value(this->CONFIG, "SIZE", "0");

		uint32_t block_id = fs_blocks_manager_request_block();
		char* block_list = string_from_format("[%d]", block_id);

		config_set_value(this->CONFIG, "BLOCKS", block_list);

    fclose(file);

    u_free(path);

    return this;
}

//Elimina la estructura de la bitacora y libera los bloques que tenga asignados.
void fs_bitacora_delete(fs_bitacora_t* this){

	char** block_list = config_get_array_value(this->CONFIG, "BLOCKS");
	for(int i=0; i<config_get_int_value(this->CONFIG, "BLOCKS_COUNT") ; i++){
		fs_blocks_manager_release_block(block_list[i]);
	}

    u_free(block_list);
    u_free(this);

}

//Se guarda el contenido de "content" en la bitacora. Para esto
// se debe intentar escribir el contenido al final del ultimo bloque disponible.
// En caso de no poder escribirse todo el contenido, pedir un nuevo bloque y escribir
// la informacion restante en el nuevo bloque. En caso de que tampoco se pueda escribir
// todo el contenido en el nuevo bloque, seguir pidiendo bloqueas hasta terminar de escribir
// todo el contenido.
void fs_bitacora_add_content(fs_bitacora_t* this, const char* content){
	int amount_values = config_get_int_value(this->CONFIG, "BLOCK_COUNT");
	char** blocks = config_get_array_value(this->CONFIG, "BLOCKS");
	t_list* blocks_tlist = lista_id_bloques_archivo(blocks);
	char* last_block = blocks_tlist->elements_count - 1;

	//uint32_t block_id =  atoi(values[amount_values-1]); se hace por t_list en vez de strings

	int offset = config_get_array_value(this->CONFIG, "SIZE") % fs_blocks_manager_get_blocks_size();

	int escritos  = u_malloc(sizeof(int));
	escritos = fs_block_write(last_block, content, sizeof(content), offset);

	if(escritos!=sizeof(content)){

		list_add(blocks_tlist,fs_blocks_manager_request_block());
		last_block++;

		escritos = fs_block_write(last_block, content, sizeof(content), 0);

		//si no entró sigo pidiendo otros y guardando
		while(escritos!=sizeof(content)){
			list_add(blocks_tlist,fs_blocks_manager_request_block());
			last_block++;

			escritos += fs_block_write(last_block, content, sizeof(content)-escritos, get_offset(this));
		}

		//actualizo config blocks, guardo tlist en string config
		config_set_value(this->CONFIG, "BLOCKS", list_convert_to_string(blocks_tlist));

	}
	u_free(content);
	u_free(escritos);
	u_free(last_block);
	u_free(blocks);
	u_free(blocks_tlist);
	u_free(amount_values);
}


uint32_t fs_bitacora_get_tid(const fs_bitacora_t* this){
    return this->TID;
}

uint32_t fs_bitacora_get_size(const fs_bitacora_t* this){
    return config_get_int_value(this, "SIZE");
}

uint32_t fs_bitacora_get_block_count(const fs_bitacora_t* this){
    return config_get_int_value(this, "BLOCK_COUNT");
}


//Devuelve contenido de la bitácora leyendo los bloques de la bitácora.
char* fs_bitacora_get_content(const fs_bitacora_t* this){ //devuelve un string enorme basicamente

	FILE* file = fopen(this->PATH, "r");
	char** block_list = config_get_array_value(this->CONFIG, "BLOCKS");

	for(int i=0; i<config_get_int_value(this->CONFIG, "BLOCKS_COUNT"); i++){ //cambiar por longitud del array en vez de buscar en config?
		fs_block_read(block_list[i],block_list->, uint64_t data_size,0);

	}
	fclose(file);
    return NULL;
    // usar realloc porque voy cambiando el tamaño
    //como en files, probar igual
}

private int get_offset(fs_file_t* this){
	return config_get_array_value(this->CONFIG, "SIZE") % fs_blocks_manager_get_blocks_size();
}
