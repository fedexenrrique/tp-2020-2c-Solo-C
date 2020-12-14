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

#include "app.h"




typedef struct {
	uint32_t    resto_x;
	uint32_t    resto_y;
	uint32_t	id_resto;
	char*       resto_nombre;
	uint32_t    socket_conectado;
	char**      list_platos;
} t_info_restaurante;



t_info_restaurante * deserializar_info_resto(void * ,uint32_t );
bool  enviar_confirmar_pedido_a_resto(t_info_restaurante *,uint32_t);
t_info_restaurante * buscar_info_de_restaurante(char *,t_list *);
uint32_t solicitar_id_a_restaurante(uint32_t);
void procesar_consultar_pedido(uint32_t id_pedido,t_header * header_recibido);
void recibir_desde_app_respuesta_obtener_pedido_y_responder(t_header * header_recibido,t_info_restaurante * info_resto);
bool realizar_plato_listo(t_plato_listo *,uint32_t id_proceso);
t_plato_listo * recibir_10_plato_listo_resto_app(t_header *);
#endif /* UTILS_H_ */
