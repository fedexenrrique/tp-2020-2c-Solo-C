/*
 ============================================================================
 Name        : sindicato.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */


#include "sindicato.h"
//#include "/home/utnso/tp-2020-2c-Solo-C/biblioteca-compartida/serializar.h"
#define PATH_FILES "/Files/"
int main(int argc, char *argv[]) {
	cargarConfiguracion();

 	puts("!!!Hello World!!!"); /* prints !!!Hello World!!! */

	tPrueba* pedidoPrueba= malloc(sizeof(tPrueba));

	pedidoPrueba->idPedido=1;
	pedidoPrueba->descripcionPedido=malloc(30);
	strcpy(pedidoPrueba->descripcionPedido,"Pedido de Prueba");
	pedidoPrueba->precioPedido=300;
	pedidoPrueba->idRestaurante=3049595;
	pedidoPrueba->nombreEstaurante=malloc(30);
	strcpy(pedidoPrueba->nombreEstaurante,"Restaurante de Prueba");

	char* buffer=malloc(100);

	//Invoco a la funcion 'serializar': ({buffer que quiero enviar}, "%s%d..." agregar tanto '%{tipoDato}'los tipos de datos de acuerdo  los parametros que quiero enviar , variable1,variable2...variable N)
	int posSerializacion=serializar(buffer,"%d%s%z%d%s",pedidoPrueba->idPedido,pedidoPrueba->descripcionPedido,pedidoPrueba->precioPedido,pedidoPrueba->idRestaurante,pedidoPrueba->nombreEstaurante);


	int idPedidoDeserializado=0;
	char* descripcionDeserializada= malloc(30);
	uint32_t precioDeserializado=0;
	int idRestauranteDeserializado=0;
	char* nombreRestauranteDeserializado=malloc(30);

	//Idem serializar, agrego las variables donde quiero guardar los datos deserialzados del buffer
	//Para datos numericos agregar el operador '&'
	int posDeserializacion= deserializar(buffer,"%d%s%z%d%s",&idPedidoDeserializado,descripcionDeserializada,&precioDeserializado,&idRestauranteDeserializado,nombreRestauranteDeserializado);

	printf("Id deserializado: %d\n",idPedidoDeserializado);
	printf("Descripcion Pedido dfeserializado: %s\n",descripcionDeserializada);
	printf("Precio pedido deserializado: %zu\n",precioDeserializado);
	printf("Id restaurante deserializado: %d\n",idRestauranteDeserializado);
	printf("Nombre restaurante deserializado: %s\n",nombreRestauranteDeserializado);

	pathFiles=string_new();
	pathBloques=string_new();
	pathMetadata=string_new();
	string_append(&pathFiles,configuracion->puntoMontaje);
	string_append(&pathFiles,"/Files/");

	string_append(&pathBloques,configuracion->puntoMontaje);
	string_append(&pathBloques,"/Bloques/");


	string_append(&pathMetadata,configuracion->puntoMontaje);
	string_append(&pathMetadata,"/Metadata/");

	//Si no le paso los argumentos BLOCKSIZE, BLOCKS + MAGIC_NUMBER no monto el FS de nuevo
	if (*argv[1]!=NULL && *argv[2]!=NULL && argv[3]!=NULL ){
		int fs=montarFS(*argv[1],*argv[2],argv[3]);
	}




	free(buffer);
	free(pathFiles);
	free(pathBloques);
	free(pathMetadata);

	return EXIT_SUCCESS;
}