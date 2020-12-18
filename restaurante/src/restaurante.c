
#include "restaurante.h"

int main(void) {

	cargar_config();

	obtener_info_restaurante();

	iniciar_planificacion();

	//g_sockets_abiertos = list_create();

	//g_tiempo_reconexion = 3;

	//signal(SIGINT, sigint);

	// pthread_t  receptor_modulo_cliente_h;

	// pthread_create(&receptor_modulo_cliente_h, NULL, (void*)&conectar_restaurante_a_applicacion, (void*)NULL );

	//conectar_restaurante_a_applicacion();

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

	t_header * header_respuesta = malloc(sizeof(t_header));
	int offset;

	switch ( header_recibido->nro_msg ) {
	case CONSULTAR_PLATOS:
		// consulto al sindicato y lo que me devuelve el sindicato lo respondo a la app
		;
		t_respuesta_platos_restaurante * respuesta_platos = consultar_platos_restaurante();
		offset = 0;
		int size_buffer = sizeof(uint32_t) + respuesta_platos->size_platos;
		void * buffer = malloc(size_buffer);

		memcpy(buffer + offset, &respuesta_platos->size_platos, sizeof(uint32_t));
		offset += sizeof(uint32_t);

		memcpy(buffer+offset, respuesta_platos->platos, respuesta_platos->size_platos);

		header_respuesta->payload = buffer;
		header_respuesta->size = size_buffer;
		header_respuesta->id_proceso = 2; //TODO: esta hardcodeado el 2, hay que modificarlo desp
		header_respuesta->modulo = RESTAURANTE;
		header_respuesta->nro_msg = CONSULTAR_PLATOS;

		if (enviar_buffer( sock_conectado, header_respuesta) == false) {
			log_error(logger_restaurante, "No se pudo responder la consulta de platos a la app");
		}

		break;
	case CREAR_PEDIDO:
		// primero le mando al sindicato si el sindicato responde OK, le tengo que devolver un id a la app
		;
		uint32_t respuesta_creacion_pedido = crear_pedido_restaurante(id_pedido_restaurante);
		size_buffer = 0;
		if (respuesta_creacion_pedido == 1) {
			header_respuesta->nro_msg = OK;
		}
		else header_respuesta->nro_msg = FAIL;
		header_respuesta->payload = NULL;
		header_respuesta->size = size_buffer;
		header_respuesta->id_proceso = 2; //TODO: esta hardcodeado el 2, hay que modificarlo desp
		header_respuesta->modulo = RESTAURANTE;

		if (enviar_buffer( sock_conectado, header_respuesta) == false) {
			log_error(logger_restaurante, "No se pudo responder la creacion de pedido a la app");
		}
		break;
	case ANIADIR_PLATO:
		// primero le mando al sindicato si el sindicato responde Ok, le tengo que devolver un OK o fail a la app
		;
		t_aniadir_plato_app_resto * plato_app_resto = deserializar_aniadir_plato_app(header_recibido->payload);
		aniadir_plato * aniadir_plato = malloc(sizeof(aniadir_plato));
		aniadir_plato->id_pedido = plato_app_resto->id_pedido;
		aniadir_plato->size_nombre_restaurante = string_length(nombre_restaurante);
		aniadir_plato->nombre_restaurante = nombre_restaurante;
		aniadir_plato->size_nombre_plato = string_length(plato_app_resto->plato);
		aniadir_plato->nombre_restaurante =plato_app_resto->plato;
		uint32_t respuesta_aniadir_plato = aniadir_plato_restaurante(aniadir_plato);

		size_buffer = 0;
		if (respuesta_creacion_pedido == 1) {
			header_respuesta->nro_msg = OK;
		}
		else header_respuesta->nro_msg = FAIL;
		header_respuesta->payload = NULL;
		header_respuesta->size = size_buffer;
		header_respuesta->id_proceso = 2; //TODO: esta hardcodeado el 2, hay que modificarlo desp
		header_respuesta->modulo = RESTAURANTE;

		if (enviar_buffer( sock_conectado, header_respuesta) == false) {
			log_error(logger_restaurante, "No se pudo responder el aniadir plato a la app");
		}

		break;
	case CONFIRMAR_PEDIDO:
		// La app me manda el id yo lo confirmo le mando al sindicato, el responde OK o fail y lo informo a la app
		break;
	case CONSULTAR_PEDIDO:
		// La app me manda la consulta y yo se reenvio al sindicato. Cuando este responde se lo devuelvo a la app

		break;
	case CONECTAR:
		// responde la app para avisar que se conecto todo OK
		header_respuesta->payload = NULL;
		header_respuesta->size = size_buffer;
		header_respuesta->id_proceso = 2; //TODO: esta hardcodeado el 2, hay que modificarlo desp
		header_respuesta->modulo = RESTAURANTE;
		header_respuesta->nro_msg = OK;
		break;
	default:
		printf("Mensaje no compatible con módulo RESTAURANTE.\n");
	}

	if ( header_recibido->size > 0 || header_recibido->payload != NULL )

		free( header_recibido->payload );

	free(header_recibido);
	free(header_respuesta);

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

	cargar_variables(respuesta_info);
}

void iniciar_planificacion() {
	colas_ready = list_create();
	cola_io = queue_create();
	cola_bloqueados = queue_create();
	cola_exit = queue_create();

	cargar_colas_ready();


	sem_init(sem_hornos, 0, cantidad_hornos);

}

void cargar_colas_ready() {
	int cola_creada;
	for(int i = 0; i < list_size(cocineros); ++i) {
		char * cocinero = list_get(cocineros, i);
		if (cocinero != "OTRO") {
			t_queue * cola_ready = queue_create();
			t_dictionary * cola_ready_afinidad = dictionary_create();
			dictionary_put(cola_ready_afinidad, cocinero, cola_ready);
			list_add(colas_ready, cola_ready_afinidad);
		}
		else {
			if (cola_creada == 0) {
				cola_creada = 1;
				t_queue * cola_ready = queue_create();
				t_dictionary * cola_ready_afinidad = dictionary_create();
				dictionary_put(cola_ready_afinidad, cocinero, cola_ready);
				list_add(colas_ready, cola_ready_afinidad);
			}
		}
	}
}

void planificador_cpu(char * afinidad) {
	while (1) {
		t_queue * cola_ready = obtener_cola_afinidad(afinidad);
		if (queue_size(cola_ready) != 0) {
			pcb_plato * plato = queue_pop(cola_ready);
			t_dictionary * paso = list_get(plato->receta_faltante, 0);
			uint32_t cantidad_cpu = dictionary_get(paso, "HORNEAR");
			if(cantidad_cpu != NULL) {
				for(int i = 0; i < cantidad_cpu; ++i ) {
					log_info(logger_restaurante, "CPU -- plato: %s, realizando %d de %d", plato->nombre_plato, i, cantidad_cpu);
				}
			}
			list_remove(plato->receta_faltante, 0);
			t_dictionary * paso_nuevo = list_get(plato->receta_faltante, 0);
			if (paso_nuevo == NULL) {
				queue_push(cola_exit, plato);
			}
			else if (dictionary_has_key(paso_nuevo, "REPOSAR")) {
				queue_push(cola_bloqueados, plato);
			}
			else {
				t_queue * cola_ready;
				int existe_cola_ready(void * p) {
					dictionary_has_key(p, plato->nombre_plato);
				}
				cola_ready = list_find(colas_ready, (void *) existe_cola_ready);
				if (cola_ready == NULL) {
					int existe_cola_otro(void *p) {
						dictionary_has_key(p, "OTRO");
					}
					cola_ready = list_find(colas_ready, (void *) existe_cola_otro);
				}
				queue_push(cola_ready, plato);
			}
		}
	}
}

void planificador_io() {
	while (1) {
		if (queue_size(cola_io) != 0) {
			sem_wait(sem_hornos);
			pcb_plato * plato = queue_pop(cola_io);
			t_dictionary * paso = list_get(plato->receta_faltante, 0);
			uint32_t cantidad_io = dictionary_get(paso, "HORNEAR");
			if(cantidad_io != NULL) {
				for(int i = 0; i < cantidad_io; ++i ) {
					log_info(logger_restaurante, "I/0 -- plato: %s, realizando %d de %d", plato->nombre_plato, i, cantidad_io);
				}
			}
			list_remove(plato->receta_faltante, 0);
			t_dictionary * paso_nuevo = list_get(plato->receta_faltante, 0);
			if (paso_nuevo == NULL) {
				queue_push(cola_exit, plato);
			}
			else if (dictionary_has_key(paso_nuevo, "REPOSAR")) {
				queue_push(cola_bloqueados, plato);
			}
			else {
				t_queue * cola_ready;
				int existe_cola_ready(void * p) {
					dictionary_has_key(p, plato->nombre_plato);
				}
				cola_ready = list_find(colas_ready, (void *) existe_cola_ready);
				if (cola_ready == NULL) {
					int existe_cola_otro(void *p) {
						dictionary_has_key(p, "OTRO");
					}
					cola_ready = list_find(colas_ready, (void *) existe_cola_otro);
				}
				queue_push(cola_ready, plato);
			}
			sem_post(sem_hornos);
		}
	}
}

void planificador_bloqueados() {
	while (1) {
		if (queue_size(cola_bloqueados) != 0) {
			pcb_plato * plato = queue_pop(cola_bloqueados);
			t_dictionary * paso = list_get(plato->receta_faltante, 0);
			uint32_t cantidad_reposar = dictionary_get(paso, "REPOSAR");
			if(cantidad_reposar != NULL) {
				for(int i = 0; i < cantidad_reposar; ++i ) {
					log_info(logger_restaurante, "REPOSAR -- plato: %s, realizando %d de %d", plato->nombre_plato, i, cantidad_reposar);
				}
			}
			list_remove(plato->receta_faltante, 0);
			t_dictionary * paso_nuevo = list_get(plato->receta_faltante, 0);
			if (paso_nuevo == NULL) {
				queue_push(cola_exit, plato);
			}
			else if (dictionary_has_key(paso, "REPOSAR")) {
				queue_push(cola_bloqueados, plato);
			}
			else {
				t_queue * cola_ready;
				int existe_cola_ready(void * p) {
					dictionary_has_key(p, plato->nombre_plato);
				}
				cola_ready = list_find(colas_ready, (void *) existe_cola_ready);
				if (cola_ready == NULL) {
					int existe_cola_otro(void *p) {
						dictionary_has_key(p, "OTRO");
					}
					cola_ready = list_find(colas_ready, (void *) existe_cola_otro);
				}
				queue_push(cola_ready, plato);
			}
		}
	}
}

t_queue * obtener_cola_afinidad(char * afinidad) {
	t_queue * cola_ready;
	int existe_cola_ready(void * p) {
		dictionary_has_key(p, afinidad);
	}
	cola_ready = list_find(colas_ready, (void *) existe_cola_ready);
	if (cola_ready == NULL) {
		int existe_cola_otro(void *p) {
			dictionary_has_key(p, "OTRO");
		}
		cola_ready = list_find(colas_ready, (void *) existe_cola_otro);
	}
	return cola_ready;
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

t_respuesta_platos_restaurante * consultar_platos_restaurante() {
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
	header->nro_msg = CONSULTAR_PLATOS;

	int conexion = crear_socket_y_conectar(ip_sindicato, puerto_sindicato);

	if (enviar_buffer(conexion, header) == false) {
		log_error(logger_restaurante, "No se pudo enviar el pedido de info del restaurante");
	}

	t_header * mensaje_recibido = recibir_buffer(conexion);

	log_info(logger_restaurante, "Se recibio mensaje del modulo: %d", mensaje_recibido->modulo);
	log_info(logger_restaurante, "Se recibio mensaje del modulo con id: %d", mensaje_recibido->id_proceso);
	log_info(logger_restaurante, "Se recibio el tipo de mensaje numero: %d", mensaje_recibido->nro_msg);
	log_info(logger_restaurante, "Se recibio un payload del tamaño: %d", mensaje_recibido->size);

	t_respuesta_platos_restaurante * respuesta_info = deserializar_respuesta_info_restaurante(mensaje_recibido->payload);

	log_info(logger_restaurante, "Resturante platos: %s", respuesta_info->platos);

	return 	respuesta_info;
}

void cargar_variables(t_respuesta_info_restaurante * respuesta) {
	char ** platos_array = string_get_string_as_array(respuesta->platos);
	int i = 0;
	while (platos_array[i] != NULL) {
		list_add(platos, platos_array[i]);
		i++;
	}

	char ** cocineros_array = string_get_string_as_array(respuesta->afinidad_cocineros);
	for(i = 0; i < respuesta->cantidad_cocineros; ++i) {
		if(cocineros_array[i] == NULL) {
			list_add(cocineros, cocineros_array[i]);
		}
		else {
			list_add(cocineros, "OTRO");
		}
	}

	posicion_x = respuesta->posicion_x;
	posicion_y = respuesta->posicion_y;
	cantidad_hornos = respuesta->cantidad_hornos;

	char ** precios_array = string_get_string_as_array(respuesta->platos);
	i = 0;
	while (precios_array[i] != NULL) {
		dictionary_put(platos_precios, list_get(platos, i), precios_array[i]);
		i++;
	}
}

t_respuesta_platos_restaurante * deserializar_respuesta_consulta_platos(void * payload) {
	t_respuesta_platos_restaurante * respuesta_info = malloc(sizeof(t_respuesta_platos_restaurante));

	memcpy(&(respuesta_info->size_platos), payload, sizeof(uint32_t));
	payload += sizeof(uint32_t);

	respuesta_info->platos = malloc((respuesta_info->size_platos) + 1);
	memcpy((respuesta_info->platos), payload, respuesta_info->size_platos);
	payload += respuesta_info->size_platos;

	return respuesta_info;
}

uint32_t crear_pedido_restaurante(uint32_t id_pedido) {
	int offset = 0;
	t_header * header = malloc(sizeof(t_header));

	t_creacion_pedido * creacion_pedido = malloc(sizeof(t_creacion_pedido));

	creacion_pedido->id_pedido = id_pedido;
	creacion_pedido->size_estado_pedido = string_length("CREANDO");
	creacion_pedido->estado_pedido = "CREANDO";
	creacion_pedido->size_lista_platos = string_length("[]");
	creacion_pedido->lista_platos = "[]";
	creacion_pedido->size_cantidad_platos = string_length("[]");
	creacion_pedido->cantidad_platos = "[]";
	creacion_pedido->precio_total = 0;

	int size_buffer = sizeof(uint32_t) * 5 + creacion_pedido->size_cantidad_platos + creacion_pedido->size_estado_pedido
			+ creacion_pedido->size_lista_platos;
	void * buffer = malloc(size_buffer);

	memcpy(buffer + offset, &creacion_pedido->id_pedido, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer + offset, &creacion_pedido->size_estado_pedido, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer + offset, creacion_pedido->estado_pedido, creacion_pedido->size_estado_pedido);
	offset += creacion_pedido->size_estado_pedido;

	memcpy(buffer + offset, &creacion_pedido->size_lista_platos, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer + offset, creacion_pedido->lista_platos, creacion_pedido->size_lista_platos);
	offset += creacion_pedido->size_lista_platos;

	memcpy(buffer + offset, &creacion_pedido->size_cantidad_platos, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer + offset, creacion_pedido->cantidad_platos, creacion_pedido->size_cantidad_platos);
	offset += creacion_pedido->size_cantidad_platos;

	memcpy(buffer + offset, &creacion_pedido->precio_total, sizeof(uint32_t));

	header->payload = buffer;
	header->size = size_buffer;
	header->id_proceso = 2; //TODO: esta hardcodeado el 2, hay que modificarlo desp
	header->modulo = RESTAURANTE;
	header->nro_msg = GUARDAR_PEDIDO;

	int conexion = crear_socket_y_conectar(ip_sindicato, puerto_sindicato);

	if (enviar_buffer(conexion, header) == false) {
		log_error(logger_restaurante, "No se pudo enviar el pedido de info del restaurante");
	}

	t_header * mensaje_recibido = recibir_buffer(conexion);

	log_info(logger_restaurante, "Se recibio mensaje del modulo: %d", mensaje_recibido->modulo);
	log_info(logger_restaurante, "Se recibio mensaje del modulo con id: %d", mensaje_recibido->id_proceso);
	log_info(logger_restaurante, "Se recibio el tipo de mensaje numero: %d", mensaje_recibido->nro_msg);
	log_info(logger_restaurante, "Se recibio un payload del tamaño: %d", mensaje_recibido->size);

	uint32_t respuesta_creacion_pedido = deserializar_respuesta_creacion_pedido(mensaje_recibido->payload);

	if (respuesta_creacion_pedido == 1) {

		pedido * pedido = malloc(sizeof(pedido));
		pedido->id_pedido = id_pedido;
		pedido->estado_pedido = "CREADO";
		list_add(pedidos_creados, pedido);
	}

	log_info(logger_restaurante, "Respuesta creacion pedido: %d", respuesta_creacion_pedido);

	return respuesta_creacion_pedido;

}

uint32_t deserializar_respuesta_creacion_pedido(void * payload) {
	int respuesta;

	memcpy(&(respuesta), payload, sizeof(uint32_t));

	return respuesta;
}

uint32_t aniadir_plato_restaurante(aniadir_plato * plato) {
	int offset = 0;

	t_header * header = malloc(sizeof(t_header));
	t_guardar_plato_restaurante * creacion_pedido = malloc(sizeof(t_guardar_plato_restaurante));

	creacion_pedido->id_pedido = plato->id_pedido;
	creacion_pedido->size_nombre_plato = string_length(plato->nombre_plato);
	creacion_pedido->nombre_plato = plato->nombre_plato;
	creacion_pedido->cantidad_plato = 1;

	int size_buffer = sizeof(uint32_t) * 3 + creacion_pedido->size_nombre_plato;
	void * buffer = malloc(size_buffer);

	memcpy(buffer + offset, &creacion_pedido->id_pedido, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer + offset, &creacion_pedido->size_nombre_plato, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer + offset, creacion_pedido->nombre_plato, creacion_pedido->size_nombre_plato);
	offset += creacion_pedido->size_nombre_plato;

	memcpy(buffer + offset, &creacion_pedido->cantidad_plato, sizeof(uint32_t));

	header->payload = buffer;
	header->size = size_buffer;
	header->id_proceso = 2; //TODO: esta hardcodeado el 2, hay que modificarlo desp
	header->modulo = RESTAURANTE;
	header->nro_msg = GUARDAR_PLATO;

	int conexion = crear_socket_y_conectar(ip_sindicato, puerto_sindicato);

	if (enviar_buffer(conexion, header) == false) {
		log_error(logger_restaurante, "No se pudo enviar el pedido de info del restaurante");
	}

	t_header * mensaje_recibido = recibir_buffer(conexion);

	log_info(logger_restaurante, "Se recibio mensaje del modulo: %d", mensaje_recibido->modulo);
	log_info(logger_restaurante, "Se recibio mensaje del modulo con id: %d", mensaje_recibido->id_proceso);
	log_info(logger_restaurante, "Se recibio el tipo de mensaje numero: %d", mensaje_recibido->nro_msg);
	log_info(logger_restaurante, "Se recibio un payload del tamaño: %d", mensaje_recibido->size);

	uint32_t respuesta_creacion_pedido = deserializar_respuesta_creacion_pedido(mensaje_recibido->payload);

	log_info(logger_restaurante, "Respuesta aniadir plato: %d", respuesta_creacion_pedido);

	pedido * pedido = busca_pedido_en_lista(plato->id_pedido);

	if (pedido != NULL) {
		plato_cantidad * plato_cant = busca_plato_en_pedido(pedido, plato->nombre_plato);
		if(plato_cant != NULL) plato_cant->cantidad++;
		else {
			plato_cantidad * plato_agregar = malloc(sizeof(plato_cantidad));
			plato_agregar->plato = plato->nombre_plato;
			plato_agregar->cantidad = 1;
			list_add(pedido->lista_platos, plato_agregar);
		}
	}

	return respuesta_creacion_pedido;
}

void confirmar_pedido(int id_pedido) {
	t_creacion_pedido * pedido = obtener_pedido(id_pedido);

}

t_creacion_pedido * obtener_pedido(int id_pedido) {
	int offset = 0;
	t_header * header = malloc(sizeof(t_header));

	int size_buffer = sizeof(uint32_t);
	void * buffer = malloc(size_buffer);

	memcpy(buffer + offset, id_pedido, sizeof(uint32_t));

	header->payload = buffer;
	header->size = size_buffer;
	header->id_proceso = 2; //TODO: esta hardcodeado el 2, hay que modificarlo desp
	header->modulo = RESTAURANTE;
	header->nro_msg = OBTENER_PEDIDO;

	int conexion = crear_socket_y_conectar(ip_sindicato, puerto_sindicato);

	if (enviar_buffer(conexion, header) == false) {
		log_error(logger_restaurante, "No se pudo enviar el pedido de info del restaurante");
	}

	t_header * mensaje_recibido = recibir_buffer(conexion);

	log_info(logger_restaurante, "Se recibio mensaje del modulo: %d", mensaje_recibido->modulo);
	log_info(logger_restaurante, "Se recibio mensaje del modulo con id: %d", mensaje_recibido->id_proceso);
	log_info(logger_restaurante, "Se recibio el tipo de mensaje numero: %d", mensaje_recibido->nro_msg);
	log_info(logger_restaurante, "Se recibio un payload del tamaño: %d", mensaje_recibido->size);

	t_creacion_pedido * respuesta_obtener_pedido = deserializar_respuesta_obtener_pedido_restaurante(mensaje_recibido->payload);

	return respuesta_obtener_pedido;
}

t_creacion_pedido * deserializar_respuesta_obtener_pedido_restaurante(void * payload) {
	t_creacion_pedido * respuesta_info = malloc(sizeof(t_creacion_pedido));

	memcpy(&(respuesta_info->id_pedido), payload, sizeof(uint32_t));
	payload += sizeof(uint32_t);

	memcpy(&(respuesta_info->size_estado_pedido), payload, sizeof(uint32_t));
	payload += sizeof(uint32_t);

	respuesta_info->estado_pedido = malloc((respuesta_info->size_estado_pedido) + 1);
	memcpy((respuesta_info->estado_pedido), payload, respuesta_info->size_estado_pedido);
	payload += respuesta_info->size_estado_pedido;

	memcpy(&(respuesta_info->size_lista_platos), payload, sizeof(uint32_t));
	payload += sizeof(uint32_t);

	respuesta_info->lista_platos = malloc((respuesta_info->size_lista_platos) + 1);
	memcpy((respuesta_info->lista_platos), payload, respuesta_info->size_lista_platos);
	payload += respuesta_info->size_lista_platos;

	memcpy(&(respuesta_info->size_cantidad_platos), payload, sizeof(uint32_t));
	payload += sizeof(uint32_t);

	respuesta_info->cantidad_platos = malloc((respuesta_info->size_cantidad_platos) + 1);
	memcpy((respuesta_info->cantidad_platos), payload, respuesta_info->size_cantidad_platos);
	payload += respuesta_info->size_cantidad_platos;

	memcpy(&(respuesta_info->precio_total), payload, sizeof(uint32_t));
	payload += sizeof(uint32_t);

	return respuesta_info;
}

t_respuesta_receta * obtener_receta(char * plato) {
	int offset = 0;
	t_header * header = malloc(sizeof(t_header));
	t_info_receta * info_receta = malloc(sizeof(t_info_receta));

	info_receta->size_plato = string_length(plato);
	info_receta->plato = plato;


	int size_buffer = sizeof(uint32_t) + info_receta->size_plato;
	void * buffer = malloc(size_buffer);

	memcpy(buffer + offset, &info_receta->size_plato, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer + offset, info_receta->plato, info_receta->size_plato);
	offset += info_receta->size_plato;

	header->payload = buffer;
	header->size = size_buffer;
	header->id_proceso = 2; //TODO: esta hardcodeado el 2, hay que modificarlo desp
	header->modulo = RESTAURANTE;
	header->nro_msg = OBTENER_RECETA;

	int conexion = crear_socket_y_conectar(ip_sindicato, puerto_sindicato);

	if (enviar_buffer(conexion, header) == false) {
		log_error(logger_restaurante, "No se pudo enviar el pedido de info del restaurante");
	}

	t_header * mensaje_recibido = recibir_buffer(conexion);

	log_info(logger_restaurante, "Se recibio mensaje del modulo: %d", mensaje_recibido->modulo);
	log_info(logger_restaurante, "Se recibio mensaje del modulo con id: %d", mensaje_recibido->id_proceso);
	log_info(logger_restaurante, "Se recibio el tipo de mensaje numero: %d", mensaje_recibido->nro_msg);
	log_info(logger_restaurante, "Se recibio un payload del tamaño: %d", mensaje_recibido->size);

	t_respuesta_receta * respuesta_obtener_pedido = deserializar_respuesta_receta(mensaje_recibido->payload);

	return respuesta_obtener_pedido;
}

t_respuesta_receta * deserializar_respuesta_receta(void * payload) {
	t_respuesta_receta * respuesta_info = malloc(sizeof(t_respuesta_receta));

	memcpy(&(respuesta_info->size_pasos), payload, sizeof(uint32_t));
	payload += sizeof(uint32_t);

	respuesta_info->pasos = malloc((respuesta_info->size_pasos) + 1);
	memcpy((respuesta_info->pasos), payload, respuesta_info->size_pasos);
	payload += respuesta_info->size_pasos;

	memcpy(&(respuesta_info->size_tiempos), payload, sizeof(uint32_t));
	payload += sizeof(uint32_t);

	respuesta_info->tiempos = malloc((respuesta_info->size_tiempos) + 1);
	memcpy((respuesta_info->tiempos), payload, respuesta_info->size_tiempos);
	payload += respuesta_info->size_tiempos;

	return respuesta_info;
}

t_aniadir_plato_app_resto * deserializar_aniadir_plato_app(void * payload) {

	t_aniadir_plato_app_resto * plato = malloc(sizeof(t_aniadir_plato_app_resto));

	memcpy(&(plato->id_pedido), payload, sizeof(uint32_t));
	payload += sizeof(uint32_t);

	memcpy(&(plato->size_plato), payload, sizeof(uint32_t));
	payload += sizeof(uint32_t);

	plato->plato = malloc(sizeof(plato->size_plato) + 1);
	memcpy((plato->plato), payload, plato->size_plato);

	return plato;
}

pedido * busca_pedido_en_lista(uint32_t id_pedido) {
	bool _detecta_pedido_en_lista(void * p_elem) {
		return (((pedido *)p_elem)->id_pedido == id_pedido );
	}

	pedido * pedido = list_find(pedidos_creados, _detecta_pedido_en_lista);

	return pedido;
}

plato_cantidad * busca_plato_en_pedido(pedido * pedido, char * plato) {
	bool _detecta_plato_en_lista(void * p_elem) { // detecta si el restaurante está disponible en la APP
		return string_equals_ignore_case( ((plato_cantidad*)p_elem)->plato , plato );
	}

	plato_cantidad * plato_cantidad = list_find(pedido->lista_platos, _detecta_plato_en_lista);

	return plato_cantidad;
}



