/*
 * memoria.c
 *
 *  Created on: 13 sep. 2020
 *      Author: utnso
 */

#include "memoria.h"


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
