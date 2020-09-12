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

tConfiguracion * configuracion;
t_config * fd_configuracion;
t_log * logger;

void recibirInfoRestaurante(tMensajeInfoRestaurante* infoRestaurante,int socketCliente);

#endif /* RESTAURANTE_H_ */
