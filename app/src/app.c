
#include "app.h"

int main(void) {

	lista_asociaciones_cliente_resto = list_create();

	lista_resto_conectados  = list_create();

	g_sockets_abiertos      = list_create();

	g_cola_nuevos = queue_create();

	signal(SIGINT, sigint);

	logger = log_create("app.log","APP",1,LOG_LEVEL_INFO);
	config = leer_config();

	pthread_create(&g_thread_long_term_scheduler  , NULL, (void*) &long_term_scheduler, (void*) NULL  );

	// short_term_scheduler();

	// lanzar planificador a largo plazo
	// lanzar planificador a mediano plazo
	// lanzar planificador a corto plazo
	// lanzar repartidores

	g_socket_cliente = crear_socket_escucha("127.0.0.1", g_puerto_escucha );

	while (1) {

		uint32_t socket_aceptado = aceptar_conexion( g_socket_cliente );

		uint32_t * _socket_aceptado = malloc( sizeof(uint32_t) );

		memcpy( _socket_aceptado, &socket_aceptado, sizeof(uint32_t) );

		if ( socket_aceptado <= 0) {

			perror("Error al aceptar el socket.");
			exit(-1);

		}

		list_add( g_sockets_abiertos, _socket_aceptado );

		pthread_t  thread_app;

		pthread_create(&thread_app,NULL,(void*)&procesamiento_mensaje,(void*)(&socket_aceptado));

	}

}

t_config * leer_config(void) {

	t_config * config = config_create("app.config");

	if ( config_has_property( config, "IP_COMANDA"                  ) ) g_ip_comanda                  = config_get_string_value(config, "IP_COMANDA");
	if ( config_has_property( config, "PUERTO_COMANDA"              ) ) g_puerto_comanda              = config_get_int_value(config, "PUERTO_COMANDA");
	if ( config_has_property( config, "PUERTO_ESCUCHA"              ) ) g_puerto_escucha              = config_get_string_value(config, "PUERTO_ESCUCHA");
	if ( config_has_property( config, "RETARDO_CICLO_CPU"           ) ) g_retardo_ciclo_cpu           = config_get_int_value(config, "RETARDO_CICLO_CPU");
	if ( config_has_property( config, "GRADO_DE_MULTIPROCESAMIENTO" ) ) g_grado_de_multiprocesamiento = config_get_int_value(config, "GRADO_DE_MULTIPROCESAMIENTO");

	if ( config_has_property( config, "ALGORITMO_DE_PLANIFICACION"  ) ) g_algoritmo_de_planificacion  = config_get_int_value(config, "ALGORITMO_DE_PLANIFICACION");
	if ( config_has_property( config, "ALPHA"                       ) ) g_alpha                       = config_get_int_value(config, "ALPHA");
	if ( config_has_property( config, "ESTIMACION_INICIAL"          ) ) g_estimacion_inicial          = config_get_int_value(config, "ESTIMACION_INICIAL");
	if ( config_has_property( config, "REPARTIDORES"                ) ) g_repartidores                = config_get_array_value(config, "REPARTIDORES");
	if ( config_has_property( config, "FRECUENCIA_DE_DESCANSO"      ) ) g_frecuencia_de_descanso      = config_get_array_value(config, "FRECUENCIA_DE_DESCANSO");

	if ( config_has_property( config, "TIEMPO_DE_DESCANSO"          ) ) g_tiempo_de_descanso          = config_get_array_value(config, "TIEMPO_DE_DESCANSO");
	if ( config_has_property( config, "ARCHIVO_LOG"                 ) ) g_log_path                    = config_get_string_value(config, "ARCHIVO_LOG");
	if ( config_has_property( config, "PLATOS_DEFAULT"              ) ) g_platos_default              = config_get_array_value(config, "PLATOS_DEFAULT");
	if ( config_has_property( config, "POSICION_REST_DEFAULT_X"     ) ) g_posicion_rest_default_x     = config_get_int_value(config, "POSICION_REST_DEFAULT_X");
	if ( config_has_property( config, "POSICION_REST_DEFAULT_Y"     ) ) g_posicion_rest_default_y     = config_get_int_value(config, "POSICION_REST_DEFAULT_Y");

	t_info_restarante * resto_default = malloc( sizeof(t_info_restarante) );

	resto_default->posx = g_posicion_rest_default_x;
	resto_default->posy = g_posicion_rest_default_y;
	resto_default->resto_nombre     = "default";
	resto_default->socket_conectado = 0;
	resto_default->list_platos = g_platos_default;

	list_add( lista_resto_conectados, resto_default );

	return config;

}

void mostrar_info_pcb_repartidor ( t_pcb_repartidor * p_repa ) {

	printf("\nInformación del repartidor '%d'.\n", p_repa->id_repartidor );
	printf("Se encuentra posicionado en ( %d, %d ).\n", p_repa->pos_x, p_repa->pos_y );
	printf("Descansa cada %d metros un tiempo de %d minutos.\n", p_repa->freq_descanso, p_repa->tiempo_descanso );

}

void procesamiento_mensaje( void * p_socket_aceptado ) {

	void * _aux_nombres_restos ( void * p_elem ) {

		return ((t_info_restarante *) p_elem)->resto_nombre;

	}

	uint32_t socket_aceptado = (uint32_t)(* ( (int*) p_socket_aceptado ));

	t_header * header_recibido = recibir_buffer( socket_aceptado );

	printf( "Módulo:       %d.\n" , header_recibido->modulo     );
	printf( "ID Proceso:   %d.\n" , header_recibido->id_proceso );
	printf( "Nro. mensaje: %s.\n" , nro_comando_a_texto( header_recibido->nro_msg )   );
	printf( "Bytes:        %d.\n" , header_recibido->size       );

	mem_hexdump(header_recibido->payload, header_recibido->size);

	switch ( header_recibido->nro_msg ) {
	case CONSULTAR_RESTAURANTES: ;
		t_list * nombres_restos = list_map( lista_resto_conectados, _aux_nombres_restos );
		responder_01_consultar_restaurantes ( socket_aceptado, nombres_restos );
		list_destroy( nombres_restos );
		break;
	case SELECCIONAR_RESTAURANTE: ;
		bool seleccionado = procedimiento_02_seleccionar_restaurante( header_recibido );
		responder_seleccionar_restaurante( socket_aceptado, seleccionado );
		break;
	case CONSULTAR_PLATOS: ;
		char ** platos = procedimiento_04_consultar_platos( header_recibido );
		responder_consultar_platos( socket_aceptado, platos );
		break;
	case CREAR_PEDIDO: ;
		// "CREAR_PEDIDO" Hacia Restaurante ( y "GUARDAR_PEDIDO" Hacia Sindicato).
		// "GUARDAR_PEDIDO" Hacia comanda.
		uint32_t id_ped_creado = procedimiento_05_crear_pedido( header_recibido );
		printf( "Se obtuvo el ID '%d'.\n", id_ped_creado );
		responder_05_crear_pedido( socket_aceptado, id_ped_creado );
		break;
	case ANIADIR_PLATO:
		// "ANIADIR_PLATO" Hacia Restaurante.
		// "GUARDAR_PLATO" Hacia comanda.
		responder_07_aniadir_plato( socket_aceptado, true );
		break;
	case CONFIRMAR_PEDIDO:
		break;
	case PLATO_LISTO:
		break;
	case CONSULTAR_PEDIDO:
		break;
	case CONECTAR:

		printf("tratamiento de conexión.....\n");

		manejar_restaurante_conectado(header_recibido, socket_aceptado);

		t_header aux_head;

		aux_head.id_proceso = 99999;
		aux_head.modulo = APP;
		aux_head.nro_msg = CONECTAR;
		aux_head.size = 0;
		aux_head.payload = NULL;

		enviar_buffer( socket_aceptado, &aux_head );

		while ( recv( socket_aceptado, NULL, 0, MSG_PEEK | MSG_DONTWAIT ) != 0 ) {

			bucle_resto_conectado( socket_aceptado );

		}

		break;
	default:
		printf("Mensaje no compatible con módulo APP.\n");
	}

	if ( header_recibido->size > 0 || header_recibido->payload != NULL )

		free( header_recibido->payload );

	free( header_recibido );

	close( socket_aceptado );

}

void manejar_restaurante_conectado( t_header * header_recibido, uint32_t p_socket_aceptado ) {

	t_info_restarante * l_resto = malloc( sizeof(l_resto) );
	uint32_t despla = 0;

	memcpy( &l_resto->posx, header_recibido->payload + despla, sizeof(uint32_t) );
	despla += sizeof(uint32_t);

	memcpy( &l_resto->posy, header_recibido->payload + despla, sizeof(uint32_t) );
	despla += sizeof(uint32_t);

	uint32_t resto_nombre_size = 0;

	memcpy( &resto_nombre_size, header_recibido->payload + despla, sizeof(uint32_t) );
	despla += sizeof(uint32_t);

	l_resto->resto_nombre = malloc( resto_nombre_size +1 );

	memcpy( l_resto->resto_nombre, header_recibido->payload + despla, resto_nombre_size );
	despla += sizeof(uint32_t);

	l_resto->resto_nombre[resto_nombre_size] = '\0';

	l_resto->socket_conectado = p_socket_aceptado;

	printf("Se incorporó el restaurante '%s' en la posición (%d,%d) del mapa.\n", l_resto->resto_nombre, l_resto->posx, l_resto->posy);

	list_add( lista_resto_conectados, l_resto );


}

void bucle_resto_conectado ( uint32_t sock_aceptado ) {

	sem_wait( &g_semaphore_envios_resto );
	// list_iterate(p_msg_queue, _control_mensaje_individual);
	sleep(5);
	sem_post( &g_semaphore_envios_resto );

}

void long_term_scheduler( void ) {

	sem_init( &g_nro_pedidos_confirmados , 0, 0 ); // 1 CPU

	uint32_t posicion = 0;

	while ( g_repartidores           [posicion] != NULL
		 && g_frecuencia_de_descanso [posicion] != NULL
		 && g_tiempo_de_descanso     [posicion] != NULL ) {

		t_pcb_repartidor * l_repartidor = malloc( sizeof(t_pcb_repartidor) );

		char ** repartidor_posiciones = (char **) string_split ( g_repartidores[posicion] , "|" );

		l_repartidor->id_repartidor = random_id_generator();
		l_repartidor->pos_x = (uint32_t) atoi( repartidor_posiciones[0] );
		l_repartidor->pos_y = (uint32_t) atoi( repartidor_posiciones[1] );
		l_repartidor->freq_descanso   = (uint32_t) atoi( g_frecuencia_de_descanso [posicion] );
		l_repartidor->tiempo_descanso = (uint32_t) atoi( g_tiempo_de_descanso     [posicion] );

		free( repartidor_posiciones[0] );
		free( repartidor_posiciones[1] );
		free( repartidor_posiciones );

		sem_init( &l_repartidor->semaforo, 0, 0 );

		mostrar_info_pcb_repartidor( l_repartidor );

		queue_push( g_cola_nuevos, l_repartidor );

		posicion++;

	}

	if ( ! ( g_repartidores           [posicion] != NULL
	      && g_frecuencia_de_descanso [posicion] != NULL
		  && g_tiempo_de_descanso     [posicion] != NULL )
	  &&   ( g_repartidores           [posicion] != NULL
	      || g_frecuencia_de_descanso [posicion] != NULL
	      || g_tiempo_de_descanso     [posicion] != NULL ) ) {

		printf("Información de repartidores incompleta. Corrija la configuración del módulo.\n");
		exit(-1);

	}

	while (1) {

		sem_wait( &g_nro_pedidos_confirmados );

		t_pcb_repartidor * l_repartidor = queue_pop( g_cola_nuevos );

		// asociar_pedido al repartidor

		queue_push( g_cola_listos, l_repartidor );

	}

}

bool procedimiento_02_seleccionar_restaurante( t_header * header_recibido ) {

	char * l_restaurante_seleccionado = NULL;

	bool _detecta_restaurante_en_lista(void * p_elem) { // detecta si el restaurante está disponible en la APP

		return string_equals_ignore_case( ((t_info_restarante*)p_elem)->resto_nombre , l_restaurante_seleccionado );

	}

	bool _detecta_asociacion_previa_de_id ( void * p_elem ) {

		return ((t_cliente_resto*)p_elem)->id_proceso == header_recibido->id_proceso ;

	}

	list_remove_by_condition( lista_asociaciones_cliente_resto, _detecta_asociacion_previa_de_id ); // Elimina asociación previa

	if ( header_recibido->size != 0 && header_recibido->payload != NULL ) { // Restaurante con nombre

		l_restaurante_seleccionado = malloc(header_recibido->size + 1);

		memcpy(l_restaurante_seleccionado, header_recibido->payload, header_recibido->size );

		l_restaurante_seleccionado[header_recibido->size] = '\0';

		bool esta_en_lista = list_any_satisfy( lista_resto_conectados, _detecta_restaurante_en_lista );

		if ( esta_en_lista ) {

			printf( "Cliente Nro.: '%d' asociado a restaurante: '%s'.\n", header_recibido->id_proceso, l_restaurante_seleccionado );

			t_cliente_resto * l_asociar = malloc( sizeof(t_cliente_resto) );

			t_info_restarante * resto = list_find ( lista_resto_conectados, _detecta_restaurante_en_lista );

			l_asociar->id_proceso = header_recibido->id_proceso;
			l_asociar->restaurante_asociado = resto;

			list_add( lista_asociaciones_cliente_resto, l_asociar );

			return true;

		} else {

			printf( "El restaurante recibido es: %s, y no se encuentra en la lista.\n", l_restaurante_seleccionado );

			return false;

		}

	} else { // Restaurante Default

		printf( "Cliente Nro.: '%d' asociado a restaurante predeterminado.\n", header_recibido->id_proceso );

		t_cliente_resto * l_asociar = malloc( sizeof(t_cliente_resto) );

		l_asociar->id_proceso = header_recibido->id_proceso;

		t_info_restarante * resto_default = list_find( lista_resto_conectados, _detecta_restaurante_en_lista );

		l_asociar->restaurante_asociado = resto_default;

		list_add( lista_asociaciones_cliente_resto, l_asociar );

		return true;

	}

}

char ** procedimiento_04_consultar_platos( t_header * header_recibido ) {

	bool _control_existe_asociacion_cliente_resto ( void * p_elem ) {

		return ( ((t_cliente_resto*)p_elem)->id_proceso == header_recibido->id_proceso ) ? true : false ;

	}

	t_cliente_resto * asociacion = list_find( lista_asociaciones_cliente_resto, _control_existe_asociacion_cliente_resto );

	if ( asociacion != NULL ) {

		printf("Se encontró asociación.");

		return asociacion->restaurante_asociado->list_platos;

	} else {

		printf("No se encontró la asociación.");

		return false;

	}

}

uint32_t procedimiento_05_crear_pedido( t_header * header_recibido ) {

	bool _chequeo_existencia_asociacion ( void * p_elem ) {

		return ((t_cliente_resto *) p_elem)->id_proceso == header_recibido->id_proceso;

	}

	t_cliente_resto * asociacion = (t_cliente_resto *) list_find( lista_asociaciones_cliente_resto, _chequeo_existencia_asociacion );

	if ( asociacion == NULL ) return -1;



	// "GUARDAR_PEDIDO" Hacia Comanda.

	uint32_t id_gen = random_id_generator();

	return id_gen;

}

void sigint(int a) {

	void _cerrar_sockets( void * p_elem ) {

		int elem = * ((int*)p_elem);

		close( elem );

	}

	list_iterate( g_sockets_abiertos, _cerrar_sockets );

	log_destroy(logger);
	config_destroy(config);
	close( g_socket_cliente );

	exit(1);

}




