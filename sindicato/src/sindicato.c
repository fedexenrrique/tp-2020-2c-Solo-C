/*
 ============================================================================
 Name        : sindicato.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "sindicato.h"
#include "..//biblioteca-compartida/serializar.h"

int main(void) {
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

	int posSerializacion=serializar(buffer,"%d%s%z%d%s",pedidoPrueba->idPedido,pedidoPrueba->descripcionPedido,pedidoPrueba->precioPedido,pedidoPrueba->idRestaurante,pedidoPrueba->nombreEstaurante);


	int idPedidoDeserializado=0;
	char* descripcionDeserializada= malloc(30);
	uint32_t precioDeserializado=0;
	int idRestauranteDeserializado=0;
	char* nombreRestauranteDeserializado=malloc(30);

	int posDeserializacion= deserializar(buffer,"%d%s%z%d%s",&idPedidoDeserializado,descripcionDeserializada,&precioDeserializado,&idRestauranteDeserializado,nombreRestauranteDeserializado);

	printf("Id deserializado: %d\n",idPedidoDeserializado);
	printf("Descripcion Pedido dfeserializado: %s\n",descripcionDeserializada);
	printf("Precio pedido deserializado: %zu\n",precioDeserializado);
	printf("Id restaurante deserializado: %d\n",idRestauranteDeserializado);
	printf("Nombre restaurante deserializado: %s\n",nombreRestauranteDeserializado);

	free(buffer);

	return EXIT_SUCCESS;
}
