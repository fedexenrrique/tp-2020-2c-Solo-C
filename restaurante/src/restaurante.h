/*
 * restaurante.h
 *
 *  Created on: 3 sep. 2020
 *      Author: utnso
 */

#ifndef RESTAURANTE_H_
#define RESTAURANTE_H_

#include "serializar.h"



typedef struct{
	char* puertoEscucha;
	char* ipSindicato;
	int puertoSindicato;
	char* ipApp;
	int puertoApp;
	int quantum;
	char* pathArchivoLog;
	char* algoritmoPlanificacion;
	char* nombreRestaurante;


}tConfiguracion;


typedef struct{

	char* nombreRestaurante;

}tSolicitudInfoResto;


tConfiguracion * configuracion;
t_config * fd_configuracion;
t_log * logger;

#endif /* RESTAURANTE_H_ */
