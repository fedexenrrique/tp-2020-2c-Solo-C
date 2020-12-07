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
	char* pasos;
	char* tiempos;

}tMensajeInfoReceta;

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

typedef struct{
	uint32_t sizePlatos;
	char*platos;
}tRespuestaConsultaPlatos;

typedef struct{
	uint32_t sizeNombreRestaurante;
	char* nombreRestaurante;
	uint32_t sizeIdPedido;
	char* idPedido;

}tGuardarPedido;

typedef struct{
	uint32_t sizePasos;
	char* pasos;
	uint32_t sizeTiempos;
	char* tiempos;
}tRespuestaReceta;


typedef struct{
	uint32_t sizeNombreRestaurante;
	char* nombreRestaurante;
	uint32_t idPedido;;

}tSolicitudPedido;

typedef struct{
	uint32_t sizeEstadoPedido;
	char* estadoPedido;
	uint32_t sizeListaPlatos;
	char* listaPlatos;
	uint32_t sizeCantidadPlatos;
	char* cantidadPlatos;
	uint32_t sizeCantidadLista;
	char* cantidadLista;
	uint32_t precioTotal;
}tRespuestaSolicitudPedido;

typedef struct{
	uint32_t sizeNombreRestaurante;
	char* nombreRestaurante;
	uint32_t idPedido;
	uint32_t sizeNombrePlatoListo;
	char* nombrePlatoListo;
}tSolicitudPlatiListo;

struct arg_struct {
	int arg1;
	tInfoBloques* arg2;
};

tConfiguracion * configuracion;
t_config * fd_configuracion;
t_log * logger;
t_bitarray* bitMap;
tInfoBloques* infoBloques;
int fdArchivoBitmap;
void* mapBitArray;
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


//void handleConexion(int socketCliente,tInfoBloques* infoBloques);
int montarFS(tInfoBloques* infoBloques);
int cargarConfiguracion();
void* handleConexion(void* arguments);
tInfoBloques* leerInfoBloques();
void llenarHeaderRespuesta(t_header2* header);
void armarPayloadRestaurante(tMensajeInfoRestaurante* info, void* stream);
int grabarInfoRestaurante(tCreacionRestaurante* restauranteNuevo, char* pathRestaurante,tInfoBloques* infoBloques);
int grabarArchivoRestaurante(tCreacionRestaurante* restauranteNuevo,tInfoBloques* infoBloques);
int grabarArchivoReceta(tCreacionReceta* recetaNueva,tInfoBloques*infoBloques);
int grabarArchivoPedido(tCreacionPedido* pedidoNuevo,char* pathPedido,char* nombrePedido,char* idPedido);
tMensajeInfoRestaurante *leerBloquesResto(int bloqueInicial,int sizeResto,t_list* bloquesAsigandosAResto);
int escribirBloques(char*propiedades,uint32_t cantEscritura, int bloqueInicial,char* nombreRecurso,t_list* bloquesAsignadosARecurso,char* tipoRecurso,char* operacion,tInfoBloques* infoBloques,char* idPedido);
#endif /* SINDICATO_H_ */
