#include "segmentacion.h"

private void segmentacion_inicializar_listado_segmentos(int tamanio_segmento_inicial);
private void segmentacion_obtener_segmentos(uint32_t pid,int tamanio_pcb,int tamanio_tareas,uint32_t cant_tripulantes);
private bool segmentacion_hay_segmento_libre(int tamanio_lista,int tamanio_segmento);
private void segmentacion_compactar(void);
private int segmentacion_buscar_segmento(int tamanio_lista,int tamanio_segmento);
private bool segmentacion_agregar_patota_en_memoria(uint32_t pid,const char* tareas);

void segmentacion_memoria_init(void)
{
    esquema_memoria_tamanio = u_config_get_int_value("TAMANIO_MEMORIA");
    esquema_memoria_mfisica = u_malloc(esquema_memoria_tamanio);
    espacio_libre = esquema_memoria_tamanio;
	listado_patotas = list_create();
	listado_segmentos = list_create();
	segmentacion_inicializar_listado_segmentos(esquema_memoria_tamanio);
}

bool segmentacion_memoria_inicializar_patota(uint32_t pid, uint32_t cant_tripulantes, const char* tareas)
{
	int tamanio_data = 2*(sizeof(uint32_t)) + 21* cant_tripulantes + strlen(tareas)+1;
	if(tamanio_data > espacio_libre){
	    return false;
	}
	int tamanio_pcb = 2*(sizeof(uint32_t));
	int tamanio_tareas = strlen(tareas)+1;
	segmentacion_obtener_segmentos(pid,tamanio_pcb,tamanio_tareas,cant_tripulantes);

	U_LOG_TRACE("Segmentos creados");
	return segmentacion_agregar_patota_en_memoria(pid, tareas);
}


bool segmentacion_memoria_inicializar_tripulante(uint32_t pid, uint32_t tid, u_pos_t pos)
{
	int tamanio_lista_patotas=list_size(listado_patotas);
	int j;
	int indice=-1;
	s_patota_y_tabla_t *aux;
	for(j=0; j<tamanio_lista_patotas; j++){
        aux = list_get(listado_patotas, j);
		if(aux->pid==pid){
				indice=j;
	    }
	}
	aux = list_get(listado_patotas, indice);
	t_list* tabla;
	tabla = list_create();
	tabla = aux->tabla_segmentos;
	s_segmento_patota_t *aux2;
	aux2 = list_get(tabla, 0);
	t_list* tabla_tripulantes = list_create();
	tabla_tripulantes = aux2->listado_tripulantes;
	int tamanio_lista_tripulantes=list_size(tabla_tripulantes);
	int indice_tripulante=tamanio_lista_tripulantes;
	int i;
	s_segmento_tripulante_t *aux3;
	for(i=0; i<tamanio_lista_tripulantes; i++){
	         aux3 = list_get(tabla_tripulantes, i);
			 if(aux3->tid==-1){
					  if(i < indice_tripulante){
						  indice_tripulante=i;
					  }
			 }
	}
	aux3 = list_get(tabla_tripulantes, indice_tripulante);
	aux3->tid=tid;
	int desplazamiento = aux3->inicio_segmento_tcb;
	char estado = 'N';
	memcpy(esquema_memoria_mfisica + desplazamiento, &tid, sizeof(uint32_t));
	desplazamiento = desplazamiento + sizeof(uint32_t);
	memcpy(esquema_memoria_mfisica + desplazamiento, &estado, sizeof(char));
	desplazamiento = desplazamiento + sizeof(char);
	uint32_t identificador_nueva_tarea = 0;
	uint32_t puntero_pcb = 0;
	uint32_t posiciones_y_puntero[4] = {pos.x, pos.y, identificador_nueva_tarea, puntero_pcb};
	int k;
    for(k=0; k<4; k++){
        memcpy(esquema_memoria_mfisica + desplazamiento, &posiciones_y_puntero[k], sizeof(uint32_t));
        desplazamiento = desplazamiento + sizeof(uint32_t);
    }
    U_LOG_TRACE("Tripulante con tid %d guardado correctamente", tid);
    return true;
}

bool segmentacion_memoria_actualizar_posicion_tripulante(uint32_t pid, uint32_t tid, u_pos_t pos)
{
	int tamanio_lista_patotas=list_size(listado_patotas);
	int j;
	int indice=-1;
	s_patota_y_tabla_t *aux;
	for(j=0; j<tamanio_lista_patotas; j++){
        aux = list_get(listado_patotas, j);
		if(aux->pid==pid){
				indice=j;
	    }
	}
	aux = list_get(listado_patotas, indice);
	t_list* tabla;
	tabla = list_create();
	tabla = aux->tabla_segmentos;
	s_segmento_patota_t *aux2;
	aux2 = list_get(tabla, 0);
	t_list* tabla_tripulantes = list_create();
	tabla_tripulantes = aux2->listado_tripulantes;
	int tamanio_lista_tripulantes=list_size(tabla_tripulantes);
	int indice_tripulante=-1;
	int i;
	s_segmento_tripulante_t *aux3;
	for(i=0; i<tamanio_lista_tripulantes; i++){
	         aux3 = list_get(tabla_tripulantes, i);
			 if(aux3->tid==tid){
				  indice_tripulante=i;
			 }
	}
	aux3 = list_get(tabla_tripulantes, indice_tripulante);
	memcpy(esquema_memoria_mfisica + aux3->inicio_segmento_tcb + sizeof(uint32_t)+ sizeof(char), &(pos.x), sizeof(uint32_t));
	memcpy(esquema_memoria_mfisica + aux3->inicio_segmento_tcb + 2*sizeof(uint32_t)+ sizeof(char), &(pos.y), sizeof(uint32_t));
	return true;
}

bool segmentacion_memoria_actualizar_estado_tripulante(uint32_t pid, uint32_t tid, char estado)
{
	int tamanio_lista_patotas=list_size(listado_patotas);
	int j;
	int indice=-1;
	s_patota_y_tabla_t *aux;
	for(j=0; j<tamanio_lista_patotas; j++){
        aux = list_get(listado_patotas, j);
		if(aux->pid==pid){
				indice=j;
	    }
	}
	aux = list_get(listado_patotas, indice);
	t_list* tabla;
	tabla = list_create();
	tabla = aux->tabla_segmentos;
	s_segmento_patota_t *aux2;
	aux2 = list_get(tabla, 0);
	t_list* tabla_tripulantes = list_create();
	tabla_tripulantes = aux2->listado_tripulantes;
	int tamanio_lista_tripulantes=list_size(tabla_tripulantes);
	int indice_tripulante=-1;
	int i;
	s_segmento_tripulante_t *aux3;
	for(i=0; i<tamanio_lista_tripulantes; i++){
	         aux3 = list_get(tabla_tripulantes, i);
			 if(aux3->tid==tid){
				  indice_tripulante=i;
			 }
	}
	aux3 = list_get(tabla_tripulantes, indice_tripulante);
	memcpy(esquema_memoria_mfisica + aux3->inicio_segmento_tcb + sizeof(uint32_t), &estado, sizeof(char));
	return true;
}


char* segmentacion_memoria_obtener_proxima_tarea(uint32_t pid, uint32_t tid)
{

}

tripulantes_t* segmentacion_memoria_obtener_info_tripulante(uint32_t pid, uint32_t tid)
{

}

t_list* segmentacion_memoria_obtener_todos_los_tripulantes(void)
{

}

bool segmentacion_memoria_expulsar_tripulante(uint32_t pid, uint32_t tid)
{

}


//---------------------------------------------------------------------------------------

private void segmentacion_inicializar_listado_segmentos(int tamanio_segmento_inicial){
	   s_segmento_t* segmento = u_malloc(sizeof(s_segmento_t));
       segmento->inicio_segmento=0;
       segmento->tamanio_segmento=tamanio_segmento_inicial;
	   segmento->tipo_segmento=-1;
       segmento->id_propietario=-1;
       list_add(listado_segmentos, segmento);
}

private void segmentacion_obtener_segmentos(uint32_t pid,int tamanio_pcb,int tamanio_tareas,uint32_t cant_tripulantes){
	int tamanio_lista;
	int i;
	int incio_segmento_tripulante;
	t_list* lista_de_tripulantes;
	t_list* lista_tabla_de_segmento;
	lista_de_tripulantes = list_create();
	lista_tabla_de_segmento = list_create();

	for(i=0; i<cant_tripulantes; i++){
		s_segmento_tripulante_t* segmento_tripulante = u_malloc(sizeof(s_segmento_tripulante_t));
		tamanio_lista=list_size(listado_segmentos);
		if(!segmentacion_hay_segmento_libre(tamanio_lista,21)){
			segmentacion_compactar();
		}
		incio_segmento_tripulante = segmentacion_buscar_segmento(tamanio_lista,21);
		segmento_tripulante->tid=-1;
		segmento_tripulante->inicio_segmento_tcb=incio_segmento_tripulante;
		segmento_tripulante->tamanio_segmento_tcb=21;
		list_add(lista_de_tripulantes, segmento_tripulante);
	}

	tamanio_lista=list_size(listado_segmentos);
	if(!segmentacion_hay_segmento_libre(tamanio_lista,tamanio_tareas)){
		segmentacion_compactar();
	}
	int incio_segmento_tareas = segmentacion_buscar_segmento(tamanio_lista,tamanio_tareas);

	tamanio_lista=list_size(listado_segmentos);
	if(!segmentacion_hay_segmento_libre(tamanio_lista,tamanio_pcb)){
		segmentacion_compactar();
	}
	int incio_segmento_pcb = segmentacion_buscar_segmento(tamanio_lista,tamanio_pcb);

	s_segmento_patota_t* segmento_patota = u_malloc(sizeof(s_segmento_patota_t));
	segmento_patota->inicio_segmento_pcb=incio_segmento_pcb;
	segmento_patota->tamanio_segmento_pcb=tamanio_pcb;
	segmento_patota->inicio_segmento_tareas=incio_segmento_tareas;
	segmento_patota->tamanio_segmento_tareas=tamanio_tareas;
	segmento_patota->listado_tripulantes=lista_de_tripulantes;

	list_add(lista_tabla_de_segmento, segmento_patota);

	s_patota_y_tabla_t* segmento_patota_y_tabla = u_malloc(sizeof(s_patota_y_tabla_t));
	segmento_patota_y_tabla->pid=pid;
	segmento_patota_y_tabla->tabla_segmentos=lista_tabla_de_segmento;

	list_add(listado_patotas, segmento_patota_y_tabla);
	espacio_libre = espacio_libre-tamanio_pcb-tamanio_tareas-(21*cant_tripulantes);
}

private bool segmentacion_hay_segmento_libre(int tamanio_lista,int tamanio_segmento){
	s_segmento_t *aux;
	int i;
	int encontro=0;
	for(i=0; i<tamanio_lista; i++){
	         aux = list_get(listado_segmentos, i);
			 if(aux->id_propietario==-1){
					  if(aux->tamanio_segmento >= tamanio_segmento){
						  encontro=1;
					  }
			 }
	}
    if(encontro==1){
    	return true;
    }
	return false;
}

private void segmentacion_compactar(void){

}

private int segmentacion_buscar_segmento(int tamanio_lista,int tamanio_segmento){
	char* criterio = u_config_get_string_value("CRITERIO_SELECCION");
	int inicio_segmento;
	int indice_segmento_encontrado;
	int tamanio_segmento_aux;
	s_segmento_t *aux;

	/* Logica CRITERIO FIRST FIT  */
	if(!strcmp(criterio, "FF")){
	   uint32_t inicio_actual = u_config_get_int_value("TAMANIO_MEMORIA");
	   int i;
	   for(i=0; i<tamanio_lista; i++){
         aux = list_get(listado_segmentos, i);
		 if(aux->id_propietario==-1){
			  if((aux->tamanio_segmento>=tamanio_segmento)&& (aux->inicio_segmento<inicio_actual)){
				  indice_segmento_encontrado=i;
				  inicio_segmento=aux->inicio_segmento;
				  inicio_actual=aux->inicio_segmento;
				  tamanio_segmento_aux=aux->tamanio_segmento;
			  }
	     }
	   }
	}
	/* Logica CRITERIO BEST FIT  */
	else{
	    uint32_t tamanio_actual = u_config_get_int_value("TAMANIO_MEMORIA");
	    tamanio_actual=tamanio_actual+1;
		int i;
		for(i=0; i<tamanio_lista; i++){
	        aux = list_get(listado_segmentos, i);
			if(aux->id_propietario==-1){
				if((aux->tamanio_segmento>=tamanio_segmento)&& (aux->tamanio_segmento<tamanio_actual)){
					indice_segmento_encontrado=i;
					inicio_segmento=aux->inicio_segmento;
					tamanio_actual=aux->tamanio_segmento;
					tamanio_segmento_aux=aux->tamanio_segmento;
				}
		    }
		}
	}
	/* Actualiza Lista de segmentos  */
	aux = list_get(listado_segmentos, indice_segmento_encontrado);
	if(!(aux->tamanio_segmento==tamanio_segmento)){
	      aux->tamanio_segmento=tamanio_segmento;
	      s_segmento_t* segmento = u_malloc(sizeof(s_segmento_t));
	      segmento->inicio_segmento=inicio_segmento+tamanio_segmento;
	      segmento->tamanio_segmento=tamanio_segmento_aux-tamanio_segmento;
	      segmento->tipo_segmento=-1;
	      segmento->id_propietario=-1;
	      list_add(listado_segmentos, segmento);
	}
	return inicio_segmento;
}

private bool segmentacion_agregar_patota_en_memoria(uint32_t pid,const char* tareas){
	uint32_t pid_y_direccion_tareas[2] = {pid, 0};
	int tamanio_lista=list_size(listado_patotas);
	int j;
	int indice=-1;
	s_patota_y_tabla_t *aux;
	for(j=0; j<tamanio_lista; j++){
        aux = list_get(listado_patotas, j);
		if(aux->pid==pid){
				indice=j;
	    }
	}
	aux = list_get(listado_patotas, indice);
	t_list* tabla;
	tabla = list_create();
	tabla = aux->tabla_segmentos;
	s_segmento_patota_t *aux2;
	aux2 = list_get(tabla, 0);
	int offset_general = aux2->inicio_segmento_pcb;
	int i;
	for(i = 0; i<2; i++){
	        memcpy(esquema_memoria_mfisica + offset_general, &pid_y_direccion_tareas[i], sizeof(uint32_t));
	        offset_general = offset_general + sizeof(uint32_t);
	}
	offset_general = aux2->inicio_segmento_tareas;
	int offset_tareas = 0;
	while(offset_tareas < strlen(tareas)+1){
		    memcpy(esquema_memoria_mfisica + offset_general, &tareas[offset_tareas], sizeof(char));
            offset_tareas++;
            offset_general++;
            U_LOG_TRACE("Escrito caracter: %d", i);
	}
	return true;
}
