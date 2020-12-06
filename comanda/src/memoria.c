/*
 * memoria.c
 *
 *  Created on: 13 sep. 2020
 *      Author: utnso
 */

#include "memoria.h"


void iniciar_memoria(){

	log_info(logger,"LEYENDO TAMAÑO DE MEMORIA");
	size_memoria_principal=config_get_int_value(config,"TAMANIO_MEMORIA");
	size_memoria_swap=     config_get_int_value(config,"TAMANIO_SWAP")   ;
	//size_minimo_block=config_get_int_value(config,"TAMANO_MINIMO_PARTICION");
	//algoritmo_memoria=config_get_string_value(config,"ALGORITMO_MEMORIA");
	algoritmo_remplazo=config_get_string_value(config,"ALGORITMO_REEMPLAZO");
	//algoritmo_particion_libre=config_get_string_value(config,"ALGORITMO_PARTICION_LIBRE");
	//frecuencia_compactacion=config_get_int_value(config,"FRECUENCIA_COMPACTACION");

	p_inicio_memoria_principal= reservar_memoria_inicial(size_memoria_principal); //----------------CREO LA MEMORIA PRINCIPAL Y LISTA DE PUNTEROS
	p_fin_memoria_principal=p_inicio_memoria_principal+size_memoria_principal;
	log_info(logger,"Los limites de memoria principal son: %p  y  %p",p_inicio_memoria_principal,p_fin_memoria_principal);

	p_inicio_memoria_swap= reservar_memoria_inicial(size_memoria_swap); //----------------CREO LA MEMORIA SWAP Y LISTA DE PUNTEROS
	p_fin_memoria_swap=p_inicio_memoria_swap+size_memoria_swap;
	log_info(logger,"Los limites de memoria Swap son: %p  y  %p",p_inicio_memoria_swap,p_fin_memoria_swap);

	puntero_clock=0;

	log_info(logger,"El tamaño de la memoria principal es: %d", size_memoria_principal);
	log_info(logger,"El tamaño de la memoria Swap es: %d", size_memoria_swap);
	log_info(logger,"El algoritmo de remplazo utilizado por la memoria es: %s",algoritmo_remplazo);


	//list_frames_memory_principal=list_create();
	//list_frames_memory_swap     =list_create();

	tabla_frames_libres=list_create();
	tabla_frames_libres_swap=list_create();

	crear_paginas_memoria(tabla_frames_libres,size_memoria_principal,p_inicio_memoria_principal);  //Creo los frame de c/memoria y los agrego a su tabla de frames libres
	crear_paginas_memoria(tabla_frames_libres_swap,size_memoria_swap, p_inicio_memoria_swap);

}

void * reservar_memoria_inicial(int size_memoria_total){

	void * p_memoria_total=malloc(size_memoria_total);

	memset(p_memoria_total, 0, size_memoria_total);

	return p_memoria_total;

}

void crear_paginas_memoria(t_list * tabla_frames,int size_memoria, void * p_inicio_memoria){

	int cantidad_paginas=size_memoria/SIZE_PAGINA;

	for (int i=0;i<cantidad_paginas;i++){

		t_frame * frame=malloc(sizeof(t_frame));
		frame->nro_frame=i;
		if(i==0)
			frame->direccion_frame=p_inicio_memoria;
		else
			frame->direccion_frame=p_inicio_memoria+(i*SIZE_PAGINA);

		//log_info(logger,"La direccion de memoria del frame es %p y la logica es %d",frame->direccion_frame,frame->direccion_frame-p_inicio_memoria_principal);

		//list_add(list_frames_memory,frame);
		list_add(tabla_frames,frame);
	}
	char * nombre=string_new();

	if(tabla_frames==tabla_frames_libres){
		string_append(&nombre,"Principal");
	    cantidad_frames_memoria_principal=cantidad_paginas;}
	else
		string_append(&nombre,"Swap");

	log_info(logger,"Se dividio la memoria %s en %d  frames",nombre,cantidad_paginas);
	log_info(logger,"Hay %d  marcos libres en la memoria %s",list_size(tabla_frames),nombre);


}


t_restaurante * crear_tabla_segmentos_restaurante(char * nombre_restaurante  ){

	t_restaurante * restaurante=malloc(sizeof(t_restaurante));

	restaurante->nombre_restaurante=nombre_restaurante;
	restaurante->tabla_pedidos=list_create();

	list_add(lista_restarurantes,restaurante);

	log_info(logger,"Se creo La tabla de segmentos del nuevo restaurante. \n");

	return restaurante;
}

bool agregar_pedido_a_tabla_segmentos(t_restaurante * restaurante, uint32_t id_pedido){

		bool buscar_pedido(void * elemento){
			t_pedido_seg * pedido=(t_pedido_seg*)elemento;
			if(id_pedido==pedido->id_pedido)
				return TRUE;
			return FALSE;
		}

	t_pedido_seg * pedido=NULL;

	//printf("Se recibio el pedido nro: %d\n",id_pedido);
	//printf("Se recibio restaurant: %s\n",restaurante->nombre_restaurante);

	pedido=list_find(restaurante->tabla_pedidos,buscar_pedido);

	if(pedido!=NULL){
		log_info(logger,"El pedido que se quiere guardar ya existe\n");
		return FALSE;
	   }

	pedido=malloc(sizeof(t_pedido_seg));

	pedido->id_pedido=id_pedido;
	pedido->estado=PENDIENTE;
	pedido->comidas_del_pedido=list_create();

	list_add(restaurante->tabla_pedidos,pedido);

//	int lista_size=list_size(restaurante->tabla_pedidos);
//	printf("El tamaño de la Lista de pedidos del restaurante es: %d\n", lista_size);


	return TRUE;

}

t_frame * buscar_frame_libre(t_list * tabla){


	t_frame * frame_libre=NULL;

	char * nombre=string_new();

	if(!list_is_empty(tabla)){
		frame_libre=list_remove(tabla,0);
		if(tabla==tabla_frames_libres){
			string_append(&nombre,"Principal");}
		else
			string_append(&nombre,"Swap");

		log_info(logger,"Se encontro en la Memoria %s el frame libre numero: %d  con la direccion %p",nombre,frame_libre->nro_frame,frame_libre->direccion_frame);
		log_info(logger,"Quedan en la Memoria %s  %d  frames libres",nombre, list_size(tabla));
		return frame_libre;
	}

	log_info(logger,"La tabla de frames libres esta vacia");
	return frame_libre;
}

void   iniciar_dump_cache  (int signal ){

	switch(signal){
		case SIGUSR1:
			dampear_memoria();
			break;
		default:
			;
	}

}

void dampear_memoria (){

	t_list * list_paginas_en_memoria_clock=list_create();
	char * cadena=NULL;

				void mostrar_comidas_en_memoria(void * elemento){
					t_struct_comida_dump * adm_comida=(t_struct_comida_dump*)elemento;
					t_comida * comida=malloc(sizeof(t_comida));
					int offset=0;


					memcpy(&comida->cantidad_lista_comida,adm_comida->frame->direccion_frame+offset, sizeof(comida->cantidad_lista_comida));
					offset+=sizeof(uint32_t);

					memcpy(&comida->cantidad_total_comida,adm_comida->frame->direccion_frame+offset, sizeof(comida->cantidad_total_comida));
					offset+=sizeof(uint32_t);

					memcpy(comida->nombre_comida,adm_comida->frame->direccion_frame+offset, SIZE_VECTOR_NOMBRE_PLATO);

					cadena=string_from_format("\n El plato: %-24s-frame: %-3d - timestamp:  %-d  -",comida->nombre_comida,adm_comida->frame->nro_frame,adm_comida->last_used);
					string_append_with_format(&cadena, " pedido: %d - restaurant: %-15s -bit uso: %-1d -bit modificado: %-1d ",adm_comida->nro_pedido,adm_comida->nombre_restaurante,adm_comida->bit_de_uso,adm_comida->modificado);
					txt_write_in_stdout(cadena);
					free(comida);
				}

				bool ordenar_lista_paginas_en_memoria(void * elemento1, void * elemento2){
					t_struct_comida_dump * adm_comida1=(t_struct_comida_dump*)elemento1;
					t_struct_comida_dump * adm_comida2=(t_struct_comida_dump*)elemento2;

					if(adm_comida1->frame->nro_frame<adm_comida2->frame->nro_frame)
						return TRUE;
					return FALSE;

				}

				void _search_oldest_pagina(void * elemento) {

					t_restaurante * restaurante= (t_restaurante *)elemento;

								void search_oldest_pagina(void * elemento){
									t_pedido_seg * pedido=(t_pedido_seg *)elemento;

											void search_oldest(void * elemento){
												t_pagina_comida * adm_comida=(t_pagina_comida *)elemento;

												//log_info(logger,"La cantidad de paginas de comida iteradas hasta el momento es:  %d",++contador_paginas_iteradas);
												if(adm_comida->esta_en_memoria_principal==TRUE){
													t_struct_comida_dump * struct_comida=malloc(sizeof(t_struct_comida_dump));
													struct_comida->bit_de_uso=adm_comida->bit_de_uso;
													struct_comida->estado=pedido->estado;
													struct_comida->frame=adm_comida->frame;
													struct_comida->last_used=adm_comida->last_used;
													struct_comida->modificado=adm_comida->modificado;
													struct_comida->nombre_restaurante=restaurante->nombre_restaurante;
													struct_comida->nro_pedido=pedido->id_pedido;
													list_add(list_paginas_en_memoria_clock,struct_comida);
												 }
											  }

						list_iterate(pedido->comidas_del_pedido,search_oldest);
					}

				 list_iterate(restaurante->tabla_pedidos,search_oldest_pagina);
			  }

	list_iterate(lista_restarurantes,_search_oldest_pagina);

	log_info(logger,"El tamaño de la lista que contiene los frames ordenado de la memoria principal es: %d", list_size(list_paginas_en_memoria_clock));

	list_sort(list_paginas_en_memoria_clock,ordenar_lista_paginas_en_memoria);

	list_iterate(list_paginas_en_memoria_clock,mostrar_comidas_en_memoria);

	list_destroy(list_paginas_en_memoria_clock);

	printf("\n");
	printf("\n");


}

void copiar_pagina_en_memoria(void * direccion_frame, t_comida * comida){

	int offset=0;


	memcpy(direccion_frame+offset,&comida->cantidad_lista_comida, sizeof(comida->cantidad_lista_comida));
	offset+=sizeof(uint32_t);

	memcpy(direccion_frame+offset,&comida->cantidad_total_comida, sizeof(comida->cantidad_total_comida));
	offset+=sizeof(uint32_t);

	memcpy(direccion_frame+offset,comida->nombre_comida, SIZE_VECTOR_NOMBRE_PLATO);

//	mem_hexdump(direccion_frame,SIZE_PAGINA);

	//free(comida);

//	log_info(logger,"Se guardo la pagina en memoria");

}

void leer_pagina_en_memoria(void * direccion_frame,t_comida * comida){

	int offset=0;


	memcpy(&comida->cantidad_lista_comida,direccion_frame+offset, sizeof(comida->cantidad_lista_comida));
	offset+=sizeof(uint32_t);

	memcpy(&comida->cantidad_total_comida,direccion_frame+offset, sizeof(comida->cantidad_total_comida));
	offset+=sizeof(uint32_t);

	memcpy(comida->nombre_comida,direccion_frame+offset, SIZE_VECTOR_NOMBRE_PLATO);

//	mem_hexdump((void*)comida,SIZE_PAGINA);


}

t_pagina_comida *          cargar_pagina_a_memoria_principal(t_pagina_comida *  adm_comida  ){

	t_frame * frame_libre=NULL;
	t_comida * comida=malloc(SIZE_PAGINA);
	adm_comida->frame=malloc(sizeof(t_frame));//-------------------------------------------------NO SE PORQUE HAGO ESTO

	frame_libre=buscar_frame_libre(tabla_frames_libres);
	if(frame_libre==NULL){
		log_info(logger,"La memoria principal se encuentra llena");
		frame_libre=seleccionar_victima_en_memoria_principal();					//No hay frames libres, utilizo algun metodo de remplazo
	}
	//log_info(logger,"Pruebo--La direccion del frame encontrado es: %p",frame_libre->direccion_frame);

	adm_comida->frame->nro_frame=frame_libre->nro_frame;
	adm_comida->frame->direccion_frame=frame_libre->direccion_frame;

	//-----------Copio la informacion de SWAP en Memoria Principal
	leer_pagina_en_memoria(adm_comida->frame_swap->direccion_frame,comida);
	copiar_pagina_en_memoria(frame_libre->direccion_frame,comida);

	adm_comida->frame->nro_frame=frame_libre->nro_frame;
	adm_comida->frame->direccion_frame=frame_libre->direccion_frame;

	adm_comida->modificado=FALSE;
	adm_comida->esta_en_memoria_principal=TRUE;
	adm_comida->bit_de_uso=TRUE;
	adm_comida->last_used=timestamp();

	log_error(logger,"Se carga la pagina en la memoria principal en el frame N°: %d y la direccion: %p",adm_comida->frame->nro_frame,adm_comida->frame->direccion_frame);
	printf("\n");
//	log_info(logger,"Se carga el timestamp con el valor: %d", adm_comida->last_used);
//	log_info(logger,"El bit de uso queda con el valor: %d", adm_comida->bit_de_uso);

	free(comida);

	return adm_comida;
}

t_frame * seleccionar_victima_en_memoria_principal(){


	if(string_equals_ignore_case(algoritmo_remplazo,"LRU")){
		log_error(logger,"Se va a utilizar el Algoritmo de LRU para eliminar una pagina de la memoria");
		return utilizar_algoritmo_remplazo_lru();
	}else{if(string_equals_ignore_case(algoritmo_remplazo,"CLOCK_MEJORADO")){
			log_error(logger,"Se va a utilizar el Algoritmo de clock mejorado para eliminar una pagina de la memoria");
			return utilizar_algoritmo_remplazo_clock_mejorado();}
	     else
	    	 {log_error(logger,"No existe el algortimo de remplazo.");}
	}
	return NULL;
}

uint64_t timestamp(void) {

	struct timeval tv;
	gettimeofday(&tv, NULL);
	unsigned long long tiempo = (((unsigned long long )(tv.tv_sec)) * 1000 + ((unsigned long long)(tv.tv_usec))/1000);

	return (uint64_t)tiempo;
}

t_frame *  utilizar_algoritmo_remplazo_lru              (){

	t_frame * frame=NULL;
	t_pagina_comida *   aux_pagina = NULL;
	uint64_t            aux_timestamp =UINT64_MAX;

			void _search_oldest_pagina(void * elemento) {

				t_restaurante * restaurante= (t_restaurante *)elemento;

							void search_oldest_pagina(void * elemento){
								t_pedido_seg * pedido=(t_pedido_seg *)elemento;

										void search_oldest(void * elemento){
											t_pagina_comida * adm_comida=(t_pagina_comida *)elemento;

											if(adm_comida->esta_en_memoria_principal==TRUE){
												if(adm_comida->last_used<aux_timestamp){
													aux_timestamp=adm_comida->last_used;
													aux_pagina=adm_comida;
												 }
											 }
										  }

					list_iterate(pedido->comidas_del_pedido,search_oldest);
				}

		     list_iterate(restaurante->tabla_pedidos,search_oldest_pagina);
	      }

	list_iterate(lista_restarurantes,_search_oldest_pagina);

	log_error(logger,"Se selecciono de la Memoria Principal como victima el frame: %d con la direccion de memoria: %p", aux_pagina->frame->nro_frame,aux_pagina->frame->direccion_frame);
	log_error(logger,"El frame seleccionado contiene la pagina perteneciente al plato: %s", ((t_comida*)aux_pagina->frame->direccion_frame)->nombre_comida);
	//	log_info(logger,"tiene el bit de uso en %d y el bit de modificado en %d",aux_pagina->bit_de_uso,aux_pagina->modificado);
	if(aux_pagina->modificado==TRUE){
		//Tengo que actualizar la memoria SWAP
		t_comida * comida=malloc(sizeof(t_comida));
		printf("\n");
		log_info(logger,"La pagina que se quiere remplazar se encuentra modificada");
		log_info(logger,"Se hace un acceso a disco para actualizar el contenido de la pagina");

		leer_pagina_en_memoria(aux_pagina->frame->direccion_frame,comida);
		copiar_pagina_en_memoria(aux_pagina->frame_swap->direccion_frame,comida);
		aux_pagina->modificado=FALSE;
//		log_info(logger,"Se actualiza la pagina guardada en SWAP");
		free(comida);
		printf("\n");
	}

	frame=aux_pagina->frame;
	aux_pagina->frame=NULL;
	aux_pagina->esta_en_memoria_principal=FALSE;

//	log_info(logger,"Se encontro la pagina con mas tiempo sin usar. Se paso a SWAP y se libero el FRAME");

	log_error(logger,"Se libero el frame N°: %d de la Memoria Principal",frame->nro_frame);
	printf("\n");
	return frame;

}

t_frame *  utilizar_algoritmo_remplazo_clock_mejorado   (){

	t_frame * frame=NULL;
	t_list * list_paginas_en_memoria_clock=list_create();
//	int contador_paginas_iteradas=0;
//	t_pagina_comida *   aux_pagina = NULL;

			bool ordenar_lista_paginas_en_memoria(void * elemento1, void * elemento2){
				t_pagina_comida * adm_comida1=(t_pagina_comida*)elemento1;
				t_pagina_comida * adm_comida2=(t_pagina_comida*)elemento2;

				if(adm_comida1->frame->nro_frame<adm_comida2->frame->nro_frame)
					return TRUE;
				return FALSE;

			}

			void _search_oldest_pagina(void * elemento) {

				t_restaurante * restaurante= (t_restaurante *)elemento;

							void search_oldest_pagina(void * elemento){
								t_pedido_seg * pedido=(t_pedido_seg *)elemento;

										void search_oldest(void * elemento){
											t_pagina_comida * adm_comida=(t_pagina_comida *)elemento;

											//log_info(logger,"La cantidad de paginas de comida iteradas hasta el momento es:  %d",++contador_paginas_iteradas);
											if(adm_comida->esta_en_memoria_principal==TRUE){
												//log_info(logger,"Cargo un en la lista de frames para elalgoritmo");
												list_add(list_paginas_en_memoria_clock,adm_comida);
											 }
										  }

					list_iterate(pedido->comidas_del_pedido,search_oldest);
				}

		     list_iterate(restaurante->tabla_pedidos,search_oldest_pagina);
	      }

	list_iterate(lista_restarurantes,_search_oldest_pagina);

//	log_info(logger,"El tamaño de la lista que contiene los frames ordenado de la memoria principal es: %d", list_size(list_paginas_en_memoria_clock));

	list_sort(list_paginas_en_memoria_clock,ordenar_lista_paginas_en_memoria);

	frame=buscar_victima_clock(list_paginas_en_memoria_clock);

	log_error(logger,"Se libero el frame N°: %d de la Memoria Principal",frame->nro_frame);
	printf("\n");
	list_destroy(list_paginas_en_memoria_clock);

	return frame;
}

t_frame * buscar_victima_clock(t_list * list_paginas_en_memoria_clock){ //Verificar modificiaciones de las paginas y guardarlas en memoria

	t_pagina_comida * adm_comida;
	int limite_for=puntero_clock;
	t_frame * frame=NULL;
	t_comida * comida=malloc(sizeof(t_comida));

//	log_info(logger,"EL puntero apunta al frame: %d", puntero_clock);
	printf("\n");
	for(int vuelta=0;vuelta<2;vuelta++){
//---------1° paso, busco m=0 y u=0
//			log_info(logger, "Arranco la vuelta 1, primera parte");
			for(;puntero_clock<cantidad_frames_memoria_principal;puntero_clock++){//Busco m=0 y u=0
				adm_comida=list_get(list_paginas_en_memoria_clock,puntero_clock);
//				log_info(logger,"Se analiza el frame %d  con el bit de uso en %d y el bit de modificado en %d",adm_comida->frame->nro_frame,adm_comida->bit_de_uso,adm_comida->modificado);
				if(adm_comida->modificado==FALSE && adm_comida->bit_de_uso==FALSE){
					log_error(logger,"Se selecciono el frame %d  con el bit de uso en %d y el bit de modificado en %d",adm_comida->frame->nro_frame,adm_comida->bit_de_uso,adm_comida->modificado);
					log_error(logger,"El frame seleccionado contiene la pagina perteneciente al plato: %s", ((t_comida*)adm_comida->frame->direccion_frame)->nombre_comida);
					frame=adm_comida->frame;
					adm_comida->frame=NULL;
					adm_comida->esta_en_memoria_principal=FALSE;
					if(puntero_clock==(cantidad_frames_memoria_principal-1))
						puntero_clock=0;
					else
						puntero_clock++;
//					log_info(logger,"El puntero del clock, queda ahora apuntando al frame numero %d", puntero_clock);
					return frame;
				}
			}
			puntero_clock=0;
			if(limite_for!=0){
//				log_info(logger, "Arranco la vuelta 1, segunda parte");
				for(puntero_clock=0;puntero_clock<limite_for;puntero_clock++){//Busco m=0 y u=0
					adm_comida=list_get(list_paginas_en_memoria_clock,puntero_clock);
//					log_info(logger,"Se analiza el frame %d  con el bit de uso en %d y el bit de modificado en %d",adm_comida->frame->nro_frame,adm_comida->bit_de_uso,adm_comida->modificado);
					if(adm_comida->modificado==FALSE && adm_comida->bit_de_uso==FALSE){
						log_error(logger,"Se Selecciono el frame %d  con el bit de uso en %d y el bit de modificado en %d",adm_comida->frame->nro_frame,adm_comida->bit_de_uso,adm_comida->modificado);
						log_error(logger,"El frame seleccionado contiene la pagina perteneciente al plato: %s", ((t_comida*)adm_comida->frame->direccion_frame)->nombre_comida);
						frame=adm_comida->frame;
						adm_comida->frame=NULL;
						adm_comida->esta_en_memoria_principal=FALSE;
						puntero_clock++;
//						log_info(logger,"El puntero del clock, queda ahora apuntando al frame numero %d", puntero_clock);
						return frame;				}
				}
			}
		//---------2° paso busco m=1 y u=0 y pongo u en 0 en caso contrario
//			log_info(logger, "Arranco la vuelta 2, primera parte");
			for(;puntero_clock<cantidad_frames_memoria_principal;puntero_clock++){//Busco m=1 y u=0
				adm_comida=list_get(list_paginas_en_memoria_clock,puntero_clock);
//				log_info(logger,"Se analiza el frame %d  con el bit de uso en %d y el bit de modificado en %d",adm_comida->frame->nro_frame,adm_comida->bit_de_uso,adm_comida->modificado);
				if(adm_comida->modificado==TRUE && adm_comida->bit_de_uso==FALSE){
					log_error(logger,"Se Selecciono el frame %d  con el bit de uso en %d y el bit de modificado en %d",adm_comida->frame->nro_frame,adm_comida->bit_de_uso,adm_comida->modificado);
					log_error(logger,"El frame seleccionado contiene la pagina perteneciente al plato: %s", ((t_comida*)adm_comida->frame->direccion_frame)->nombre_comida);
					frame=adm_comida->frame;
					leer_pagina_en_memoria(adm_comida->frame->direccion_frame,comida);
					copiar_pagina_en_memoria(adm_comida->frame_swap->direccion_frame,comida);
					adm_comida->modificado=FALSE;
					log_info(logger,"La pagina que se quiere remplazar se encuentra modificada");
					log_info(logger,"Se hace un acceso a disco para actualizar el contenido de la pagina");
					adm_comida->frame=NULL;
					adm_comida->esta_en_memoria_principal=FALSE;
					free(comida);
					if(puntero_clock==(cantidad_frames_memoria_principal-1))
						puntero_clock=0;
					else
						puntero_clock++;
//					log_info(logger,"El puntero del clock, queda ahora apuntando al frame numero %d", puntero_clock);
					return frame;
				}else{
					adm_comida->bit_de_uso=FALSE;
//					log_info(logger,"El puntero del clock, queda ahora apuntando al frame numero %d", puntero_clock);
				}
			}
			puntero_clock=0;
			if(limite_for!=0){
//				log_info(logger, "Arranco la vuelta 2, segunda parte");
				for(puntero_clock=0;puntero_clock<limite_for;puntero_clock++){//Busco m=1 y u=0
					adm_comida=list_get(list_paginas_en_memoria_clock,puntero_clock);
//					log_info(logger,"Se analiza el frame %d  con el bit de uso en %d y el bit de modificado en %d",adm_comida->frame->nro_frame,adm_comida->bit_de_uso,adm_comida->modificado);
					if(adm_comida->modificado==TRUE && adm_comida->bit_de_uso==FALSE){
						log_error(logger,"Se Selecciono el frame %d  con el bit de uso en %d y el bit de modificado en %d",adm_comida->frame->nro_frame,adm_comida->bit_de_uso,adm_comida->modificado);
						log_error(logger,"El frame seleccionado contiene la pagina perteneciente al plato: %s", ((t_comida*)adm_comida->frame->direccion_frame)->nombre_comida);
						frame=adm_comida->frame;
						leer_pagina_en_memoria(adm_comida->frame->direccion_frame,comida);
						copiar_pagina_en_memoria(adm_comida->frame_swap->direccion_frame,comida);
						adm_comida->modificado=FALSE;
						log_info(logger,"La pagina que se quiere remplazar se encuentra modificada");
						log_info(logger,"Se hace un acceso a disco para actualizar el contenido de la pagina");
						adm_comida->frame=NULL;
						adm_comida->esta_en_memoria_principal=FALSE;
						free(comida);
						puntero_clock++;
//						log_info(logger,"El puntero del clock, queda ahora apuntando al frame numero %d", puntero_clock);
						return frame;
					}else{
						adm_comida->bit_de_uso=FALSE;
//						log_info(logger,"El puntero del clock, queda ahora apuntando al frame numero %d", puntero_clock);
					}
				}
			}
	}
	free(comida);
	log_error(logger, "No selecciono ninguna victima el algortimo clock mejorado. Verificar su comportamiento");
	return frame;
}
