#include "bitacora.h"
#include "fs/blocks/blocks_manager.h"
#include <commons/config.h>

struct fs_bitacora_t{
    uint32_t    SIZE;
    t_queue*    BLOCKS;
    uint32_t	TID;
};


fs_bitacora_t* fs_bitacora_create(const char* mount_point, uint32_t tid){

	char * path = string_from_format("%s/Files/Bitacoras/Tripulante%d",mount_point,tid);

	FILE* file = fopen(path, "w");

		fs_bitacora_t* this = u_malloc(sizeof(fs_bitacora_t));

		this = config_create(path);

		config_set_value(this, "SIZE", "0");

		uint32_t block_id = fs_blocks_manager_request_block();
		char* block_list = string_from_format("[%d]", block_id);

		config_set_value(this, "BLOCKS", block_list);

    fclose(file);

    u_free(path);

    return this;
}

//Elimina la estructura de la bitacora y libera los bloques que tenga asignados.
void fs_bitacora_delete(fs_bitacora_t* this){
	for(int i=0; i<this->BLOCKS->elements->elements_count ; i++){
		fs_blocks_manager_release_block(this->BLOCKS[i]);
	}
    u_free(this);
}

void fs_bitacora_add_content(fs_bitacora_t* this, const char* content){
    //Se debe guardar el contenido de "content" en la bitacora. Para esto
    // se debe intentar escribir el contenido al final del ultimo bloque disponible.
    // En caso de no poder escribirse todo el contenido, pedir un nuevo bloque y escribir
    // la informacion restante en el nuevo bloque. En caso de que tampoco se pueda escribir
    // todo el contenido en el nuevo bloque, seguir pidiendo bloqueas hasta terminar de escribir
    // todo el contenido.

	//obtengo la cant de bloques de la bitacora
	uint32_t block_id = this->BLOCKS->elements->elements_count;

	//obtengo el offset del ultimo bloque de la lista
	int offset =

	//intento escribir el content en el bloque
	int escritos = fs_block_write(block_id, content, sizeof(content), offset);

	if(escritos!=sizeof(content)){

		//pido un nuevo bloque
		list_add(this->BLOCKS,fs_blocks_manager_request_block());
		block_id++;

		escritos = fs_block_write(block_id, content, sizeof(content), 0);

		while(escritos!=sizeof(content)){
			escritos += fs_block_write(block_id, content, sizeof(content)-escritos, offset); //aca deberia solo tomar el content menos el ya guardado
		}
	}

	free(offset);

}

bool fs_bitacora_check_integrity(fs_bitacora_t* this){
    //TODO: Se debe devolver true o false dependiendo si la bitacora esta corrompida o no.
    // El criterio que se va a seguir para saber si una bitacora esta corrompida o no, se
    // especifica en el enunciado del TP.


    return false;
}

uint32_t fs_bitacora_get_tid(const fs_bitacora_t* this){
    //TODO: Devolver el TID del tripulante.
	uint32_t tid = NULL;

    return tid;

	/*
    fs_bitacora_t* bitacora = NULL;

    char bitacora_key[64] = { 0 };
    sprintf(bitacora_key, "Tripulante%d", tid);

    pthread_mutex_lock(&p_bitacoras_manager_instance->bitacoras_mx);
    fs_bitacora_ref_t* bitacora_ref = dictionary_get(p_bitacoras_manager_instance->bitacoras, (char*)bitacora_key);

    if(bitacora_ref)
    {
        pthread_mutex_lock(&bitacora_ref->ref_counter_mx);
        bitacora_ref->ref_counter ++;
        pthread_mutex_unlock(&bitacora_ref->ref_counter_mx);

        bitacora = bitacora_ref->bitacora;
    }

    pthread_mutex_unlock(&p_bitacoras_manager_instance->bitacoras_mx);

    return bitacora;
	*/
}

uint32_t fs_bitacora_get_size(const fs_bitacora_t* this){
    return this->SIZE;
}

uint32_t fs_bitacora_get_block_count(const fs_bitacora_t* this){
    return sizeof(this->BLOCKS);
}

char* fs_bitacora_get_content(const fs_bitacora_t* this){
    //TODO: Devolver el contenido de la bitacora. Leyendo los bloques de la bitacora.

	for(int i=0; i<this->BLOCKS->elements->elements_count ; i++){
		fs_block_read(i+1, , , fs_block_get_disk_offset(i+1);
	}
    return NULL;
}

