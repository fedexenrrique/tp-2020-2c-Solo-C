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
	uint32_t		size_nombre_restaurante;
	char		* 	nombre_restaurante;
}t_solicitud_info_restaurante;

typedef struct{
	uint32_t		cantidad_cocineros;
	uint32_t		posicion_x;
	uint32_t		posicion_y;
	uint32_t		size_afinidad_cocineros;
	char		*	afinidad_cocineros;
	uint32_t		size_platos;
	char		*	platos;
	uint32_t		size_precio_platos;
	char		*	precio_platos;
	uint32_t		cantidad_hornos;
}t_respuesta_info_restaurante;



#endif /* RESTAURANTE_H_ */
