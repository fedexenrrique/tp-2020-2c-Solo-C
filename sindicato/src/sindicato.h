/*
 * sindicato.h
 *
 *  Created on: 1 sep. 2020
 *      Author: utnso
 */

#ifndef SINDICATO_H_
#define SINDICATO_H_

#include "serializar.h"

typedef struct{

	int idPedido;
	uint32_t precioPedido;
	char* descripcionPedido;
	int idRestaurante;
	char* nombreEstaurante;

} tPrueba;

typedef struct{
	int puertoEscucha;
	char* puntoMontaje;

}tConfiguracion;


tConfiguracion * configuracion;
t_config * fd_configuracion;
t_log * logger;

char* pathFiles;
char* pathMetadata;
char* pathBloques;
char* pathRestaurantes;
char* pathRecetas;




#endif /* SINDICATO_H_ */
