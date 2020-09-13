/*
 * memoria.h
 *
 *  Created on: 13 sep. 2020
 *      Author: utnso
 */

#ifndef MEMORIA_H_
#define MEMORIA_H_

#include "serializar.h"


typedef struct {
	char * nombre_restaurante;
	t_list * tabla_pedidos;
}t_tabla_segmentos;


typedef struct{
	char * nombre_pedido;
	t_list * comidas_del_pedido;
}t_segmento;

#endif /* MEMORIA_H_ */
