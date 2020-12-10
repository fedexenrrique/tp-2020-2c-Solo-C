/*
 * utils.h
 *
 *  Created on: 8 dic. 2020
 *      Author: utnso
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <serializar.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/memory.h>
#include "commons/collections/list.h"




typedef struct {
	uint32_t    resto_x;
	uint32_t    resto_y;
	char*       resto_nombre;
	uint32_t    socket_conectado;
	char**      list_platos;
} t_info_restaurante;



t_info_restaurante * deserializar_info_resto(void * ,uint32_t );
bool  enviar_confirmar_pedido_a_resto(t_info_restaurante *,uint32_t);

#endif /* UTILS_H_ */
