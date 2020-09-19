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

	conectar_restaurante_a_applicación();

	return 1;
}

void conectar_restaurante_a_applicación(void) {



	uint32_t sock_conectado = crear_socket_y_conectar( ip_app, puerto_app );

	t_header head;

	head.id_proceso = g_id_proceso;
	head.modulo = RESTAURANTE;
	head.nro_msg = CONECTAR;
	head.size = 999;
	head.payload = NULL;

	enviar_buffer(sock_conectado, &head);

}

void obtener_info_restaurante(void) {
	int offset = 0;
	t_header * header = malloc(sizeof(t_header));
	t_solicitud_info_restaurante * solicitud_info = malloc(sizeof(t_solicitud_info_restaurante));

	solicitud_info->nombre_restaurante = nombre_restaurante;
	solicitud_info->size_nombre_restaurante = string_length(nombre_restaurante);

	int size_buffer = sizeof(uint32_t) + solicitud_info->size_nombre_restaurante;
	void * buffer = malloc(size_buffer);

	memcpy(buffer + offset, &solicitud_info->size_nombre_restaurante, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer+offset, solicitud_info->nombre_restaurante, solicitud_info->size_nombre_restaurante);

	header->payload = buffer;
	header->size = size_buffer;
	header->id_proceso = 2; //TODO: esta hardcodeado el 2, hay que modificarlo desp
	header->modulo = RESTAURANTE;
	header->nro_msg = OBTENER_RESTAURANTES;

	int conexion = crear_socket_y_conectar(ip_sindicato, puerto_sindicato);

	if (enviar_buffer(conexion, header) == false) {
		log_error(logger_restaurante, "No se pudo enviar el pedido de info del restaurante");
	}

	t_header * mensaje_recibido = recibir_buffer(conexion);

	log_info(logger_restaurante, "Se recibio mensaje del modulo: %d", mensaje_recibido->modulo);
	log_info(logger_restaurante, "Se recibio mensaje del modulo con id: %d", mensaje_recibido->id_proceso);
	log_info(logger_restaurante, "Se recibio el tipo de mensaje numero: %d", mensaje_recibido->nro_msg);
	log_info(logger_restaurante, "Se recibio un payload del tamaño: %d", mensaje_recibido->size);

	t_respuesta_info_restaurante * respuesta_info = deserializar_respuesta_info_restaurante(mensaje_recibido->payload);

	log_info(logger_restaurante, "Resturante cantidad cocineros: %d", respuesta_info->cantidad_cocineros);
	log_info(logger_restaurante, "Resturante posicion x: %d", respuesta_info->posicion_x);
	log_info(logger_restaurante, "Resturante posicion y: %d", respuesta_info->posicion_y);
	log_info(logger_restaurante, "Resturante afinidad cocineros: %s", respuesta_info->afinidad_cocineros);
	log_info(logger_restaurante, "Resturante platos: %s", respuesta_info->platos);
	log_info(logger_restaurante, "Resturante precio platos: %s", respuesta_info->precio_platos);
	log_info(logger_restaurante, "Resturante cantidad hornos: %d", respuesta_info->cantidad_hornos);
}



t_respuesta_info_restaurante * deserializar_respuesta_info_restaurante(void * payload) {
	t_respuesta_info_restaurante * respuesta_info = malloc(sizeof(t_respuesta_info_restaurante));

	memcpy(&(respuesta_info->cantidad_cocineros), payload, sizeof(uint32_t));
	payload += sizeof(uint32_t);

	memcpy(&(respuesta_info->posicion_x), payload, sizeof(uint32_t));
	payload += sizeof(uint32_t);

	memcpy(&(respuesta_info->posicion_y), payload, sizeof(uint32_t));
	payload += sizeof(uint32_t);

	memcpy(&(respuesta_info->size_afinidad_cocineros), payload, sizeof(uint32_t));
	payload += sizeof(uint32_t);

	respuesta_info->afinidad_cocineros = malloc((respuesta_info->size_afinidad_cocineros) + 1);
	memcpy((respuesta_info->afinidad_cocineros), payload, respuesta_info->size_afinidad_cocineros);
	payload += respuesta_info->size_afinidad_cocineros;

	memcpy(&(respuesta_info->size_platos), payload, sizeof(uint32_t));
	payload += sizeof(uint32_t);

	respuesta_info->platos = malloc((respuesta_info->size_platos) + 1);
	memcpy((respuesta_info->platos), payload, respuesta_info->size_platos);
	payload += respuesta_info->size_platos;

	memcpy(&(respuesta_info->size_precio_platos), payload, sizeof(uint32_t));
	payload += sizeof(uint32_t);

	respuesta_info->precio_platos = malloc((respuesta_info->size_precio_platos) + 1);
	memcpy((respuesta_info->precio_platos), payload, respuesta_info->size_precio_platos);
	payload += respuesta_info->size_precio_platos;

	memcpy(&(respuesta_info->cantidad_hornos), payload, sizeof(uint32_t));

	return respuesta_info;
}


