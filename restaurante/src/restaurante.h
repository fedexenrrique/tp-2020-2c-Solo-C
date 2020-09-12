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
	int			cantidad_cocineros;
	char	*	posicion;
	char	*	afinidad_cocineros;
	char	*	platos;
	char	*	precio_platos;
	int			cantidad_hornos;
}t_respuesta_info_restaurante;



#endif /* RESTAURANTE_H_ */
