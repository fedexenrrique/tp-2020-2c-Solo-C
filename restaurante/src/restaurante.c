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
void* serializarPedidoRestaurante(t_header2* header) {

	int stream_size = sizeof(uint32_t) * 4 + header->size;
	void * stream = malloc(stream_size);
	int offset = 0;

	int valor = 0;
	memset(stream, valor, stream_size);

	valor = header->modulo; // 01 - MODULO
	memcpy(stream + offset, &valor, sizeof(uint32_t));

	offset += sizeof(uint32_t);

	valor = header->id_proceso; // 02 - ID PROCESO
	memcpy(stream + offset, &valor, sizeof(uint32_t));

	offset += sizeof(uint32_t);

	valor = header->nro_msg; // 03 - TIPO MENSAJE
	memcpy(stream + offset, &valor, sizeof(uint32_t));

	offset += sizeof(uint32_t);

	valor = header->size; // 04 - TAMAÑO DE PAYLOAD
	memcpy(stream + offset, &valor, sizeof(uint32_t));

	offset += sizeof(uint32_t);

	if (header->size > 0) { // 05 - PAYLOAD

		memcpy(stream + offset, header->payload, header->size);

		offset += header->size;

	}

	return stream;

	free(stream);

}

int main(void) {
	prueba_biblioteca_compartida();
	int conf = cargarConfiguracion();

	int socketCliente = crear_socket_y_conectar(configuracion->ipSindicato,
			configuracion->puertoSindicato);

	tSolicitudInfoResto* info = malloc(sizeof(tSolicitudInfoResto));

	info->nombreRestaurante = string_new();

	string_append(&info->nombreRestaurante, "RESTO_PRUEBA");

	//Armo el header + payload

	t_header2* header = malloc(sizeof(t_header2));

	int bufferSize = strlen(info->nombreRestaurante);

	header->modulo = 4;
	header->id_proceso = 0;
	header->nro_msg = 3;
	header->size = 0;
	header->payload = malloc(bufferSize);

	header->size = strlen(info->nombreRestaurante);

	//header->size=strlen(header->payload)-1;
	int streamSize = sizeof(uint32_t) * 4 + header->size;
	void* stream = malloc(streamSize);

	memcpy(header->payload, info->nombreRestaurante, bufferSize);

//	int bytesAEnviarPaquete = serializar(stream, "%z%z%z%z%s", header->modulo,
//			header->id_proceso, header->nro_msg, header->size,
//			info->nombreRestaurante);

	void* streamSerializado= serializarPedidoRestaurante(header);

	//int bufferSize=bytesAEnviarPaquete-1;
	printf("BUFFER SIZE: %d\n", bufferSize);

	if (-1 == send(socketCliente, streamSerializado, streamSize, 0)) {
		log_error(logger, "Error al enviar pedido de infor Resturante");
	}
	free(info);
//	free(header);
//	free(buffer);

	return EXIT_SUCCESS;
}

