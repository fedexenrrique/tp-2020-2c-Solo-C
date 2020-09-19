
#include "app.h"

int main(void) {

	prueba_biblioteca_compartida();

	lista_pcbs = list_create();

	g_sockets_abiertos = list_create();

	signal(SIGINT, sigint);

	logger = log_create("app.log","APP",1,LOG_LEVEL_INFO);
	config = leer_config();

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

	return config;

}

void procesamiento_mensaje( void * p_socket_aceptado ) {

	uint32_t socket_aceptado = (uint32_t)(* ( (int*) p_socket_aceptado ));

	t_header * header_recibido = recibir_buffer( socket_aceptado );

	printf( "Módulo:       %d.\n" , header_recibido->modulo     );
	printf( "ID Proceso:   %d.\n" , header_recibido->id_proceso );
	printf( "Nro. mensaje: %s.\n" , nro_comando_a_texto( header_recibido->nro_msg )   );
	printf( "Bytes:        %d.\n" , header_recibido->size       );

	mem_hexdump(header_recibido->payload, header_recibido->size);

	switch ( header_recibido->nro_msg ) {
	case CONSULTAR_RESTAURANTES:
		recibir_consultar_restaurante_y_responder( socket_aceptado );
		break;
	case SELECCIONAR_RESTAURANTE: ;
		bool seleccionado = procedimiento_02_seleccionar_restaurante( header_recibido );
		responder_seleccionar_restaurante( socket_aceptado, seleccionado );
		break;
	case CONSULTAR_PLATOS:
		responder_consultar_platos( socket_aceptado, g_platos_default );
		break;
	case CREAR_PEDIDO:
		// uint32_t socket_hacia_restaurante = crear_socket_y_conectar(char* p_ip, char* p_puerto);
		// enviar_buffer ( ---------, header_recibido );
		recibir_crear_pedido_y_responder( socket_aceptado, 987 );
		break;
	case ANIADIR_PLATO:
		break;
	case CONFIRMAR_PEDIDO:
		break;
	case PLATO_LISTO:
		break;
	case CONSULTAR_PEDIDO:
		break;
	default:
		printf("Mensaje no compatible con módulo APP.\n");
	}

	if ( header_recibido->size > 0 || header_recibido->payload != NULL )

		free( header_recibido->payload );

	close( socket_aceptado );

}

bool procedimiento_02_seleccionar_restaurante( t_header * header_recibido ) {

	char * l_restaurante_seleccionado;

	t_list * _obtener_restaurante_hardcodeado() {

		t_list * l_restaurantes = list_create();

		list_add( l_restaurantes, "McDonals" );
		list_add( l_restaurantes, "KFC" );
		list_add( l_restaurantes, "Wendy's" );
		list_add( l_restaurantes, "GreenEat" );

		return l_restaurantes;

	}

	bool _detecta_restaurante_en_lista(void * p_elem) {

		return string_equals_ignore_case( (char*)p_elem, l_restaurante_seleccionado );

	}

	l_restaurante_seleccionado = malloc(header_recibido->size + 1);

	memcpy(l_restaurante_seleccionado, header_recibido->payload, header_recibido->size );

	l_restaurante_seleccionado[header_recibido->size] = '\0';

	t_list * lista_de_restaurante = _obtener_restaurante_hardcodeado();

	bool esta_en_lista = list_any_satisfy(lista_de_restaurante, _detecta_restaurante_en_lista );

	if ( esta_en_lista ) {

		printf( "El restaurante recibido es: %s, y está en la lista.\n", l_restaurante_seleccionado );

		t_pcb * l_pcb = malloc( sizeof(t_pcb) );

		l_pcb->id_proceso = header_recibido->id_proceso;
		l_pcb->restaurante_asociado = l_restaurante_seleccionado;

		list_add( lista_pcbs, l_pcb );

		return true;

	} else {

		printf( "El restaurante recibido es: %s, y no se encuentra en la lista.\n", l_restaurante_seleccionado );

		return false;

	}

}

void sigint(int a) {

	/*
	void _cerrar_socket(void * p_elem) {

		int * socket = (int*)p_elem;
		close( * socket );

	}

	printf ("\nCerrando File Descriptor abiertos.\n");

	list_iterate( g_sockets_abiertos, _cerrar_socket );
*/
	log_destroy(logger);
	config_destroy(config);
	close( g_socket_cliente );

	exit(1);

}




