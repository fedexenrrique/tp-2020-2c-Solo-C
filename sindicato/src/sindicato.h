/*
 * sindicato.h
 *
 *  Created on: 1 sep. 2020
 *      Author: utnso
 */

#ifndef SINDICATO_H_
#define SINDICATO_H_

#include "serializar.h"
#include <stdio.h>
#include <dirent.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <setjmp.h>

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <math.h>
#include <pthread.h>
#include <signal.h>
#include "commons/bitarray.h"

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

typedef struct{
	char* nombreReceta;
	char* pasos;
	char* tiemposPasos;


}tCreacionReceta;

typedef struct{
	uint32_t tamBloques;
	uint32_t cantBloques;
	char* magicNumber;
}tInfoBloques;

tConfiguracion * configuracion;
t_config * fd_configuracion;
t_log * logger;
t_bitarray* bitMap;
tInfoBloques* infoBloques;
int fdArchivoBitmap;
int mapBitArray;

char* pathFiles;
char* pathMetadata;
char* pathBloques;
char* pathRestaurantes;
char* pathRecetas;


void handleConexion(int socketCliente);
void armarPayloadRestaurante(tMensajeInfoRestaurante* info, void* stream);
int grabarInfoRestaurante(tCreacionRestaurante* restauranteNuevo, char* pathRestaurante);


#endif /* SINDICATO_H_ */
