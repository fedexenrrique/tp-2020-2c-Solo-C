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
	char* puertoEscucha;
	char* puntoMontaje;

}tConfiguracion;

typedef struct { // uint32_t modulo, id_proceso, nro_msg, size;
	uint32_t    modulo;
	uint32_t    id_proceso;
	uint32_t    nro_msg;
	uint32_t	size;
	void* payload;
} t_header2;

typedef struct{
	uint32_t cantCocineros;
	char* posicion;
	char* afinidadCocineros;
	char* platos;
	char* preciosPlatos;
	uint32_t cantidadHornos;


}tMensajeInfoRestaurante;

typedef struct{
	char* nombreRestaurante;
	int cantCocineros;
	char* posicion;
	char* afinidadCocineros;
	char* platos;
	char* preciosPlatos;
	int cantidadHornos;

}tCreacionRestaurante;

tConfiguracion * configuracion;
t_config * fd_configuracion;
t_log * logger;

char* pathFiles;
char* pathMetadata;
char* pathBloques;
char* pathRestaurantes;
char* pathRecetas;

void handleConexion(int socketCliente);
void armarPayloadRestaurante(tMensajeInfoRestaurante* info, void* stream);


#endif /* SINDICATO_H_ */
