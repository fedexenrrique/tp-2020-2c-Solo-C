/*
 * restaurante.h
 *
 *  Created on: 3 sep. 2020
 *      Author: utnso
 */

#ifndef RESTAURANTE_H_
#define RESTAURANTE_H_

#include "config_restaurante.h"


int									main										(void);
void 								obtener_info_restaurante					(void);
t_respuesta_info_restaurante 	* 	deserializar_respuesta_info_restaurante		(void *);

#endif /* RESTAURANTE_H_ */
