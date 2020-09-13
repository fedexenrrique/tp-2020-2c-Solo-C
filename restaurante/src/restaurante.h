/*
 * restaurante.h
 *
 *  Created on: 3 sep. 2020
 *      Author: utnso
 */

#ifndef RESTAURANTE_H_
#define RESTAURANTE_H_

#include "config_restaurante.h"



typedef struct{
	char	* 	nombre_restaurante;
}t_solicitud_info_restaurante;

typedef struct{

	char* nombreRestaurante;

}tSolicitudInfoResto;
typedef struct { // uint32_t modulo, id_proceso, nro_msg, size;
	uint32_t    modulo;
	uint32_t 	id_proceso;
	uint32_t 	nro_msg;
	uint32_t    size;
	void * payload;
} t_header2;


typedef struct{
	uint32_t cantCocineros;
	char* posicion;
	char* afinidadCocineros;
	char* platos;
	char* preciosPlatos;
	uint32_t cantidadHornos;
}tMensajeInfoRestaurante;

t_config * fd_configuracion;
t_log * logger;
typedef struct{
	int			cantidad_cocineros;
	char	*	posicion;
	char	*	afinidad_cocineros;
	char	*	platos;
	char	*	precio_platos;
	int			cantidad_hornos;
}t_respuesta_info_restaurante;


void recibirInfoRestaurante(tMensajeInfoRestaurante* infoRestaurante,int socketCliente);

#endif /* RESTAURANTE_H_ */
