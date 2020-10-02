
#include "restaurante.h"

int main(void) {

	cargar_config();

	g_sockets_abiertos = list_create();

	g_tiempo_reconexion = 3;

	signal(SIGINT, sigint);

	// obtener_info_restaurante();

	// pthread_t  receptor_modulo_cliente_h;

	// pthread_create(&receptor_modulo_cliente_h, NULL, (void*)&conectar_restaurante_a_applicacion, (void*)NULL );

	conectar_restaurante_a_applicacion();

	return 1;

}

void conectar_restaurante_a_applicacion(void) {

	uint32_t despla = 0;
	uint32_t resto_nombre_size = string_length(nombre_restaurante);
	uint32_t buffer_size = sizeof(uint32_t) * 3 + resto_nombre_size ;
	void * payload = malloc(buffer_size);

	memcpy( payload + despla, &g_pos_x, sizeof(uint32_t) );
	despla += sizeof(uint32_t);

	memcpy( payload + despla, &g_pos_y, sizeof(uint32_t) );
	despla += sizeof(uint32_t);

	memcpy( payload + despla, &resto_nombre_size, sizeof(uint32_t) );
	despla += sizeof(uint32_t);

	memcpy( payload + despla, nombre_restaurante, resto_nombre_size );
	despla += string_length(nombre_restaurante);

	uint32_t sock_conectado = crear_socket_y_conectar( ip_app, puerto_app );

	g_socket_cliente = sock_conectado;

	if ( -1 == sock_conectado ) exit(-1);

	t_header head;

	head.id_proceso = g_id_proceso;
	head.modulo     = RESTAURANTE;
	head.nro_msg    = CONECTAR;
	head.size       = buffer_size;
	head.payload    = payload;

	free(payload);

	enviar_buffer( sock_conectado, &head);

	while (1) {

		recibir_buffer( sock_conectado );

	}










	t_header * header_recibido = recibir_buffer( sock_conectado );

	printf( "Módulo:       %d.\n" , header_recibido->modulo     );
	printf( "ID Proceso:   %d.\n" , header_recibido->id_proceso );
	printf( "Nro. mensaje: %s.\n" , nro_comando_a_texto( header_recibido->nro_msg )   );
	printf( "Bytes:        %d.\n" , header_recibido->size       );

	mem_hexdump(header_recibido->payload, header_recibido->size);

	switch ( header_recibido->nro_msg ) {
	case CONSULTAR_PLATOS:
		// responder_consultar_platos( sock_conectado, g_platos_default );
		break;
	case CREAR_PEDIDO:
		break;
	case ANIADIR_PLATO:
		break;
	case CONFIRMAR_PEDIDO:
		break;
	case PLATO_LISTO:
		break;
	case CONSULTAR_PEDIDO:
		break;
	case CONECTAR:
		break;
	default:
		printf("Mensaje no compatible con módulo RESTAURANTE.\n");
	}

	if ( header_recibido->size > 0 || header_recibido->payload != NULL )

		free( header_recibido->payload );

	free( header_recibido );






	close( sock_conectado );

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


