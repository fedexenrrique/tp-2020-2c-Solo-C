/*
 ============================================================================
 Name        : restaurante.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "restaurante.h"

int main(void) {
	puts("!!!Hello World!!!");
	prueba_biblioteca_compartida();
	int conf=cargarConfiguracion();

	int socketCliente= crear_socket_y_conectar(configuracion->ipSindicato,configuracion->puertoSindicato);

	tSolicitudInfoResto* info=malloc(sizeof(tSolicitudInfoResto));

	info->nombreRestaurante=string_new();
	string_append(&info->nombreRestaurante,"RESTO PRUEBA");

	//Armo el header + payload
	char* buffer=malloc(100);

	t_header* header= malloc(sizeof(t_header));

	header->modulo=4;
	header->id_proceso=0;
	header->nro_msg=3;
	header->size=0;
	header->payload=string_new();

	string_append(&header->payload,info->nombreRestaurante);
	header->size=strlen(header->payload);

	int bytesAEnviarPayload=serializar(buffer,"%d%d%d%d%s",header->modulo,header->id_proceso,header->nro_msg,header->size,header->payload);
	int bufferSize=bytesAEnviarPayload-1;

	if(-1==send(socketCliente,buffer,bufferSize,0)){
		log_error(logger,"Error al enviar pedido de infor Resturante");
	}


	return EXIT_SUCCESS;
}
