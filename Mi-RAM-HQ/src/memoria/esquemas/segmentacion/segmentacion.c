#include "segmentacion.h"
#include <string.h>

private void segmentacion_inicializar_listado_segmentos(int tamanio_segmento_inicial);
private bool segmentacion_obtener_dos_segmentos(uint32_t pid,int tamanio_pcb,int tamanio_tareas);
private bool segmentacion_agregar_patota_en_estructuras(uint32_t inicio_segmento_pcb,int tamanio_pcb,uint32_t inicio_segmento_tareas,int tamanio_tareas,uint32_t pid,int indice_listado_segmentos_pcb,int indice_listado_segmentos_tareas);
private bool segmentacion_agregar_patota_en_memoria(uint32_t pid,const char* tareas,int tamanio_pcb,int tamanio_tareas);

void segmentacion_memoria_init(void)
{
    esquema_memoria_tamanio = u_config_get_int_value("TAMANIO_MEMORIA");
    esquema_memoria_mfisica = u_malloc(esquema_memoria_tamanio);
	listado_patotas = list_create();
	listado_segmentos = list_create();
	segmentacion_inicializar_listado_segmentos(esquema_memoria_tamanio);
}

bool segmentacion_memoria_inicializar_patota(uint32_t pid, uint32_t cant_tripulantes, const char* tareas)
{
    int tamanio_pcb = 2*(sizeof(uint32_t));
    int tamanio_tareas = strlen(tareas)+1;
	bool segmentos_creados = segmentacion_obtener_dos_segmentos(pid,tamanio_pcb,tamanio_tareas);
	if(!segmentos_creados){
    return false;
    }
	U_LOG_TRACE("Segmentos creados"); //Chequear
	return segmentacion_agregar_patota_en_memoria(pid, tareas, tamanio_pcb, tamanio_tareas);   /*  implementar funcion  */
}


bool segmentacion_memoria_inicializar_tripulante(uint32_t pid, uint32_t tid, u_pos_t pos)
{

}

bool segmentacion_memoria_actualizar_posicion_tripulante(uint32_t pid, uint32_t tid, u_pos_t pos)
{

}

bool segmentacion_memoria_actualizar_estado_tripulante(uint32_t pid, uint32_t tid, char estado)
{

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
	   p_segmento_t* segmento = u_malloc(sizeof(p_segmento_t));
       segmento->inicio_segmento=0;
       segmento->fin_segmento=tamanio_segmento_inicial - 1;
	   segmento->tipo_segmento=-1;
       segmento->id_propietario=-1;
       list_add(listado_segmentos, segmento);
}



private bool segmentacion_obtener_dos_segmentos(uint32_t pid,int tamanio_pcb,int tamanio_tareas){
       char* criterio = u_config_get_string_value("CRITERIO_SELECCION");

	   if(!strcmp(criterio, "FF")){
		   /* Logica CRITERIO FIRST FIT  */
           int i;
		   int indice_listado_segmentos_pcb;
		   int indice_listado_segmentos_tareas;
		   int segmento_excedido = u_config_get_int_value("TAMANIO_MEMORIA");
		   p_segmento_t *aux;
		   uint32_t inicio_segmento_pcb=segmento_excedido;
		   uint32_t inicio_segmento_tareas=segmento_excedido;
		   int tamanio_lista=list_size(listado_segmentos);

		   for(i=0; i<tamanio_lista; i++){
            aux = list_get(listado_segmentos, i);
			if(aux->id_propietario==-1){
				  if(((aux->fin_segmento - aux->inicio_segmento + 1)>= tamanio_pcb)& (aux->inicio_segmento<inicio_segmento_pcb)){
					  inicio_segmento_pcb=aux->inicio_segmento;
					  indice_listado_segmentos_pcb=i;
				 }
		   }

		   for(i=0; i<tamanio_lista; i++){
            aux = list_get(listado_segmentos, i);
			if(aux->id_propietario==-1){
				  if(((aux->fin_segmento - aux->inicio_segmento + 1)>= tamanio_tareas)& (aux->inicio_segmento<inicio_segmento_tareas)){
					  if(((aux->fin_segmento - aux->inicio_segmento + 1 - tamanio_pcb)>= tamanio_tareas)& (aux->inicio_segmento==inicio_segmento_pcb)){
						  inicio_segmento_tareas=inicio_segmento_pcb+tamanio_tareas;
						  indice_listado_segmentos_tareas=indice_listado_segmentos_pcb;
					  }
					  else{
					      inicio_segmento_tareas=aux->inicio_segmento;
						  indice_listado_segmentos_tareas=i;
					  }
				 }
		   }

		   if((inicio_segmento_pcb!=segmento_excedido)& (inicio_segmento_tareas!=segmento_excedido)){
                  return segmentacion_agregar_patota_en_estructuras(inicio_segmento_pcb,tamanio_pcb,inicio_segmento_tareas,tamanio_tareas,pid,indice_listado_segmentos_pcb,indice_listado_segmentos_tareas);
		   }

		   return false;

       }
	   /* Logica CRITERIO BEST FIT  */
           int i;
		   int indice_listado_segmentos_pcb;
		   int indice_listado_segmentos_tareas;
		   int tamanio_excedido = u_config_get_int_value("TAMANIO_MEMORIA");
		   p_segmento_t *aux;
		   uint32_t tamanio_segmento_pcb=tamanio_excedido+1;
		   uint32_t tamanio_segmento_tareas=tamanio_excedido+1;
		   uint32_t inicio_segmento_pcb=segmento_excedido;
		   uint32_t inicio_segmento_tareas=segmento_excedido;
		   int tamanio_lista=list_size(listado_segmentos);

		   for(i=0; i<tamanio_lista; i++){
            aux = list_get(listado_segmentos, i);
			if(aux->id_propietario==-1){
				  if(((aux->fin_segmento - aux->inicio_segmento + 1)>= tamanio_pcb)& ((aux->fin_segmento - aux->inicio_segmento + 1)<tamanio_segmento_pcb)){
					  inicio_segmento_pcb=aux->inicio_segmento;
					  indice_listado_segmentos_pcb=i;
					  tamanio_segmento_pcb=aux->fin_segmento - aux->inicio_segmento + 1;
				 }
		   }

		   for(i=0; i<tamanio_lista; i++){
            aux = list_get(listado_segmentos, i);
			if(aux->id_propietario==-1){
				  if(((aux->fin_segmento - aux->inicio_segmento + 1)>= tamanio_tareas)& ((aux->fin_segmento - aux->inicio_segmento + 1)<tamanio_segmento_tareas)){
					  if(((aux->fin_segmento - aux->inicio_segmento + 1 - tamanio_pcb)>= tamanio_tareas)& (aux->inicio_segmento==inicio_segmento_pcb)){
						  inicio_segmento_tareas=inicio_segmento_pcb+tamanio_tareas;
						  indice_listado_segmentos_tareas=indice_listado_segmentos_pcb;
						  tamanio_segmento_tareas=aux->fin_segmento - aux->inicio_segmento + 1 - tamanio_pcb;
					  }
					  else{
					      inicio_segmento_tareas=aux->inicio_segmento;
						  indice_listado_segmentos_tareas=i;
						  tamanio_segmento_tareas=aux->fin_segmento - aux->inicio_segmento + 1;
					  }
				 }
		   }

		   if((inicio_segmento_pcb!=segmento_excedido)& (inicio_segmento_tareas!=segmento_excedido)){
                  return segmentacion_agregar_patota_en_estructuras(inicio_segmento_pcb,tamanio_pcb,inicio_segmento_tareas,tamanio_tareas,pid,indice_listado_segmentos_pcb,indice_listado_segmentos_tareas);
		   }

		   return false;
}


private bool segmentacion_agregar_patota_en_estructuras(uint32_t inicio_segmento_pcb,int tamanio_pcb,uint32_t inicio_segmento_tareas,int tamanio_tareas,uint32_t pid,int indice_listado_segmentos_pcb,int indice_listado_segmentos_tareas){

	    p_segmento_t *segmento_pcb=u_malloc(sizeof(p_segmento_t));
		p_segmento_t *segmento_tareas=u_malloc(sizeof(p_segmento_t));
		t_list* lista_de_tripulantes=list_create();
		p_segmento_patota_t *segmento_patota=u_malloc(sizeof(p_segmento_patota_t));
		t_list* tabla_segmentos_patota=list_create();
		p_patota_y_tabla_t *patota_y_tabla=u_malloc(sizeof(p_patota_y_tabla_t));

		segmento_pcb->inicio_segmento=inicio_segmento_pcb;
		segmento_pcb->fin_segmento=inicio_segmento_pcb + tamanio_pcb - 1;
		segmento_pcb->tipo_segmento=0;
		segmento_pcb->id_propietario=pid;

		segmento_tareas->inicio_segmento=inicio_segmento_tareas;
		segmento_tareas->fin_segmento=inicio_segmento_tareas + tamanio_tareas - 1;
		segmento_tareas->tipo_segmento=1;
		segmento_tareas->id_propietario=pid;

		p_segmento_t *segmento_modificado_pcb=list_replace(listado_segmentos, indice_listado_segmentos_pcb, segmento_pcb); /* chequear si se puede usar otra funcion para reemplazar el valor */
		if(indice_listado_segmentos_tareas==indice_listado_segmentos_pcb){
			list_add(listado_segmentos, segmento_tareas);
		}
		else{
			p_segmento_t *segmento_modificado_tareas=list_replace(listado_segmentos, indice_listado_segmentos_tareas, segmento_tareas); /* chequear si se puede usar otra funcion para reemplazar el valor */
		}

		segmento_patota->inicio_segmento_pcb=inicio_segmento_pcb;
		segmento_patota->tamanio_segmento_pcb=tamanio_pcb;
		segmento_patota->inicio_segmento_tareas=inicio_segmento_tareas;
		segmento_patota->tamanio_segmento_tareas=tamanio_tareas;
		segmento_patota->listado_tripulantes=lista_de_tripulantes;

		list_add(tabla_segmentos_patota, segmento_patota);

		patota_y_tabla->pid=pid;
		patota_y_tabla->tabla_segmentos=tabla_segmentos_patota;

		list_add(tabla_segmentos_patota, segmento_patota);
		return true;
}



private bool segmentacion_agregar_patota_en_memoria(uint32_t pid,const char* tareas,int tamanio_pcb,int tamanio_tareas){

}
