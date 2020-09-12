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

	prueba_biblioteca_compartida();

	cargar_config();

	obtener_info_restaurante();

	return EXIT_SUCCESS;
}

void obtener_info_restaurante() {
	int socket_sindicato = crear_socket_y_conectar(ip_sindicato, puerto_sindicato);

	t_solicitud_info_restaurante* info_restaurante = malloc(sizeof(t_solicitud_info_restaurante));

	info_restaurante->nombre_restaurante = string_new();
	//	string_append(&info_restaurante->nombre_restaurante, "RESTO PRUEBA");
	string_append(&info_restaurante->nombre_restaurante, nombre_restaurante);

	//Armo el header + payload
	char* buffer = malloc(100);

	t_header* header= malloc(sizeof(t_header));

	header->modulo = 4;
	header->id_proceso = 0;
	header->nro_msg = 3;
	header->size = 0;
	header->payload = string_new();

	string_append(&header->payload, info_restaurante->nombre_restaurante);
	header->size = strlen(header->payload) - 1;

	int payload_a_enviar = serializar(buffer,"%d%d%d%d%s", header->modulo, header->id_proceso,
											header->nro_msg, header->size, header->payload);
	int buffer_size = payload_a_enviar - 1;

	if(-1 == send(socket_sindicato, buffer, buffer_size, 0)){
		log_error(logger_restaurante, "Error al enviar pedido de infor Resturante");
	}
}
