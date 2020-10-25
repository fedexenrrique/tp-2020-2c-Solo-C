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

	p_inicio_memoria_swap= reservar_memoria_inicial(size_memoria_swap); //----------------CREO LA MEMORIA SWAP Y LISTA DE PUNTEROS
	p_fin_memoria_swap=p_inicio_memoria_swap+size_memoria_swap;


	log_info(logger,"El tamaño de la memoria principal es: %d", size_memoria_principal);
	log_info(logger,"El tamaño de la memoria principal es: %d", size_memoria_swap);
	log_info(logger,"El algoritmo de remplazo utilizado por la memoria es: %s",algoritmo_remplazo);


	list_pointer_memory_principal=list_create();
	list_pointer_memory_swap     =list_create();

	tabla_frames_libres=list_create();

	crear_pagina_memoria(list_pointer_memory_principal,size_memoria_principal);

/*	t_admin_memory_message * pointer_block=malloc(sizeof(t_admin_memory_message));
	pointer_block->estado=LIBRE;
	pointer_block->inicio_bloque=p_inicio_memoria_total;
	pointer_block->fin_bloque=p_inicio_memoria_total+size_memoria_total;
	pointer_block->size_mensaje=0;
	pointer_block->mensaje=NULL;
	pointer_block->last_used=0;
	log_info(logger,"La memoria comienza en la direccion: %p", pointer_block->inicio_bloque);
	log_info(logger,"La memoria finaliza en la direccion: %p", pointer_block->fin_bloque);
	list_add(list_pointer_memory,pointer_block);
*/
}

void * reservar_memoria_inicial(int size_memoria_total){

	void * p_memoria_total=malloc(size_memoria_total);

	memset(p_memoria_total, 0, size_memoria_total);

	return p_memoria_total;

}

void crear_pagina_memoria(t_list * list_pointer_memory_principal,int size_memoria_principal){

	int cantidad_paginas=size_memoria_principal/SIZE_PAGINA;

	for (int i=0;i<cantidad_paginas;i++){

		t_frame * frame=malloc(sizeof(t_frame));
		frame->nro_frame=i;
		if(i==0)
			frame->direccion_frame=p_inicio_memoria_principal;
		else
			frame->direccion_frame=p_inicio_memoria_principal+(i*SIZE_PAGINA);

		log_info(logger,"La direccion de memoria del frame es %p y la logica es %d",frame->direccion_frame,frame->direccion_frame-p_inicio_memoria_principal);

		list_add(tabla_frames_libres,frame);
	}

	log_info(logger,"Hay %d  marcos libres",list_size(tabla_frames_libres));
	log_info(logger,"Se dividio la memoria principal en %d  frames",cantidad_paginas);

}


t_restaurante * crear_tabla_segmentos_restaurante(char * nombre_restaurante  ){

	t_restaurante * restaurante=malloc(sizeof(t_restaurante));

	restaurante->nombre_restaurante=nombre_restaurante;
	restaurante->tabla_pedidos=list_create();

	list_add(lista_restarurantes,restaurante);

	printf("Se creo La tabla de segmentos del nuevo restaurante. \n");

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

	printf("Se recibio el pedido nro: %d\n",id_pedido);
	printf("Se recibio restaurant: %s\n",restaurante->nombre_restaurante);

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

	int lista_size=list_size(restaurante->tabla_pedidos);
	printf("El tamaño de la Lista es: %d\n", lista_size);


	return TRUE;

}
