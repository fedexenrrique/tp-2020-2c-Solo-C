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


typedef struct {
	uint32_t size_nombre_restaurante;
	char *   nombre_restaurante     ;
	uint32_t id_pedido              ;          ;
	uint32_t  size_nombre_plato;
	char *    nombre_plato     ;
}t_plato_listo_para_app;



t_info_restaurante * deserializar_info_resto(void * ,uint32_t );
bool  enviar_confirmar_pedido_a_resto(t_info_restaurante *,uint32_t);
t_info_restaurante * buscar_info_de_restaurante(char *,t_list *);
uint32_t solicitar_id_a_restaurante(uint32_t);
void procesar_consultar_pedido(uint32_t id_pedido,t_header * header_recibido, int socket);
void recibir_desde_app_respuesta_obtener_pedido_y_responder(t_header * header_recibido,t_info_restaurante * info_resto);
bool realizar_plato_listo(t_plato_listo_para_app *,uint32_t);
t_plato_listo_para_app * recibir_10_plato_listo_resto_app(t_header *);
void recibir_obtener_pedido_y_verificar_estado_correcto (t_header *);
bool deserializar_obtener_pedido_y_verificar_estado_correcto(t_header *);
bool enviar_07_aniadir_plato_app_a_resto(int,uint32_t,char*);
void armar_y_enviar_respuesta(cod_msg tipo_msj,int socket_cliente);
#endif /* UTILS_H_ */
