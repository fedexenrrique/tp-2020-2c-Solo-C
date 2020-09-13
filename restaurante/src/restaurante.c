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

	cargar_config();

	obtener_info_restaurante();

	return 1;
}

void obtener_info_restaurante() {
	int socket_sindicato = crear_socket_y_conectar(ip_sindicato,
			puerto_sindicato);

	t_solicitud_info_restaurante* info_restaurante = malloc(
			sizeof(t_solicitud_info_restaurante));

	info_restaurante->nombre_restaurante = string_new();
	//	string_append(&info_restaurante->nombre_restaurante, "RESTO PRUEBA");
	string_append(&info_restaurante->nombre_restaurante, nombre_restaurante);
	log_info("Nombre restaurante: %s\n",nombre_restaurante);

	//Armo el header + payload
	char* buffer = malloc(100);
	t_header* header= malloc(sizeof(t_header));
	header->modulo = 4;
	header->id_proceso = 0;
	header->nro_msg = 3;
	header->size = 0;
	header->payload = string_new();

	string_append(&header->payload, info_restaurante->nombre_restaurante);
	header->size = strlen(header->payload);

	int payload_a_enviar = serializar(buffer, "%z%z%z%z%s", header->modulo,
			header->id_proceso, header->nro_msg, header->size, header->payload);
	int buffer_size = payload_a_enviar - 1;

	if (-1 == send(socket_sindicato, buffer, buffer_size, 0)) {
		log_error(logger_restaurante,
				"Error al enviar pedido de infor Resturante");
	}

//Recibo info del restaurante
	t_header* headerRecibido = malloc(sizeof(t_header2));
	uint32_t modulo, idProceso, nroMsg, size;

	recv(socket_sindicato, &modulo, sizeof(uint32_t), MSG_WAITALL);
	recv(socket_sindicato, &idProceso, sizeof(uint32_t), MSG_WAITALL);
	recv(socket_sindicato, &nroMsg, sizeof(uint32_t), MSG_WAITALL);
	recv(socket_sindicato, &size, sizeof(uint32_t), MSG_WAITALL);

	headerRecibido->id_proceso = idProceso;
	headerRecibido->modulo = modulo;
	headerRecibido->nro_msg = nroMsg;
	headerRecibido->size = size;
	headerRecibido->payload = malloc(headerRecibido->size);
	tMensajeInfoRestaurante* infoRestaurante = malloc(
			sizeof(tMensajeInfoRestaurante));

	if (size > 0) {
		recibirInfoRestaurante(infoRestaurante, socket_sindicato);
	}

	log_info(logger, infoRestaurante->platos);


}

void recibirInfoRestaurante(tMensajeInfoRestaurante* infoRestaurante,
		int socketCliente) {
	uint32_t cantPosicion, cantAfinidadesCocineros, cantPlatos,
			cantPreciosPlatos;

	recv(socketCliente, &infoRestaurante->cantCocineros, sizeof(uint32_t),
			MSG_WAITALL);

	recv(socketCliente, &cantPosicion, sizeof(uint32_t), MSG_WAITALL);
	recv(socketCliente, infoRestaurante->posicion, cantPosicion, MSG_WAITALL);

	recv(socketCliente, &cantAfinidadesCocineros, sizeof(uint32_t),
			MSG_WAITALL);
	recv(socketCliente, infoRestaurante->afinidadCocineros,
			cantAfinidadesCocineros, MSG_WAITALL);

	recv(socketCliente, &cantPlatos, sizeof(uint32_t), MSG_WAITALL);
	recv(socketCliente, infoRestaurante->platos, cantPlatos, MSG_WAITALL);

	recv(socketCliente, &cantPreciosPlatos, sizeof(uint32_t), MSG_WAITALL);
	recv(socketCliente, infoRestaurante->preciosPlatos, cantPreciosPlatos,
			MSG_WAITALL);

	recv(socketCliente, &infoRestaurante->cantidadHornos, sizeof(uint32_t),
			MSG_WAITALL);

}

