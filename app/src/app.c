
#include "app.h"

int main(void) {

	g_generador_id_repartidor = 90222000;

	lista_clientes = list_create();

	lista_resto_conectados  = list_create();

	queue_confirmados_cliente_resto = queue_create();

	sem_init( &sem_nuevos  ,0 ,0);

	sem_init( &sem_listos  ,0 ,0);

	sem_init( &sem_bloq    ,0 ,0);



	signal(SIGINT, sigint);

	logger = log_create("app.log","APP",1,LOG_LEVEL_INFO);
	config = leer_config();  // Carga tambien el resto default a la cola de lista_resto_conectados

	g_cola_nuevos = queue_create();

	g_cola_bloqueados = queue_create();

	if ( string_equals_ignore_case( g_algoritmo_de_planificacion , "SJF") )

		g_lista_listos = list_create();

	if ( string_equals_ignore_case( g_algoritmo_de_planificacion , "FIFO") )

		g_cola_listos = queue_create();

	pthread_create( &g_thread_long_term_scheduler  , NULL, (void*) &long_term_scheduler   , (void*) NULL  );

	pthread_create( &g_thread_medium_term_scheduler, NULL, (void*) &medium_term_scheduler , (void*) NULL  );

	pthread_create( &g_thread_short_term_scheduler , NULL, (void*) &short_term_scheduler  , (void*) NULL  );

	g_socket_cliente = crear_socket_escucha("127.0.0.1", g_puerto_escucha );

	while (1) {

		uint32_t socket_aceptado = aceptar_conexion( g_socket_cliente );

		if ( socket_aceptado <= 0) {

			perror("Error al aceptar el socket.");
			exit(-1);

		}

		pthread_t  thread_app;

		pthread_create(&thread_app,NULL,(void*)&procesamiento_mensaje,(void*)(&socket_aceptado));

	}

}

t_config * leer_config(void) {

	t_config * config = config_create("app.config");

	if ( config_has_property( config, "IP_COMANDA"                  ) ) g_ip_comanda                  = config_get_string_value(config, "IP_COMANDA");
	if ( config_has_property( config, "PUERTO_COMANDA"              ) ) g_puerto_comanda              = config_get_string_value(config, "PUERTO_COMANDA");
	if ( config_has_property( config, "PUERTO_ESCUCHA"              ) ) g_puerto_escucha              = config_get_string_value(config, "PUERTO_ESCUCHA");
	if ( config_has_property( config, "RETARDO_CICLO_CPU"           ) ) g_retardo_ciclo_cpu           = config_get_int_value(config, "RETARDO_CICLO_CPU");
	if ( config_has_property( config, "GRADO_DE_MULTIPROCESAMIENTO" ) ) g_grado_de_multiprocesamiento = config_get_int_value(config, "GRADO_DE_MULTIPROCESAMIENTO");

	if ( config_has_property( config, "ALGORITMO_DE_PLANIFICACION"  ) ) g_algoritmo_de_planificacion  = config_get_string_value(config, "ALGORITMO_DE_PLANIFICACION");
	if ( config_has_property( config, "ALPHA"                       ) ) g_alpha                       = config_get_int_value(config, "ALPHA");
	if ( config_has_property( config, "ESTIMACION_INICIAL"          ) ) g_estimacion_inicial          = config_get_int_value(config, "ESTIMACION_INICIAL");
	if ( config_has_property( config, "REPARTIDORES"                ) ) g_repartidores                = config_get_array_value(config, "REPARTIDORES");
	if ( config_has_property( config, "FRECUENCIA_DE_DESCANSO"      ) ) g_frecuencia_de_descanso      = config_get_array_value(config, "FRECUENCIA_DE_DESCANSO");

	if ( config_has_property( config, "TIEMPO_DE_DESCANSO"          ) ) g_tiempo_de_descanso          = config_get_array_value(config, "TIEMPO_DE_DESCANSO");
	if ( config_has_property( config, "ARCHIVO_LOG"                 ) ) g_log_path                    = config_get_string_value(config, "ARCHIVO_LOG");
	if ( config_has_property( config, "PLATOS_DEFAULT"              ) ) g_platos_default              = config_get_array_value(config, "PLATOS_DEFAULT");
	if ( config_has_property( config, "POSICION_REST_DEFAULT_X"     ) ) g_posicion_rest_default_x     = config_get_int_value(config, "POSICION_REST_DEFAULT_X");
	if ( config_has_property( config, "POSICION_REST_DEFAULT_Y"     ) ) g_posicion_rest_default_y     = config_get_int_value(config, "POSICION_REST_DEFAULT_Y");

	t_info_restaurante * resto_default = malloc( sizeof(t_info_restaurante) );

	resto_default->resto_x = g_posicion_rest_default_x;
	resto_default->resto_y = g_posicion_rest_default_y;
	resto_default->resto_nombre     = "default";
	resto_default->socket_conectado = 0;
	resto_default->list_platos = g_platos_default;

	list_add( lista_resto_conectados, resto_default );

	return config;

}

void mostrar_info_pcb_repartidor ( t_pcb_repartidor * p_pcb ) {

	printf("\nInformación del repartidor '%d'.\n", p_pcb->id_repartidor );
	printf("Se encuentra posicionado en ( %d, %d ).\n", p_pcb->repa_x, p_pcb->repa_y );
	printf("Descansa cada %d metros un tiempo de %d minutos.\n", p_pcb->freq_de_descanso, p_pcb->tiempo_de_descanso );

}

void procesamiento_mensaje( void * p_socket_aceptado ) {

	void * _aux_nombres_restos ( void * p_elem ) {

		return ((t_info_restaurante *) p_elem)->resto_nombre;

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
		responder_02_seleccionar_restaurante( socket_aceptado, seleccionado );
		break;
	case CONSULTAR_PLATOS: ;
		char ** platos = procedimiento_04_consultar_platos( header_recibido );
		responder_04_consultar_platos( socket_aceptado, platos );
		break;
	case CREAR_PEDIDO: ;
		// "CREAR_PEDIDO" Hacia Restaurante ( y "GUARDAR_PEDIDO" Hacia Sindicato).
		// "GUARDAR_PEDIDO" Hacia comanda.
		uint32_t id_ped_creado = procedimiento_05_crear_pedido( header_recibido );
		printf( "Se obtuvo el ID '%d'.\n", id_ped_creado );
		responder_05_crear_pedido( socket_aceptado, id_ped_creado );
		break;
	case ANIADIR_PLATO: ;
		// "ANIADIR_PLATO" Hacia Restaurante.
		// "GUARDAR_PLATO" Hacia comanda.
		bool se_aniadio = procesamiento_07_aniadir_plato( header_recibido );
		responder_07_aniadir_plato( socket_aceptado, se_aniadio );
		break;
	case CONFIRMAR_PEDIDO: ;
		bool confirmacion = procesamiento_09_confirmar_pedido ( header_recibido );
		responder_09_confirmar_pedido ( socket_aceptado, confirmacion );
		break;

	case CONFIRMAR_PEDIDO_HACK: ;
		printf("HACK DE CONFIRMAR PEDIDO");
		procesamiento_09_confirmar_pedido_hack ( );
		responder_09_confirmar_pedido ( socket_aceptado, TRUE );
		break;

	case PLATO_LISTO:
		//Me envia el restaurante el plato listo y yo informo a la comanda
		break;
	case CONSULTAR_PEDIDO:
		break;
	case CONECTAR:

		printf("tratamiento de conexión.....\n");

		// manejar_restaurante_conectado(header_recibido, socket_aceptado);

		t_header aux_head;

		aux_head.id_proceso = 99999;
		aux_head.modulo     = APP;
		aux_head.nro_msg    = CONECTAR;
		aux_head.size       = 0;
		aux_head.payload    = NULL;

		enviar_buffer( socket_aceptado, &aux_head );

		//deserializo la informacion del resto y lo agrego a la lista

		t_info_restaurante * info_resto=malloc(sizeof(t_info_restaurante));

		info_resto=deserializar_info_resto(header_recibido->payload,header_recibido->size);

		list_add(lista_resto_conectados,info_resto);//Agrego el nuevo resto a la lista

		//Esto estaba de antes, tengo q ver que hago
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
/*
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
*/

void bucle_resto_conectado ( uint32_t sock_aceptado ) {


	//Habria que agregar a la lista de los restaurantes

}

void long_term_scheduler( void ) {//Agrego los pcb de los repartidores a la cola de listos

	_aux_long_term_scheduler(); // Cargo los reaprtidores que tengo por archivo e inicio dos hilos, de descanso y ejecucuion

	while (1) {

		if ( queue_size(queue_confirmados_cliente_resto) != 0 && queue_size(g_cola_nuevos) != 0 ){//Me fijo si hay pedido confirmado y repartidor libre

			t_pcb_repartidor * l_repartidor = queue_pop(g_cola_nuevos);

			t_cliente_a_resto * l_confirmado = queue_pop(queue_confirmados_cliente_resto); //Saco de la cola de confirmados un pedido. No lo usaria mas calculo

			printf("Estoy por planificar el repartidor %d hasta con el pedido %d.\n", l_repartidor->id_repartidor, l_confirmado->id_pedido);

			l_repartidor->cliente_x = l_confirmado->cliente_x;

			l_repartidor->cliente_y = l_confirmado->cliente_y;

			l_repartidor->id_cliente= l_confirmado->id_cliente;

			l_repartidor->id_pedido = l_confirmado->id_pedido;

			l_repartidor->nombre_resto = l_confirmado->nombre_resto;

			l_repartidor->resto_x = l_confirmado->resto_x;

			l_repartidor->resto_y = l_confirmado->resto_y;

			l_repartidor->yendo_a = RESTO;

			//Agrego el pcb repartidos a la lista o cola de listos

			if ( string_equals_ignore_case( g_algoritmo_de_planificacion , "SJF") )

				list_add( g_lista_listos, l_repartidor );

			if ( string_equals_ignore_case( g_algoritmo_de_planificacion , "FIFO") )

				queue_push( g_cola_listos, l_repartidor );

			sem_post(&sem_listos);

		} else sleep(1);

	}

}

void _aux_long_term_scheduler() {

	uint32_t posicion = 0;

	while ( g_repartidores           [posicion] != NULL
		 && g_frecuencia_de_descanso [posicion] != NULL
		 && g_tiempo_de_descanso     [posicion] != NULL ) {

		t_pcb_repartidor * l_repartidor = malloc( sizeof(t_pcb_repartidor) );

		char ** repartidor_posiciones = (char **) string_split ( g_repartidores[posicion] , "|" );

		l_repartidor->id_repartidor = g_generador_id_repartidor++;
		l_repartidor->repa_x = (uint32_t) atoi( repartidor_posiciones[0] );
		l_repartidor->repa_y = (uint32_t) atoi( repartidor_posiciones[1] );
		l_repartidor->freq_de_descanso   = (uint32_t) atoi( g_frecuencia_de_descanso [posicion] );
		l_repartidor->tiempo_de_descanso = (uint32_t) atoi( g_tiempo_de_descanso     [posicion] );

		free( repartidor_posiciones[0] );
		free( repartidor_posiciones[1] );
		free( repartidor_posiciones );

		sem_init( &l_repartidor->semaforo, 0, 0 );

		sem_init( &l_repartidor->cpu, 0, 0 );

		sem_init( &l_repartidor->sem_bloq, 0, 0 );

		sem_init( &l_repartidor->bloq, 0, 0 );

		mostrar_info_pcb_repartidor( l_repartidor );

		queue_push( g_cola_nuevos, l_repartidor );

		pthread_create(&l_repartidor->thread_metadata  , NULL, (void*) &ejecucion_repartidor, (void*) l_repartidor  );

		pthread_t thread_data;

		pthread_create( &thread_data , NULL, (void*) &descanso_repartidor, (void*) l_repartidor  );

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
}

void short_term_scheduler( void ) {

			t_pcb_repartidor * _sjf_repartidor( void ) {

				uint32_t distancia_aux = 9999;

				t_pcb_repartidor * pcb_aux = NULL;

				bool _aux_sjf_remove_condition( void * p_elem ) {

					return p_elem == ((void*) pcb_aux);

				}

				void _aux_sjf ( void * p_elem ) {

					t_pcb_repartidor * elem = (t_pcb_repartidor *) p_elem;

					uint32_t distancia = abs(elem->cliente_x - elem->resto_x) + abs(elem->cliente_y - elem->resto_y)
							+ ( elem->yendo_a == RESTO ) ?
									abs(elem->resto_x   - elem->repa_x ) + abs(elem->resto_y   - elem->repa_y) : 0;

					if ( distancia < distancia_aux ) {

						distancia_aux = distancia;

						pcb_aux = elem;

					}

				}

				list_iterate( g_lista_listos, _aux_sjf );

				list_remove_by_condition( g_lista_listos, _aux_sjf_remove_condition );

				return pcb_aux;

			 }

	uint32_t size_cola_listos = 0;

	if ( string_equals_ignore_case( g_algoritmo_de_planificacion , "SJF") ) {

		while(1){

			sem_wait(&sem_listos);

			size_cola_listos = list_size( g_lista_listos );

			printf("El tamaño de la lista de listos es ahora de %d.\n", size_cola_listos );

			if (size_cola_listos == 0) size_cola_listos = -1;

			t_pcb_repartidor * repa = _sjf_repartidor();

			repa->estado = EJEC;

			sem_post(&repa->semaforo);

			sem_wait(&repa->cpu);

		}

	}

	if ( string_equals_ignore_case( g_algoritmo_de_planificacion , "FIFO") ) {

		while(1){

			sem_wait(&sem_listos);

			size_cola_listos = queue_size( g_cola_listos );

			printf("El tamaño de la cola de listos es ahora de %d.\n", size_cola_listos );

			if (size_cola_listos == 0) size_cola_listos = -1;

			t_pcb_repartidor * repa = (t_pcb_repartidor *) queue_pop( g_cola_listos );

			repa->estado = EJEC;

			sem_post(&repa->semaforo);

			sem_wait(&repa->cpu);

		}

	}

}

void medium_term_scheduler( void ) {//SAco un repartido bloqueado y haria un wait para q se bloquee

	while(1){

		sem_wait(&sem_bloq);  //Esto podria ser un semaforo general para poder sacar un elemento de la cola de bloqueados

		t_pcb_repartidor * repa = (t_pcb_repartidor *) queue_pop( g_cola_bloqueados );

		sem_post(&repa->sem_bloq);

		sem_wait(&repa->bloq);// Seria un semaforo para que se bloquee el repartidor

	}

}

//Hace su descanso y luego se vuelve a agregar a la cola de listos. El semaforo calculo q se activa cuando ya cumplio la cantidad de ciclos solicitados
void descanso_repartidor ( t_pcb_repartidor * p_pcb ) { while (1) {

	sem_wait( &p_pcb->sem_bloq );

	uint32_t tiempo_de_descanso = p_pcb->tiempo_de_descanso;

	printf( "Tengo que descansar %d minutos.\n", tiempo_de_descanso );

	while ( tiempo_de_descanso != 0) {

		tiempo_de_descanso--;

		sleep( g_retardo_ciclo_cpu );

	    }

	p_pcb->cansancio = 0;

	p_pcb->estado = LISTO;

	if ( string_equals_ignore_case( g_algoritmo_de_planificacion , "SJF") )

		list_add( g_lista_listos, p_pcb );

	if ( string_equals_ignore_case( g_algoritmo_de_planificacion , "FIFO") )

		queue_push( g_cola_listos, p_pcb );

	sem_post(&sem_listos);

	mostrar_info_pcb_repartidor( p_pcb );

	sem_post( &p_pcb->bloq );

  }
}

void ejecucion_repartidor ( t_pcb_repartidor * p_pcb ) {

			void _moverse_hacia_restaurante () {

				if ( p_pcb->repa_x != p_pcb->resto_x || p_pcb->repa_y != p_pcb->resto_y ) {

					if ( p_pcb->repa_x != p_pcb->resto_x ) {

						if ( p_pcb->repa_x < p_pcb->resto_x )

							p_pcb->repa_x++;

						else

							p_pcb->repa_x--;

					} else if ( p_pcb->repa_y != p_pcb->resto_y ) {

						if ( p_pcb->repa_y < p_pcb->resto_y )

							p_pcb->repa_y++;

						else

							p_pcb->repa_y--;

					}

					p_pcb->cansancio++;

					log_info( logger, "El repartidor '%d'. va al restaurante (%d,%d) y avanza a (%d,%d)", p_pcb->id_repartidor , p_pcb->resto_x, p_pcb->resto_y, p_pcb->repa_x , p_pcb->repa_y );

				}

			}

				void _moverse_hacia_cliente () {

					if ( p_pcb->repa_x != p_pcb->cliente_x || p_pcb->repa_y != p_pcb->cliente_y ) { // ir al cliente

						if ( p_pcb->repa_x != p_pcb->cliente_x ) {

							if ( p_pcb->repa_x < p_pcb->cliente_x )

								p_pcb->repa_x++;
							else

								p_pcb->repa_x--;

						} else if ( p_pcb->repa_y != p_pcb->cliente_y ) {

							if ( p_pcb->repa_y < p_pcb->cliente_y )

								p_pcb->repa_y++;

							else

								p_pcb->repa_y--;

						}

						p_pcb->cansancio++;

						log_info( logger, "El repartidor '%d'. va el cliente en (%d,%d) y avanza a (%d,%d)", p_pcb->id_repartidor , p_pcb->cliente_x, p_pcb->cliente_y, p_pcb->repa_x , p_pcb->repa_y );

					}

				}

			void _repartidor_en_bicicleta(void) {

				if ( p_pcb->estado != EJEC ) {

					p_pcb->estado = EJEC;

					mostrar_info_pcb_repartidor( p_pcb );

				}

				sleep(g_retardo_ciclo_cpu);

				if ( p_pcb->cansancio == p_pcb->freq_de_descanso ) {

					p_pcb->estado = BLOQ;

				}


				if ( p_pcb->repa_x == p_pcb->cliente_x && p_pcb->repa_y == p_pcb->cliente_y && p_pcb->yendo_a == CLI ) { // ESTOY en CLIENTE

					bool finalizo = enviar_13_finalizar_pedido   ( g_ip_comanda, g_puerto_comanda, p_pcb->nombre_resto, p_pcb->id_pedido );

					if ( finalizo )

						printf("Finalizó pedido correctamente.");

					else

						printf("Hubo un problema al finalizar el pedido.");

					p_pcb->estado = FINAL ;

				} else if ( p_pcb->repa_x == p_pcb->resto_x && p_pcb->repa_y == p_pcb->resto_y && p_pcb->yendo_a == RESTO ) { // ESTOY en RESTAURANTE

					enviar_obtener_pedido( g_ip_comanda, g_puerto_comanda, p_pcb->nombre_resto, p_pcb->id_pedido);
					//enviar_12_obtener_pedido( g_ip_comanda, g_puerto_comanda, p_pcb->nombre_resto, p_pcb->id_pedido );

					if ( p_pcb->yendo_a == RESTO ){

						p_pcb->yendo_a = CLI;

						_moverse_hacia_cliente();

					}

				} else { // EN CAMINO

					if ( p_pcb->yendo_a == RESTO ) {

						_moverse_hacia_restaurante();

					} else if ( p_pcb->yendo_a == CLI ) {

						_moverse_hacia_cliente();

					}

				}

			}

	printf( "Se inició correctamente el Hilo del Repartidor '%d'.\n", p_pcb->id_repartidor );

	if ( string_equals_ignore_case(g_algoritmo_de_planificacion, "SJF") ) while (1) {//Inicio repartidor con SJF

		sem_wait(&p_pcb->semaforo);

		_repartidor_en_bicicleta();

		if ( p_pcb->estado == EJEC ) {

			printf("Consumí una ráfaga de CPU.\n");

			list_add( g_lista_listos, p_pcb );

			sem_post(&sem_listos);

		}

		if ( p_pcb->estado == BLOQ ) {

			printf("Salí de CPU por cansancio.\n");

			queue_push( g_cola_bloqueados, p_pcb );

			sem_post(&sem_bloq);

		}

		if ( p_pcb->estado == FINAL ) {

			printf("Salí de CPU por finalización.\n");

			p_pcb->estado = NUEVO;

			queue_push( g_cola_nuevos, p_pcb );

		}

		sem_post(&p_pcb->cpu);

		mostrar_info_pcb_repartidor( p_pcb );

	}//FIN repartidor con SJF

	if ( string_equals_ignore_case(g_algoritmo_de_planificacion, "FIFO") ) while (1) {//Inicio repartidor con FIFO

		sem_wait(&p_pcb->semaforo);

		do { _repartidor_en_bicicleta(); } while ( p_pcb->estado == EJEC );

		if ( p_pcb->estado == BLOQ ) {

			printf("Salí de CPU por cansancio.\n");

			queue_push( g_cola_bloqueados, p_pcb );

			sem_post(&sem_bloq);

			sem_post(&p_pcb->cpu);

			mostrar_info_pcb_repartidor( p_pcb );

		} else {

			printf("Salí de CPU por finalización.\n");

			p_pcb->estado = NUEVO;

			queue_push( g_cola_nuevos, p_pcb );

			sem_post(&p_pcb->cpu);

			mostrar_info_pcb_repartidor( p_pcb );

		}

	}//FIN repartidor con FIFO

}

bool procedimiento_02_seleccionar_restaurante ( t_header * header_recibido ) {//Me envian el nombre de resto y la posicion del cliente

	char * l_restaurante_seleccionado = NULL;

			bool _detecta_restaurante_en_lista(void * p_elem) { // detecta si el restaurante está disponible en la APP

				return string_equals_ignore_case( ((t_info_restaurante*)p_elem)->resto_nombre , l_restaurante_seleccionado );

			}

			bool _detecta_asociacion_previa_de_id ( void * p_elem ) {

				return ((t_cliente_a_resto*)p_elem)->id_cliente == header_recibido->id_proceso ;

			}



	list_remove_by_condition( lista_clientes, _detecta_asociacion_previa_de_id ); // Elimina asociación previa


	if ( header_recibido->size != 0 && header_recibido->payload != NULL ) { // Restaurante con nombre

		uint32_t pos_x, pos_y, size_resto, despla = 0;

		memcpy( &pos_x, header_recibido->payload + despla, sizeof(uint32_t) );

		despla += sizeof(uint32_t);

		memcpy( &pos_y, header_recibido->payload + despla, sizeof(uint32_t) );

		despla += sizeof(uint32_t);

		memcpy( &size_resto, header_recibido->payload + despla, sizeof(uint32_t) );

		despla += sizeof(uint32_t);

		l_restaurante_seleccionado = malloc( size_resto +1 );

		memcpy(l_restaurante_seleccionado, header_recibido->payload + despla , size_resto );

		l_restaurante_seleccionado[size_resto ] = '\0';

		printf( "'%s'\n", l_restaurante_seleccionado);

		bool esta_en_lista = list_any_satisfy( lista_resto_conectados, _detecta_restaurante_en_lista );

		if ( esta_en_lista ) {

			printf( "Cliente Nro.: '%d' en la posición (%d,%d) se asociado al restaurante: '%s'.\n"
					, header_recibido->id_proceso, pos_x, pos_y, l_restaurante_seleccionado );

			t_cliente_a_resto * l_confirmar = malloc( sizeof(t_cliente_a_resto) );

			t_info_restaurante * resto = list_find ( lista_resto_conectados, _detecta_restaurante_en_lista );

			if ( resto == NULL ) return false;

			l_confirmar->id_cliente = header_recibido->id_proceso;
			l_confirmar->cliente_x  = pos_x;
			l_confirmar->cliente_y  = pos_y;
			l_confirmar->nombre_resto = resto->resto_nombre;
			l_confirmar->id_pedido  = 0;
			l_confirmar->resto_x    = resto->resto_x;
			l_confirmar->resto_y    = resto->resto_y;

			list_add( lista_clientes, l_confirmar );

			return true;

		} else {

			printf( "El restaurante recibido es: %s, y no se encuentra en la lista.\n", l_restaurante_seleccionado );

			return false;

		}

	} else { // falta payload

		printf( "Cliente Nro.: '%d' no se puede asociar a restaurante. Seleccionar un nombre.\n", header_recibido->id_proceso );

		return false;

	}

}

char ** procedimiento_04_consultar_platos( t_header * header_recibido ) {

	char ** vector_platos=NULL;

				bool _control_existe_asociacion_cliente_resto ( void * p_elem ) {

					return ( ((t_cliente_a_resto*)p_elem)->id_cliente == header_recibido->id_proceso ) ? true : false ;

				}

	t_cliente_a_resto * asociacion = list_find( lista_clientes , _control_existe_asociacion_cliente_resto );

	if ( asociacion != NULL ) {

		printf("Se encontró asociación.");

		if(string_equals_ignore_case(asociacion->nombre_resto,"default"))
			return g_platos_default;

		t_info_restaurante * info_resto=buscar_info_de_restaurante(asociacion->nombre_resto,lista_resto_conectados);
		vector_platos=enviar_04_consultar_platos_app_a_resto(info_resto->socket_conectado);
		return vector_platos; // DEBE RETORNAR LA LISTA DE PLATOS DE LOS RESTAURANTS QUE NO SON DEFAULT ¿--- Tiene q mandar msj a restaurant o comanda

	} else {

		printf("No se encontró la asociación.");

		return NULL;

	}

}

uint32_t procedimiento_05_crear_pedido( t_header * header_recibido ) {

					bool _chequeo_existencia_asociacion ( void * p_elem ) {

						return ((t_cliente_a_resto *) p_elem)->id_cliente == header_recibido->id_proceso;

					}

	t_cliente_a_resto * asociacion = (t_cliente_a_resto *) list_find( lista_clientes, _chequeo_existencia_asociacion );

	if ( asociacion == NULL ) return -1;

	if ( asociacion->id_pedido != 0 ) {//Aca creo q elimino un pedido pre existente, y le doy por default el valor 0

		log_info(logger, "Se elimina pedido '%d' pre-existente.\n", asociacion->id_pedido );

		asociacion->id_pedido = 0;

	    }

	if(string_equals_ignore_case(asociacion->nombre_resto,"default")){//Si es reto default, genero un id, sino lo pido al restorant

		uint32_t id_pedido_generado = random_id_generator();
		asociacion->id_pedido = id_pedido_generado;
	}else{

		t_info_restaurante * info_resto=buscar_info_de_restaurante(asociacion->nombre_resto,lista_resto_conectados);

		asociacion->id_pedido = solicitar_id_a_restaurante(info_resto->socket_conectado);
	    }

	// "GUARDAR_PEDIDO" Hacia Comanda.


	bool guardado = enviar_06_guardar_pedido(g_ip_comanda, g_puerto_comanda, asociacion->nombre_resto, asociacion->id_pedido );

	if (guardado) {

		printf("Se pudo guardar el pedido.\n");

	} else printf("No se pudo guardar el pedido.\n");

	return asociacion->id_pedido;

}

bool procesamiento_07_aniadir_plato( t_header * header_recibido ) {

				bool _control_existe_asociacion_cliente_resto ( void * p_elem ) {

					return ((t_cliente_a_resto*)p_elem)->id_cliente == header_recibido->id_proceso ;

				}

	t_cliente_a_resto * asociacion = list_find( lista_clientes, _control_existe_asociacion_cliente_resto );

	if ( asociacion == NULL || asociacion->id_pedido == 0 ) {

		printf("Se requiere asociar un restaurante y crear un pedido previamente a solicitar un plato.");

		return false;

	}

	uint32_t despla = 0;
	uint32_t cantidad_platos=1;
	uint32_t id_pedido = 0;

	memcpy( &id_pedido, header_recibido->payload + despla, sizeof(uint32_t) );

	despla += sizeof(uint32_t);

	uint32_t size_nombre_plato = 0;

	memcpy( &size_nombre_plato, header_recibido->payload + despla, sizeof(uint32_t) );

	despla += sizeof(uint32_t);

	char * nombre_plato = malloc(size_nombre_plato+1);

	memcpy( nombre_plato, header_recibido->payload + despla, size_nombre_plato );

	despla += size_nombre_plato;

	nombre_plato[size_nombre_plato] = '\0';

	printf("\n%s\n", nombre_plato);

	if(asociacion->id_pedido!=id_pedido)log_error(logger,"Cliente envio mal el id del pedido. Se utiliza el existente %d:",asociacion->id_pedido);

	bool guardado = enviar_08_guardar_plato(  g_ip_comanda , g_puerto_comanda
										    , asociacion->nombre_resto
										    , asociacion->id_pedido
										    , nombre_plato
										    , cantidad_platos);

	return guardado;

}

void auxiliar_aniadir_plato ( t_list * p_list_platos, uint32_t p_cant_plato, char * p_nom_plato ) {

	bool _existe_plato ( void * p_elem ) {

		return string_equals_ignore_case( ((t_elem_pedido*)p_elem)->nombre_plato , p_nom_plato );

	}

	t_elem_pedido * elem_pedido = list_find( p_list_platos, _existe_plato );

	if ( elem_pedido != NULL ) {

		elem_pedido->cantidad_plato += p_cant_plato;

	} else {

		elem_pedido = malloc( sizeof(t_elem_pedido) );

		elem_pedido->nombre_plato = p_nom_plato;

		elem_pedido->cantidad_plato = p_cant_plato;

		list_add( p_list_platos, elem_pedido );

	}

}

//pido a resto y a comanda la confirmacion del pedido. SI es correcta, agrego el pedido a la queue_confirmados_cliente_resto para q se empiece a planificar
bool procesamiento_09_confirmar_pedido ( t_header * header_recibido ) {

	uint32_t despla = 0;
	uint32_t id_pedido = 0;
	bool confirmacion=FALSE;
	char * nombre_resto;

				bool _control_existe_pedido ( void * p_elem ) {//Aca le agregue la comprobacion del nombre del resto tambien. "COmprueba sii existe pedido para tal resto"

					return ((t_cliente_a_resto*)p_elem)->id_pedido == id_pedido && string_equals_ignore_case(((t_cliente_a_resto*)p_elem)->nombre_resto ,nombre_resto) ;

					}


				bool buscar_resto(void * elemento){

					t_info_restaurante * resto=(t_info_restaurante*)elemento;

					if(string_equals_ignore_case(resto->resto_nombre,nombre_resto))
						return TRUE;
					return FALSE;
				}

	memcpy( &id_pedido, header_recibido->payload + despla, sizeof(uint32_t) );

	despla += sizeof(uint32_t);

	uint32_t size_nombre_resto = 0;

	memcpy( &size_nombre_resto, header_recibido->payload + despla, sizeof(uint32_t) );

	despla += sizeof(uint32_t);

	nombre_resto = malloc(size_nombre_resto + 1);

	memcpy( nombre_resto, header_recibido->payload + despla, size_nombre_resto );

	despla += size_nombre_resto;

	nombre_resto[size_nombre_resto] = '\0';

	printf("\n%s\n", nombre_resto);

	//aca tendria q mandar msj a restaurant, y confirmar pedido

	if(!string_equals_ignore_case(nombre_resto,"default")){
		t_info_restaurante * resto=list_find(lista_resto_conectados,buscar_resto);

		confirmacion=enviar_confirmar_pedido_a_resto(resto,id_pedido);

		if(confirmacion==FALSE){
			printf( "No se pudo confirmar el pedido.\n" );
			return confirmacion;
		}

	}

	confirmacion = enviar_09_confirmar_pedido ( g_ip_comanda, g_puerto_comanda, nombre_resto, id_pedido );

	if (confirmacion) {

		printf( "Se confirmó el pedido.\n" );

	   } else{ printf( "No se pudo confirmar el pedido.\n" );
	   	   	   	return confirmacion;   }

	//confirmacion = TRUE;
													//--------ESTO DEBERIA IR PRIMERO CREO

	t_cliente_a_resto * asociacion = list_find( lista_clientes, _control_existe_pedido );

	if ( asociacion == NULL ) {

		printf("Se requiere asociar un restaurante y crear un pedido previamente a solicitar un plato.");

		return FALSE;

	   } else {

		agregar_pedid_a_planificacion (asociacion);  //Agrego pedido a la cola de confirmados cliente_resto  queue_confirmados_cliente_resto

	   }

	return TRUE;

}

bool procesamiento_09_confirmar_pedido_hack ( void ) {

	t_cliente_a_resto * asociacion = malloc(sizeof(t_cliente_a_resto));

	asociacion->cliente_x=10;

	asociacion->cliente_y=10;

	asociacion->id_cliente= 999;

	asociacion->id_pedido= 999;

	asociacion->nombre_resto= "default";

	asociacion->resto_x=20;

	asociacion->resto_y=20;

	agregar_pedid_a_planificacion (asociacion);

	t_cliente_a_resto * asociacion2 = malloc(sizeof(t_cliente_a_resto));

	asociacion2->cliente_x=25;

	asociacion2->cliente_y=25;

	asociacion2->id_cliente= 999;

	asociacion2->id_pedido= 999;

	asociacion2->nombre_resto= "default";

	asociacion2->resto_x=30;

	asociacion2->resto_y=30;

	agregar_pedid_a_planificacion (asociacion2);

	t_cliente_a_resto * asociacion3 = malloc(sizeof(t_cliente_a_resto));

	asociacion3->cliente_x=25;

	asociacion3->cliente_y=25;

	asociacion3->id_cliente= 999;

	asociacion3->id_pedido= 999;

	asociacion3->nombre_resto= "default";

	asociacion3->resto_x=25;

	asociacion3->resto_y=25;

	agregar_pedid_a_planificacion (asociacion3);

	return true;

}

void agregar_pedid_a_planificacion (t_cliente_a_resto * asociacion) {

	queue_push( queue_confirmados_cliente_resto, asociacion );

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




