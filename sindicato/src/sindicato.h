/*
 * sindicato.h
 *
 *  Created on: 1 sep. 2020
 *      Author: utnso
 */

#ifndef SINDICATO_H_
#define SINDICATO_H_

//#include "serializar.h"

#include "utils.h"


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
	uint32_t sizeNombreRestaurante;
	char* nombreRestaurante;
	uint32_t sizeIdPedido;
	char* idPedido;
	uint32_t sizePlato;
	char* plato;
	uint32_t cantidad;

}tSolicitudAniadirPlato;

typedef struct{
	char* estadoPedido;
		char* listaPlatos;
		char* cantidadPlatos;
		char* cantidadLista;
		uint32_t precioTotal;
}tMensajeInfoPedido;

typedef struct{
	char* nombreRestaurante;
	uint32_t cantCocineros;
	char* posicion;
	char* afinidadCocineros;
	char* platos;
	char* preciosPlatos;
	uint32_t cantidadHornos;

}tCreacionRestaurante;

typedef struct{
	char* nombreReceta;
	char* pasos;
	char* tiemposPasos;
}tCreacionReceta;

typedef struct{
	char* estadoPedido;
	char* listaPlatos;
	char* cantidadPlatos;
	char* cantidadLista;
	uint32_t precioTotal;

}tCreacionPedido;

typedef struct{
	uint32_t tamBloques;
	uint32_t cantBloques;
	char* magicNumber;
}tInfoBloques;

typedef struct{
	uint32_t posCantCocineros;
	uint32_t sizeCantCocineros;
	uint32_t posPosicion;
	uint32_t sizePosicion;
	uint32_t posAfinidad;
	uint32_t sizeAfinidad;
	uint32_t posPlatos;
	uint32_t sizePlatos;
	uint32_t posPreciosPlatos;
	uint32_t sizePreciosPlatos;
	uint32_t posCantHornos;
	uint32_t sizeCantHornos;
}tPosicionesEnArchivoRestaurante;

tConfiguracion * configuracion;
t_config * fd_configuracion;
t_log * logger;
t_bitarray* bitMap;
tInfoBloques* infoBloques;
int fdArchivoBitmap;
int mapBitArray;
t_dictionary* diccionarioBloquesAsignadosARestos;
t_dictionary* diccionarioBloquesAsignadosARecetas;
t_dictionary* diccionarioBloquesAsignadosAPedidos;

t_dictionary* diccionarioPosPropiedadesEnArchivo;

int bloquesLibres;


char* pathFiles;
char* pathMetadata;
char* pathBloques;
char* pathRestaurantes;
char* pathRecetas;


void handleConexion(int socketCliente);
void armarPayloadRestaurante(tMensajeInfoRestaurante* info, void* stream);
int grabarInfoRestaurante(tCreacionRestaurante* restauranteNuevo, char* pathRestaurante);


#endif /* SINDICATO_H_ */
