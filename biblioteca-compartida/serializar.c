#include "serializar.h"

void prueba_biblioteca_compartida(void) {
	printf("\n!!!Texto impreso desde la biblioteca compartida!!!\n");
}

t_list * enviar_01_consultar_restaurantes(char* p_ip, char* p_puerto) {

	t_list * _recibir_lista_restaurantes( int p_conexion ) {

		t_header * header_restaurantes = recibir_buffer( p_conexion );

		t_list   * lista_restaurantes;

		if ( header_restaurantes->size > 0 ) {

			lista_restaurantes = list_create();

			uint32_t despla = 0;

			uint32_t cantidad_restaurantes = 0;

			memcpy( &cantidad_restaurantes, header_restaurantes->payload + despla, sizeof(uint32_t) );

			despla += sizeof(uint32_t);

			for ( int i = 0; i < cantidad_restaurantes; i++ ) {

				uint32_t act_size;

				memcpy( &act_size    , header_restaurantes->payload + despla, sizeof(uint32_t) );
				despla += sizeof(uint32_t);

				char * nombre_resto;

				if ( act_size > 0 ) {

					nombre_resto = malloc(act_size + 1);

					memcpy( nombre_resto , header_restaurantes->payload + despla, act_size );
					despla += act_size;

					nombre_resto[act_size] = '\0';

				} else {

					nombre_resto = "Default";

				}

				printf("\n%s\n", nombre_resto);

				list_add(lista_restaurantes, nombre_resto);

			}

			return lista_restaurantes;

		} else return NULL;

	}

	t_header l_header;

	l_header.modulo     = CLIENTE;
	l_header.id_proceso = 0;
	l_header.nro_msg    = CONSULTAR_RESTAURANTES;
	l_header.size       = 0;
	l_header.payload    = NULL;

	printf( "Se va a enviar la consulta a la ip %s y puerto %s.", p_ip, p_puerto );

	uint32_t conexion = crear_socket_y_conectar(p_ip, p_puerto);

	if ( enviar_buffer( conexion, &l_header ) ) {

		t_list * restaurantes_recibidos = _recibir_lista_restaurantes(conexion);

		close( conexion );

		return restaurantes_recibidos;

	} else {

		close(conexion);

		return NULL;

	}

}

void responder_01_consultar_restaurantes ( uint32_t socket_cliente, t_list * p_list_restaurantes ) {

	void * buffer_response;
	int    buffer_size;
	int    despla = 0;

	int _aux_longitud_acumulada( t_list * p_list ) {

		int total = 0;

		void _aux_longitud_acumulada_iter( void * p_elem ) {

			total += (p_elem != NULL) ? string_length( (char *) p_elem ) : 0;

		}

		list_iterate( p_list, _aux_longitud_acumulada_iter );

		return total;

	}

	void _aux_mostrar_restaurantes( void * p_elem ) {

		printf( "Palabra: '%s'; Longitud: %d\n", (char*)p_elem, string_length( (char*)p_elem ) );

	}

	void _aux_cargar_buffer( void * p_elem ) {

		uint32_t length = (p_elem != NULL) ? string_length((char *) p_elem) : 0;

		memcpy( buffer_response + despla, &length, sizeof(uint32_t) );

		despla += sizeof(uint32_t);

		if ( p_elem != NULL ) {

			memcpy( buffer_response + despla, p_elem, string_length((char *) p_elem) );

			despla += string_length((char *) p_elem);

		}

	}

	uint32_t cant_restos = list_size( p_list_restaurantes );

	uint32_t cant_letras = _aux_longitud_acumulada( p_list_restaurantes );

	buffer_size = sizeof(uint32_t)
			+ cant_restos * sizeof(uint32_t)
			+ cant_letras;

	buffer_response = malloc(buffer_size);

	memcpy( buffer_response + despla, &cant_restos, sizeof(uint32_t) );

	despla += sizeof(uint32_t);

	list_iterate( p_list_restaurantes, _aux_cargar_buffer );

	list_iterate( p_list_restaurantes, _aux_mostrar_restaurantes );

	t_header header_response;

	header_response.modulo     = APP;
	header_response.id_proceso = 0;
	header_response.nro_msg    = CONSULTAR_RESTAURANTES;
	header_response.size       = buffer_size;
	header_response.payload    = buffer_response;

	enviar_buffer( socket_cliente, &header_response);

	free(buffer_response);

}

uint32_t crear_socket_y_conectar(char* p_ip, char* p_puerto) {

	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family   = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags    = AI_PASSIVE;

	getaddrinfo(p_ip, p_puerto, &hints, &server_info);

	int socket_cliente   = -1;
	int validar_conexion = -1;

	while ( validar_conexion == -1 || socket_cliente == -1 ) {

		socket_cliente   = socket (server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
		validar_conexion = connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen);

		if ( g_tiempo_reconexion == 0 ) break;

		if( validar_conexion == -1 || socket_cliente == -1 ){

			sleep(g_tiempo_reconexion);

		}

	}

	freeaddrinfo(server_info);

	if( validar_conexion == -1 || socket_cliente == -1) {

		log_info(logger,"[ BIBLIO - crear_socket_y_conectar] No se pudo establecer la conexión al módulo servidor.");

		return -1;

	} else

		log_info(logger,"Conexión exitosa!");

	return socket_cliente;

}

uint32_t recibir_confirmacion(uint32_t socket_cliente) {

	return 000;

}

uint32_t crear_socket_escucha( char * p_ip, char * p_puerto ) {

	int socket_servidor;

	struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags    = AI_PASSIVE;

    getaddrinfo( p_ip, p_puerto, &hints, &servinfo);

    for ( p=servinfo; p != NULL; p = p->ai_next) {

    	if ( -1 == (socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) ){

    		perror("ERROR EN LA CREACION DE SOCKET");
        	continue;

    	}

        if ( -1 == bind(socket_servidor, p->ai_addr, p->ai_addrlen) ) {

        	perror("ERROR DE PUERTO");
        	close(socket_servidor);
            continue;
        }

        break;

    }

	if( -1 == listen( socket_servidor, SOMAXCONN ) ) perror("ERROR PUERTO EN ESCUCHA");


    freeaddrinfo(servinfo);

    return socket_servidor;

}

uint32_t aceptar_conexion ( uint32_t p_socket_para_escuchar ) {

	socklen_t          dir_cliente_size;
	struct sockaddr_in dir_cliente;
	int                socket_cliente;

	dir_cliente_size = sizeof(struct sockaddr_in);

	socket_cliente = accept(p_socket_para_escuchar, (void*) &dir_cliente, &dir_cliente_size);

	if(socket_cliente == -1)
		perror("ERROR AL ACEPTAR CONEXION DE SOCKET");
	else {
		log_info(logger,"Se conecto Cliente");
		return socket_cliente;
	}

	return -1;

}

uint32_t detectar_comando(char * p_comando) {

	if ( string_equals_ignore_case(p_comando, "CONSULTAR_RESTAURANTES" ) ) return CONSULTAR_RESTAURANTES ; else
	if ( string_equals_ignore_case(p_comando, "SELECCIONAR_RESTAURANTE") ) return SELECCIONAR_RESTAURANTE; else
	if ( string_equals_ignore_case(p_comando, "OBTENER_RESTAURANTES"   ) ) return OBTENER_RESTAURANTES   ; else
	if ( string_equals_ignore_case(p_comando, "CONSULTAR_PLATOS"       ) ) return CONSULTAR_PLATOS       ; else
	if ( string_equals_ignore_case(p_comando, "CREAR_PEDIDO"           ) ) return CREAR_PEDIDO           ; else
	if ( string_equals_ignore_case(p_comando, "GUARDAR_PEDIDO"         ) ) return GUARDAR_PEDIDO         ; else
	if ( string_equals_ignore_case(p_comando, "ANIADIR_PLATO"          ) ) return ANIADIR_PLATO          ; else
	if ( string_equals_ignore_case(p_comando, "GUARDAR_PLATO"          ) ) return GUARDAR_PLATO          ; else
	if ( string_equals_ignore_case(p_comando, "CONFIRMAR_PEDIDO"       ) ) return CONFIRMAR_PEDIDO       ; else
	if ( string_equals_ignore_case(p_comando, "PLATO_LISTO"            ) ) return PLATO_LISTO            ; else
	if ( string_equals_ignore_case(p_comando, "CONSULTAR_PEDIDO"       ) ) return CONSULTAR_PEDIDO       ; else
	if ( string_equals_ignore_case(p_comando, "OBTENER_PEDIDO"         ) ) return OBTENER_PEDIDO         ; else
	if ( string_equals_ignore_case(p_comando, "FINALIZAR_PEDIDO"       ) ) return FINALIZAR_PEDIDO       ; else
	if ( string_equals_ignore_case(p_comando, "TOMAR_PEDIDO"           ) ) return TOMAR_PEDIDO           ; else

	return -1;

}

char * nro_comando_a_texto(cod_msg p_comando) {

	switch (p_comando) {
		case CONSULTAR_RESTAURANTES:  	return "CONSULTAR_RESTAURANTES" ;break;
		case SELECCIONAR_RESTAURANTE: 	return "SELECCIONAR_RESTAURANTE";break;
		case OBTENER_RESTAURANTES: 		return "OBTENER_RESTAURANTES"   ;break;
		case CONSULTAR_PLATOS: 			return "CONSULTAR_PLATOS"       ;break;
		case CREAR_PEDIDO: 			 	return "CREAR_PEDIDO"           ;break;
		case GUARDAR_PEDIDO: 			return "GUARDAR_PEDIDO"         ;break;
		case ANIADIR_PLATO: 			return "ANIADIR_PLATO"          ;break;
		case GUARDAR_PLATO: 			return "GUARDAR_PLATO"          ;break;
		case CONFIRMAR_PEDIDO: 			return "CONFIRMAR_PEDIDO"       ;break;
		case PLATO_LISTO: 				return "PLATO_LISTO"            ;break;
		case CONSULTAR_PEDIDO: 			return "CONSULTAR_PEDIDO"       ;break;
		case OBTENER_PEDIDO: 			return "OBTENER_PEDIDO"         ;break;
		case FINALIZAR_PEDIDO: 			return "FINALIZAR_PEDIDO"       ;break;
		case TOMAR_PEDIDO: 				return "TOMAR_PEDIDO"           ;break;
		case CONECTAR:                  return "CONECTAR"               ;break;
		default: 						return NULL ;break;
	}

}

uint32_t detectar_modulo( char * p_modulo ) {

	if ( string_equals_ignore_case(p_modulo, "APP"          ) ) return APP         ; else
	if ( string_equals_ignore_case(p_modulo, "COMANDA"      ) ) return COMANDA     ; else
	if ( string_equals_ignore_case(p_modulo, "RESTAURANTE"  ) ) return RESTAURANTE ; else
	if ( string_equals_ignore_case(p_modulo, "SINDICATO"    ) ) return SINDICATO   ; else

	return -1;

}

char * nro_modulo_a_texto( uint32_t p_modulo ) {

	switch ( p_modulo ) {
		case APP:         return "APP"          ;break;
		case COMANDA: 	  return "COMANDA"      ;break;
		case RESTAURANTE: return "RESTAURANTES" ;break;
		case SINDICATO:   return "SINDICATO"    ;break;
		default:          return NULL           ;break;
	}

}

char * nro_estado_pedido_a_texto( estado_pedido estado ) {

	switch ( estado ) {
		case PENDIENTE:       return "PENDIENTE" ;break;
		case CONFIRMADO: 	  return "CONFIRMADO";break;
		case TERMINADO:		  return "TERMINADO" ;break;
		default:          return NULL            ;break;
	}

}

bool enviar_buffer ( uint32_t p_conexion, t_header * p_header ) {

	int    stream_size = sizeof( uint32_t ) * 4 + p_header->size;
	void * stream      = malloc( stream_size );
	int    offset      = 0;

	int    valor = 0;
    memset( stream         , valor                        , stream_size        );

    valor = p_header->modulo; // 01 - MODULO
	memcpy( stream + offset, &valor                       , sizeof( uint32_t ) );

	offset += sizeof( uint32_t );

    valor = p_header->id_proceso; // 02 - ID PROCESO
	memcpy( stream + offset, &valor                       , sizeof( uint32_t ) );

	offset += sizeof( uint32_t );

	valor = p_header->nro_msg; // 03 - TIPO MENSAJE
	memcpy( stream + offset, &valor                       , sizeof( uint32_t ) );

	offset += sizeof( uint32_t );

	valor = p_header->size; // 04 - TAMAÑO DE PAYLOAD
	memcpy( stream + offset, &valor                       , sizeof( uint32_t ) );

	offset += sizeof( uint32_t );

	if ( p_header->size > 0 ) { // 05 - PAYLOAD

		memcpy( stream + offset, p_header->payload        , p_header->size     );

		offset += p_header->size;

	}

	bool resultado = true;

//	mem_hexdump(stream, stream_size);

	if ( p_conexion != -1 ) {

		int bytes_enviados=send( p_conexion, stream, stream_size, 0);
		printf("\n");
		log_info(logger,"Se enviaron la cantidad de bytes: %d",bytes_enviados);

		if( -1 == bytes_enviados ) resultado = false;

	} else

		resultado = false;

	free(stream);

	return resultado;

}

t_header * recibir_buffer ( uint32_t socket_cliente ) {

	uint32_t modulo, id_proceso, size;
	cod_msg nro_msg;

	recv(socket_cliente, &modulo     , sizeof(uint32_t), 0);
	recv(socket_cliente, &id_proceso , sizeof(uint32_t), 0);
	recv(socket_cliente, &nro_msg    , sizeof(cod_msg), 0);
	recv(socket_cliente, &size       , sizeof(uint32_t), 0);

	t_header * l_header = malloc( sizeof(t_header) );

	l_header->modulo     = modulo;
	l_header->id_proceso = id_proceso;
	l_header->nro_msg    = nro_msg;
	l_header->size       = size;

	if ( size > 0 ) {

		l_header->payload= malloc(size);

		int bytes_recibidos = recv( socket_cliente, l_header->payload, size, 0 );

//		mem_hexdump(l_header->payload, size);

		if ( bytes_recibidos < size )

			log_error(logger,"Se recibieron menos bytes de los que se esperaban");

	} else l_header->payload = NULL;

	return l_header;

}

bool enviar_02_seleccionar_restaurante( char* p_ip, char* p_puerto, uint32_t p_pos_x,uint32_t p_pos_y,uint32_t p_id_process, char * p_restaurante ) {

	bool _recibir_02_confirmacion_seleccionar_restaurante( int p_conexion ) {

		t_header * header_restaurantes = recibir_buffer( p_conexion );

		printf( "Módulo:       %d.\n" , header_restaurantes->modulo     );
		printf( "ID Proceso:   %d.\n" , header_restaurantes->id_proceso );
		printf( "Nro. mensaje: %d.\n" , header_restaurantes->nro_msg    );
		printf( "Bytes:        %d.\n" , header_restaurantes->size       );

		switch ( header_restaurantes->nro_msg ){

			case SELECCIONAR_RESTAURANTE_OK:
				free(header_restaurantes);
				return true;
				break;

			case SELECCIONAR_RESTAURANTE_FAIL:
				free(header_restaurantes);
				return false;
				break;

			default:
				free(header_restaurantes);
				return NULL;
				break;

		}

	}

	void * l_payload = NULL;

	uint32_t size_payload = 0;

	uint32_t size_resto = 0;

	uint32_t despla = 0;

	if ( p_restaurante != NULL ) {

		size_resto = string_length( p_restaurante );

		size_payload = sizeof(uint32_t) * 3 + size_resto;

		l_payload = malloc( size_payload );

		memcpy( l_payload + despla , &p_pos_x, sizeof(uint32_t) );

		despla += sizeof(uint32_t);

		memcpy( l_payload + despla , &p_pos_y, sizeof(uint32_t) );

		despla += sizeof(uint32_t);

		memcpy( l_payload + despla , &size_resto, sizeof(uint32_t) );

		despla += sizeof(uint32_t);

		memcpy( l_payload + despla , p_restaurante, size_resto );

		despla += size_resto;

	}

	t_header l_header;

	l_header.modulo     = CLIENTE;
	l_header.id_proceso = p_id_process;
	l_header.nro_msg    = SELECCIONAR_RESTAURANTE;
	l_header.size       = size_payload;
	l_header.payload    = l_payload;

	int conexion = crear_socket_y_conectar(p_ip, p_puerto);

	if ( enviar_buffer( conexion, &l_header ) ) {

		bool resultado = _recibir_02_confirmacion_seleccionar_restaurante(conexion);

		close(conexion);

		return resultado;

	} else {

		close(conexion);

		return NULL;

	}

}

void responder_02_seleccionar_restaurante( int socket_cliente, bool seleccionado ) {

	t_header header_response;

	header_response.modulo     = APP;
	header_response.id_proceso = 0;
	header_response.nro_msg    = ( seleccionado ) ? SELECCIONAR_RESTAURANTE_OK : SELECCIONAR_RESTAURANTE_FAIL ;
	header_response.size       = 0;
	header_response.payload    = NULL;

	enviar_buffer( socket_cliente, &header_response);

}

uint32_t enviar_05_crear_pedido ( char* p_ip, char* p_puerto, uint32_t p_id_process ) {

	uint32_t _recibir_id_pedido( uint32_t p_conexion ) {

		t_header * header_restaurantes = recibir_buffer( p_conexion );

		printf( "Módulo:       %d.\n" , header_restaurantes->modulo     );
		printf( "ID Proceso:   %d.\n" , header_restaurantes->id_proceso );
		printf( "Nro. mensaje: %d.\n" , header_restaurantes->nro_msg    );
		printf( "Bytes:        %d.\n" , header_restaurantes->size       );

		switch ( header_restaurantes->nro_msg ){

			case OK: ;

				uint32_t id_recibido = 0;
				memcpy( &id_recibido , header_restaurantes->payload , sizeof(uint32_t) );
				if ( header_restaurantes->payload != NULL ) free( header_restaurantes->payload );
				free(header_restaurantes);
				return id_recibido;
				break;

			case FAIL:
				if ( header_restaurantes->payload != NULL ) free( header_restaurantes->payload );
				free(header_restaurantes);
				return -1;
				break;

			default:
				free(header_restaurantes);
				return -1;
				break;

		}

	}

	t_header l_header;

	l_header.modulo     = CLIENTE;
	l_header.id_proceso = p_id_process;
	l_header.nro_msg    = CREAR_PEDIDO;
	l_header.size       = 0;
	l_header.payload    = NULL;

	uint32_t conexion = crear_socket_y_conectar(p_ip, p_puerto);

	if ( enviar_buffer( conexion, &l_header ) ) {

		uint32_t resultado = _recibir_id_pedido(conexion);

		close(conexion);

		return resultado;

	} else {

		close(conexion);

		return -1;

	}

}

void responder_05_crear_pedido( uint32_t socket_cliente, uint32_t p_id_pedido_creado ) {

	void * v_payload = malloc( sizeof(uint32_t) );

	memcpy( v_payload , &p_id_pedido_creado , sizeof(uint32_t) );

	t_header header_response;

	header_response.modulo     = APP;
	header_response.id_proceso = 0;
	header_response.nro_msg    = ( p_id_pedido_creado == -1 ) ? FAIL : OK ;
	header_response.size       = sizeof(uint32_t);
	header_response.payload    = v_payload;

	enviar_buffer( socket_cliente, &header_response);

	free(v_payload);

}

bool enviar_09_confirmar_pedido ( char* p_ip, char* p_puerto, char * p_nom_resto, uint32_t p_id_pedido ) {

	bool _recibir_09_confirmacion_pedido( uint32_t p_conexion ) {

		t_header * header_response = recibir_buffer( p_conexion );

		bool resultado = ( header_response->nro_msg == OK) ? TRUE : FALSE ;

		free ( header_response );

		return resultado;

	}

	uint32_t size_nombre_resto = string_length( p_nom_resto );

	uint32_t size_payload = sizeof(uint32_t)* 2 + size_nombre_resto;

	void * l_payload = malloc( size_payload );

	uint32_t despla = 0;

	memcpy( l_payload + despla, &p_id_pedido, sizeof(uint32_t) );

	despla += sizeof(uint32_t);

	memcpy( l_payload + despla, &size_nombre_resto, sizeof(uint32_t) );

	despla += sizeof(uint32_t);

	memcpy( l_payload + despla, p_nom_resto, size_nombre_resto );

	despla += size_nombre_resto;

	uint32_t conexion = crear_socket_y_conectar(p_ip, p_puerto);

	t_header header_request;

	header_request.modulo     = APP;
	header_request.id_proceso = 0;
	header_request.nro_msg    = CONFIRMAR_PEDIDO;
	header_request.size       = size_payload;
	header_request.payload    = l_payload;

	if ( enviar_buffer( conexion, &header_request ) ) {

		bool resultado = _recibir_09_confirmacion_pedido(conexion);

		close( conexion );

		return resultado;

	} else {

		close(conexion);

		return false;

	}

}

bool  enviar_09_confirmar_pedido_a_resto   (char* p_ip,char* p_puerto,uint32_t id_proceso,uint32_t id_pedido){

	t_header * encabezado=malloc(sizeof(t_header));
	int offset=0;

	int size_buffer=sizeof(uint32_t);
	void * buffer=malloc(size_buffer);

	memcpy(buffer+offset,&id_pedido,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	encabezado->payload=buffer;
	encabezado->size=size_buffer;
	encabezado->id_proceso=id_proceso;
	encabezado->modulo=CLIENTE;
	encabezado->nro_msg=CONFIRMAR_PEDIDO;


	int conexion =crear_socket_y_conectar(p_ip,p_puerto);

	if(enviar_buffer(conexion,encabezado)==FALSE){
			log_error(logger,"No se pudo enviar el guardado del pedido");
			return FALSE;}

	encabezado=recibir_buffer(conexion);

		log_info(logger,"Se recibio mensaje del modulo numero: %d",encabezado->id_proceso);
		log_info(logger,"Se recibio el mensaje: %s",nro_comando_a_texto(encabezado->nro_msg));
		if(encabezado->nro_msg==OK)return TRUE;
		return FALSE;


}

bool enviar_09_confirmar_pedido_hack ( char* p_ip, char* p_puerto ) {

	bool _recibir_09_confirmacion_pedido( uint32_t p_conexion ) {

		t_header * header_response = recibir_buffer( p_conexion );

		bool resultado = ( header_response->nro_msg == OK) ? TRUE : FALSE ;

		free ( header_response );

		return resultado;

	}

	uint32_t conexion = crear_socket_y_conectar(p_ip, p_puerto);

	t_header header_request;

	header_request.modulo     = APP;
	header_request.id_proceso = 0;
	header_request.nro_msg    = CONFIRMAR_PEDIDO_HACK;
	header_request.size       = 0;
	header_request.payload    = NULL;

	if ( enviar_buffer( conexion, &header_request ) ) {

		bool resultado = _recibir_09_confirmacion_pedido(conexion);

		close( conexion );

		return resultado;

	} else {

		close(conexion);

		return false;

	}

}

void responder_09_confirmar_pedido ( uint32_t socket_cliente, bool p_resultado ) {

	t_header header_response;

	header_response.modulo     = APP;
	header_response.id_proceso = 0;
	header_response.nro_msg    = ( p_resultado ) ? OK : FAIL ;
	header_response.size       = 0;
	header_response.payload    = NULL;

	enviar_buffer( socket_cliente, &header_response);

}

t_list * enviar_04_consultar_platos( char* p_ip, char* p_puerto, uint32_t p_id_process ) {

	t_list * _recibir_lista_platos( uint32_t p_conexion ) {

		t_header * header_restaurantes = recibir_buffer( p_conexion );

		printf( "Módulo:       %d.\n" , header_restaurantes->modulo     );
		printf( "ID Proceso:   %d.\n" , header_restaurantes->id_proceso );
		printf( "Nro. mensaje: %d.\n" , header_restaurantes->nro_msg    );
		printf( "Bytes:        %d.\n" , header_restaurantes->size       );

		t_list   * lista_platos;

		if ( header_restaurantes->nro_msg != CONSULTAR_PLATOS ) {

			perror("No es la respuesta esperada.");
			exit(-1);

		}

		if ( header_restaurantes->size > 0 ) {

			lista_platos = list_create();

			int despla = 0;

			int cantidad_platos = 0;

			memcpy( &cantidad_platos, header_restaurantes->payload + despla, sizeof(uint32_t) );

			despla += sizeof(uint32_t);

			for ( int i = 0; i < cantidad_platos; i++ ) {

				uint32_t act_size;

				memcpy( &act_size    , header_restaurantes->payload + despla, sizeof(uint32_t) );
				despla += sizeof(uint32_t);

				char * nombre_plato = malloc(act_size + 1);

				memcpy( nombre_plato , header_restaurantes->payload + despla, act_size );
				despla += act_size;

				nombre_plato[act_size] = '\0';

				printf("\n%s\n", nombre_plato);

				list_add(lista_platos, nombre_plato);

			}

			return lista_platos;

		} else return NULL;

	}

	t_header l_header;

	l_header.modulo     = CLIENTE;
	l_header.id_proceso = p_id_process;
	l_header.nro_msg    = CONSULTAR_PLATOS;
	l_header.size       = 0;
	l_header.payload    = NULL;

	int conexion = crear_socket_y_conectar(p_ip, p_puerto);

	if ( enviar_buffer( conexion, &l_header ) ) {

		t_list * list_restaurantes = _recibir_lista_platos(conexion);

		close(conexion);

		return list_restaurantes;

	} else {

		close(conexion);

		return NULL;

	}

}

void responder_04_consultar_platos( uint32_t socket_cliente, char ** p_platos ) {

	void * buffer_response = NULL;
	uint32_t buffer_size = 0;
	uint32_t despla = 0;

	uint32_t cantidad_platos = 0;
	uint32_t size_nombres    = 0;

	uint32_t aux = 0;

	if ( p_platos != NULL ) {

		while ( p_platos[aux] != NULL ) {

			cantidad_platos ++;

			size_nombres    += string_length( p_platos[aux] );

			printf("\nEl plato actual es: %s.\n", p_platos[aux] );

			aux ++;

		}

		buffer_size = sizeof(uint32_t)
						+ cantidad_platos * sizeof(uint32_t)
						+ size_nombres;

		buffer_response = malloc(buffer_size);

		memcpy( buffer_response + despla, &cantidad_platos, sizeof(uint32_t) );

		despla += sizeof(uint32_t);

		aux = 0;

		while ( p_platos[aux] != NULL ) {

			uint32_t l_size = (uint32_t) string_length( p_platos[aux] );

			memcpy( buffer_response + despla , &l_size , sizeof( uint32_t ) );

			despla += sizeof( uint32_t );

			memcpy( buffer_response + despla , p_platos[aux] , string_length( p_platos[aux] ) );

			despla += string_length( p_platos[aux] );

			aux ++;

		}

	}

	t_header header_response;

	header_response.modulo     = APP;
	header_response.id_proceso = 0;
	header_response.nro_msg    = CONSULTAR_PLATOS;
	header_response.size       = buffer_size;
	header_response.payload    = buffer_response;

	enviar_buffer( socket_cliente, &header_response);

	if ( buffer_response != NULL ) free( buffer_response );

}

bool enviar_07_aniadir_plato( char * p_ip, char * p_puerto, uint32_t p_id_proceso, uint32_t p_id_pedido, char * p_plato ) {

					bool _recibir_confirmacion( uint32_t p_conexion ) {

						t_header * header_restaurantes = recibir_buffer( p_conexion );

						printf( "Módulo:       %d.\n" , header_restaurantes->modulo     );
						printf( "ID Proceso:   %d.\n" , header_restaurantes->id_proceso );
						printf( "Nro. mensaje: %d.\n" , header_restaurantes->nro_msg    );
						printf( "Bytes:        %d.\n" , header_restaurantes->size       );

						switch ( header_restaurantes->nro_msg ){

							case OK:

								if ( header_restaurantes->payload != NULL ) free( header_restaurantes->payload );

								free(header_restaurantes);

								return true;

								break;

							case FAIL:

								if ( header_restaurantes->payload != NULL ) free( header_restaurantes->payload );

								free(header_restaurantes);

								return false;

								break;

							default:

								free(header_restaurantes);

								return false;

								break;

						}

					}

	if ( p_plato == NULL ) {

		printf("Debe ingresar un plato para añadir al pedido.\n");

		return false;

	}

	log_info(logger,"El id del plato que se quiere enviar es: %d",p_id_pedido);

	uint32_t size_nombre_plato = string_length( p_plato );

	uint32_t size_payload = sizeof(uint32_t) * 2 + size_nombre_plato;

	void * l_payload = malloc( size_payload );

	uint32_t despla = 0;

//	uint32_t cant_plato = 1;

	memcpy( l_payload + despla, &p_id_pedido, sizeof(uint32_t) );

	despla += sizeof(uint32_t);

	memcpy( l_payload + despla, &size_nombre_plato, sizeof(uint32_t) );

	despla += sizeof(uint32_t);

	memcpy( l_payload + despla, p_plato, size_nombre_plato );

	despla += size_nombre_plato;

	mem_hexdump(l_payload,size_payload);

	t_header l_header;

	l_header.modulo     = CLIENTE;
	l_header.id_proceso = p_id_proceso;
	l_header.nro_msg    = ANIADIR_PLATO;
	l_header.size       = size_payload;
	l_header.payload    = l_payload;

	uint32_t conexion = crear_socket_y_conectar(p_ip, p_puerto);

	if ( enviar_buffer( conexion, &l_header ) ) {

		bool resultado = _recibir_confirmacion (conexion);

		close(conexion);

		return resultado;

	} else {

		close(conexion);

		return false;

	}

}

void responder_07_aniadir_plato( uint32_t socket_cliente, bool p_resultado ) {

	t_header header_response;

	header_response.modulo     = APP;
	header_response.id_proceso = 0;
	header_response.nro_msg    = ( p_resultado ) ? OK : FAIL ;
	header_response.size       = 0;
	header_response.payload    = NULL;

	enviar_buffer( socket_cliente, &header_response);

}

bool enviar_06_guardar_pedido   (char* p_ip,char* p_puerto, char * p_nom_resto, uint32_t p_id_pedido){

	bool _recibir_confirmacion_06_guardar_pedido( uint32_t p_conexion ) {

		t_header * header_restaurantes = recibir_buffer( p_conexion );

		printf( "Módulo:       %d.\n" , header_restaurantes->modulo     );
		printf( "ID Proceso:   %d.\n" , header_restaurantes->id_proceso );
		printf( "Nro. mensaje: %d.\n" , header_restaurantes->nro_msg    );
		printf( "Bytes:        %d.\n" , header_restaurantes->size       );

		switch ( header_restaurantes->nro_msg ){

			case OK: ;

				if ( header_restaurantes->payload != NULL ) free( header_restaurantes->payload );
				free(header_restaurantes);
				return TRUE;
				break;

			case FAIL:
				if ( header_restaurantes->payload != NULL ) free( header_restaurantes->payload );
				free(header_restaurantes);
				return FALSE;
				break;

			default:
				free(header_restaurantes);
				return -1;
				break;

		}

	}

	if ( p_nom_resto == NULL || p_id_pedido == -1 ) {

		printf("Debe ingresar un restaurante y ID de pedido para guardar.\n");

		return false;

	}

	uint32_t size_nombre_resto = string_length( p_nom_resto );

	uint32_t size_payload = sizeof(uint32_t) * 2 + size_nombre_resto;

	void * l_payload = malloc( size_payload );

	uint32_t despla = 0;

	memcpy( l_payload + despla, &p_id_pedido, sizeof(uint32_t) );

	despla += sizeof(uint32_t);

	memcpy( l_payload + despla, &size_nombre_resto, sizeof(uint32_t) );

	despla += sizeof(uint32_t);

	memcpy( l_payload + despla, p_nom_resto, size_nombre_resto );

	despla += size_nombre_resto;

	t_header l_header;

	l_header.modulo     = APP;
	l_header.id_proceso = 0;
	l_header.nro_msg    = GUARDAR_PEDIDO;
	l_header.size       = size_payload;
	l_header.payload    = l_payload;

	uint32_t conexion = crear_socket_y_conectar(p_ip, p_puerto);

	if ( enviar_buffer( conexion, &l_header ) ) {

		bool resultado = _recibir_confirmacion_06_guardar_pedido(conexion);

		close(conexion);

		return resultado;

	} else {

		close(conexion);

		return FALSE;

	}

}


bool enviar_12_obtener_pedido   (char* p_ip,char* p_puerto, char * p_nom_resto, uint32_t p_id_pedido){

	bool _recibir_12_obtener_pedido( uint32_t p_conexion ) {

		t_header * header_restaurantes = recibir_buffer( p_conexion );

		printf( "Módulo:       %d.\n" , header_restaurantes->modulo     );
		printf( "ID Proceso:   %d.\n" , header_restaurantes->id_proceso );
		printf( "Nro. mensaje: %d.\n" , header_restaurantes->nro_msg    );
		printf( "Bytes:        %d.\n" , header_restaurantes->size       );

		if ( header_restaurantes->nro_msg == RESPUESTA_OBTENER_PEDIDO ) {

			uint32_t despla = 0;

			uint32_t estado = 0;

			memcpy( &estado, header_restaurantes->payload + despla, sizeof(uint32_t) );//ERROR, PUEDE PORQ NO ENCONTRO EL PEDIDO

			despla += sizeof(uint32_t);

			uint32_t cantidad = 1;

			// memcpy( &cantidad, header_restaurantes->payload + despla, sizeof(uint32_t) );

			despla += sizeof(uint32_t);

			for ( int i = 0; i < cantidad ; i++ ) {

				uint32_t cant_lista = 0;

				memcpy( &cant_lista, header_restaurantes->payload + despla, sizeof(uint32_t) );

				despla += sizeof(uint32_t);

				uint32_t cant_total = 0;

				memcpy( &cant_total, header_restaurantes->payload + despla, sizeof(uint32_t) );

				despla += sizeof(uint32_t);

				char * nombre_plato = malloc(24+1);

				memcpy( nombre_plato, header_restaurantes->payload + despla, 24 );

				nombre_plato[24] = '\0';

				despla += sizeof(uint32_t);

				printf("Hay %d platos listos de %d pedidos de '%s'\n", cant_lista, cant_total, nombre_plato );

			}

		} else {

		}

		if ( header_restaurantes->payload != NULL ) free( header_restaurantes->payload );
		free(header_restaurantes);
		return FALSE;

	}

	if ( p_nom_resto == NULL || p_id_pedido == -1 ) {

		printf("Debe ingresar un restaurante y ID de pedido para guardar.\n");

		return false;

	}

	uint32_t size_nombre_resto = string_length( p_nom_resto );

	uint32_t size_payload = sizeof(uint32_t) * 2 + size_nombre_resto;

	void * l_payload = malloc( size_payload );

	uint32_t despla = 0;

	memcpy( l_payload + despla, &p_id_pedido, sizeof(uint32_t) );

	despla += sizeof(uint32_t);

	memcpy( l_payload + despla, &size_nombre_resto, sizeof(uint32_t) );

	despla += sizeof(uint32_t);

	memcpy( l_payload + despla, p_nom_resto, size_nombre_resto );

	despla += size_nombre_resto;

	t_header l_header;

	l_header.modulo     = APP;
	l_header.id_proceso = 0;
	l_header.nro_msg    = OBTENER_PEDIDO;
	l_header.size       = size_payload;
	l_header.payload    = l_payload;

	uint32_t conexion = crear_socket_y_conectar(p_ip, p_puerto);

	if ( enviar_buffer( conexion, &l_header ) ) {

		bool resultado = _recibir_12_obtener_pedido(conexion);

		close(conexion);

		return resultado;

	} else {

		close(conexion);

		return FALSE;

	}

}

int enviar_guardar_pedido   (char* p_ip,char* p_puerto,char * nombre_restaurante,uint32_t id_pedido){

	uint32_t nro_msg=GUARDAR_PEDIDO;
	t_header * encabezado=serializar_pedido(nro_msg,nombre_restaurante,id_pedido);

	int conexion =crear_socket_y_conectar(p_ip,p_puerto);

	if(enviar_buffer(conexion,encabezado)==FALSE)log_error(logger,"No se pudo enviar el guardado del pedido");

	return conexion;

}

int enviar_obtener_pedido   (char* p_ip,char* p_puerto,char * nombre_restaurante,uint32_t id_pedido){

	uint32_t nro_msg=OBTENER_PEDIDO;
	t_header * encabezado=serializar_pedido(nro_msg,nombre_restaurante,id_pedido);

	int conexion =crear_socket_y_conectar(p_ip,p_puerto);

	if(enviar_buffer(conexion,encabezado)==FALSE)log_error(logger,"No se pudo enviar el obtener pedido");

	return conexion;
}



int  enviar_confirmar_pedido   (char* p_ip,char* p_puerto, char * nombre_restaurante,uint32_t id_pedido){

	uint32_t nro_msg=CONFIRMAR_PEDIDO;
	t_header * encabezado=serializar_pedido(nro_msg, nombre_restaurante,id_pedido);

	int conexion =crear_socket_y_conectar(p_ip,p_puerto);

	if(enviar_buffer(conexion,encabezado)==FALSE)log_error(logger,"No se pudo enviar el guardado del pedido");

	return conexion;
}


bool enviar_13_finalizar_pedido   (char* p_ip,char* p_puerto, char * p_nom_resto, uint32_t p_id_pedido){

	bool _recibir_confirmacion_13_finalizar_pedido( uint32_t p_conexion ) {

		t_header * header_restaurantes = recibir_buffer( p_conexion );

		printf( "Módulo:       %d.\n" , header_restaurantes->modulo     );
		printf( "ID Proceso:   %d.\n" , header_restaurantes->id_proceso );
		printf( "Nro. mensaje: %d.\n" , header_restaurantes->nro_msg    );
		printf( "Bytes:        %d.\n" , header_restaurantes->size       );

		switch ( header_restaurantes->nro_msg ){

			case OK: ;

				if ( header_restaurantes->payload != NULL ) free( header_restaurantes->payload );
				free(header_restaurantes);
				return TRUE;
				break;

			case FAIL:
				if ( header_restaurantes->payload != NULL ) free( header_restaurantes->payload );
				free(header_restaurantes);
				return FALSE;
				break;

			default:
				free(header_restaurantes);
				return -1;
				break;

		}

	}

	if ( p_nom_resto == NULL || p_id_pedido == -1 ) {

		printf("Debe ingresar un restaurante y ID de pedido para finalizar el pedido.\n");

		return false;

	}

	uint32_t size_nombre_resto = string_length( p_nom_resto );

	uint32_t size_payload = sizeof(uint32_t) * 2 + size_nombre_resto;

	void * l_payload = malloc( size_payload );

	uint32_t despla = 0;

	memcpy( l_payload + despla, &p_id_pedido, sizeof(uint32_t) );

	despla += sizeof(uint32_t);

	memcpy( l_payload + despla, &size_nombre_resto, sizeof(uint32_t) );

	despla += sizeof(uint32_t);

	memcpy( l_payload + despla, p_nom_resto, size_nombre_resto );

	despla += size_nombre_resto;

	t_header * l_header = serializar_pedido( FINALIZAR_PEDIDO, p_nom_resto, p_id_pedido );

	uint32_t conexion = crear_socket_y_conectar(p_ip, p_puerto);

	if ( enviar_buffer( conexion, l_header ) ) {

		bool resultado = _recibir_confirmacion_13_finalizar_pedido(conexion);

		close(conexion);

		return resultado;

	} else {

		close(conexion);

		return FALSE;

	}

}

int enviar_finalizar_pedido   (char* p_ip,char* p_puerto, char * nombre_restaurante,uint32_t id_pedido){

	uint32_t nro_msg=FINALIZAR_PEDIDO;
	t_header * encabezado=serializar_pedido(nro_msg, nombre_restaurante,id_pedido);

	int conexion =crear_socket_y_conectar(p_ip,p_puerto);

	if(enviar_buffer(conexion,encabezado)==FALSE)log_error(logger,"No se pudo enviar el guardado del pedido");

	return conexion;

}

t_header * serializar_pedido       (uint32_t nro_msg, char * nombre_restaurante,uint32_t id_pedido        ){

	t_pedido * pedido=malloc(sizeof(t_pedido));
	t_header * encabezado=malloc(sizeof(t_header));
	int offset=0;

	pedido->id_pedido=id_pedido;
	pedido->size_nombre_restaurante=(uint32_t)string_length(nombre_restaurante);
	pedido->nombre_restaurante=malloc(pedido->size_nombre_restaurante);
	pedido->nombre_restaurante=nombre_restaurante;

	int size_buffer=2*sizeof(uint32_t)+pedido->size_nombre_restaurante;
	void * buffer=malloc(size_buffer);

	memcpy(buffer+offset,&pedido->id_pedido,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(buffer+offset,&pedido->size_nombre_restaurante,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(buffer+offset,pedido->nombre_restaurante,pedido->size_nombre_restaurante);

	encabezado->payload=buffer;
	encabezado->size=size_buffer;
	encabezado->id_proceso=1;
	encabezado->modulo=2;
	encabezado->nro_msg=nro_msg;

	return encabezado;
}

t_pedido * recibir_pedido  (void * payload         ){

	t_pedido * pedido=malloc(sizeof(t_pedido));

	memcpy(&(pedido->id_pedido),payload,sizeof(uint32_t));
	payload+=sizeof(uint32_t);

	memcpy(&(pedido->size_nombre_restaurante),payload,sizeof(uint32_t));
	payload+=sizeof(uint32_t);

	pedido->nombre_restaurante=malloc((pedido->size_nombre_restaurante)+1);
	memcpy(pedido->nombre_restaurante,payload,pedido->size_nombre_restaurante);
	pedido->nombre_restaurante[pedido->size_nombre_restaurante]='\0';

	log_info(logger,"Se recibio el id numero: %d",pedido->id_pedido);
//	log_info(logger,"Se recibio el size del nombre %d",pedido->size_nombre_restaurante);
	log_info(logger,"Se recibio el restaurante %s",pedido->nombre_restaurante);

	return pedido;

}

bool enviar_08_guardar_plato( char * p_ip       , char *   p_puerto
		                    , char * p_nom_resto, uint32_t p_id_pedido
		                    , char * p_nom_plato, uint32_t p_cant_plato ){

	bool _recibir_confirmacion_08_guardar_plato( uint32_t p_conexion ) {

		t_header * header_restaurantes = recibir_buffer( p_conexion );

		printf( "Módulo:       %d.\n" , header_restaurantes->modulo     );
		printf( "ID Proceso:   %d.\n" , header_restaurantes->id_proceso );
		printf( "Nro. mensaje: %d.\n" , header_restaurantes->nro_msg    );
		printf( "Bytes:        %d.\n" , header_restaurantes->size       );

		switch ( header_restaurantes->nro_msg ){

			case OK: ;

				if ( header_restaurantes->payload != NULL ) free( header_restaurantes->payload );
				free(header_restaurantes);
				return TRUE;
				break;

			case FAIL:
				if ( header_restaurantes->payload != NULL ) free( header_restaurantes->payload );
				free(header_restaurantes);
				return FALSE;
				break;

			default:
				free(header_restaurantes);
				return FALSE;
				break;

		}

	}

	if ( p_nom_resto == NULL || p_id_pedido == -1 ) {

		printf("Debe ingresar un restaurante y ID de pedido para guardar.\n");

		return false;

	}

	// ID_PEDIDO + NOM_RESTO + NOM_PLATO + CANTIDAD_PLATO

	uint32_t size_nombre_resto = string_length( p_nom_resto );

	uint32_t size_nombre_plato = string_length( p_nom_plato );

	uint32_t size_payload = sizeof(uint32_t) * 4 + size_nombre_resto + size_nombre_plato;

	void * l_payload = malloc( size_payload );

	uint32_t despla = 0;

	memcpy( l_payload + despla, &p_id_pedido, sizeof(uint32_t) );

	despla += sizeof(uint32_t);

	memcpy( l_payload + despla, &size_nombre_resto, sizeof(uint32_t) );

	despla += sizeof(uint32_t);

	memcpy( l_payload + despla, p_nom_resto, size_nombre_resto );

	despla += size_nombre_resto;

	memcpy( l_payload + despla, &size_nombre_plato, sizeof(uint32_t) );

	despla += sizeof(uint32_t);

	memcpy( l_payload + despla, p_nom_plato, size_nombre_plato );

	despla += size_nombre_plato;

	memcpy( l_payload + despla, &p_cant_plato, sizeof(uint32_t) );

	despla += sizeof(uint32_t);

	t_header l_header;

	l_header.modulo     = APP;
	l_header.id_proceso = 0;
	l_header.nro_msg    = GUARDAR_PLATO;
	l_header.size       = size_payload;
	l_header.payload    = l_payload;

	uint32_t conexion = crear_socket_y_conectar(p_ip, p_puerto);

	if ( enviar_buffer( conexion, &l_header ) ) {

		bool resultado = _recibir_confirmacion_08_guardar_plato(conexion);

		close(conexion);

		return resultado;

	} else {

		close(conexion);

		return FALSE;

	}

}

int 	   enviar_guardar_plato    (char* p_ip,char* p_puerto, char * nombre_restaurante,uint32_t id_pedido, char * nombre_plato, uint32_t cantidad_plato){

	t_guardar_plato * plato=malloc(sizeof(t_guardar_plato));
	plato->pedido=malloc(sizeof(t_pedido));
	t_header * encabezado=malloc(sizeof(t_header));
	int offset=0;

	plato->pedido->id_pedido=id_pedido;
	plato->pedido->size_nombre_restaurante=string_length(nombre_restaurante);
	plato->pedido->nombre_restaurante=malloc(plato->pedido->size_nombre_restaurante);
	plato->pedido->nombre_restaurante=nombre_restaurante;
	plato->size_nombre_plato=string_length(nombre_plato);
	plato->nombre_plato=malloc(plato->size_nombre_plato);
	plato->nombre_plato=nombre_plato;
	plato->cantidad_plato=cantidad_plato;

	printf("Voy a enviar el plato con nombre: %s \n",plato->nombre_plato);
	printf("El tamaño del nombre sin caracter nulo es: %d \n", string_length(plato->nombre_plato));

	int size_buffer=4*sizeof(uint32_t)+plato->pedido->size_nombre_restaurante+plato->size_nombre_plato;
	void * buffer=malloc(size_buffer);

	memcpy(buffer+offset,&plato->pedido->id_pedido,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(buffer+offset,&plato->pedido->size_nombre_restaurante,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(buffer+offset,plato->pedido->nombre_restaurante,plato->pedido->size_nombre_restaurante);
	offset+=plato->pedido->size_nombre_restaurante;

	memcpy(buffer+offset,&plato->size_nombre_plato,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(buffer+offset,plato->nombre_plato,plato->size_nombre_plato);
	offset+=plato->size_nombre_plato;

	memcpy(buffer+offset,&plato->cantidad_plato,sizeof(uint32_t));

	encabezado->payload=buffer;
	encabezado->size=size_buffer;
	encabezado->id_proceso=2;
	encabezado->modulo=3;
	encabezado->nro_msg=GUARDAR_PLATO;

	int conexion =crear_socket_y_conectar(p_ip,p_puerto);

	if(enviar_buffer(conexion,encabezado)==FALSE)log_error(logger,"No se pudo enviar el guardado del plato");

	return conexion;

}

t_guardar_plato * recibir_guardar_plato   (void * payload         ){

	t_guardar_plato * plato=malloc(sizeof(t_guardar_plato));
	plato->pedido=malloc(sizeof(t_pedido));

	plato->pedido=recibir_pedido(payload);

	int size_pedido=2*sizeof(uint32_t)+plato->pedido->size_nombre_restaurante;
	payload+=size_pedido;

	memcpy(&(plato->size_nombre_plato),payload,sizeof(uint32_t));
	payload+=sizeof(uint32_t);

	plato->nombre_plato=malloc((plato->size_nombre_plato)+1);
	memcpy(plato->nombre_plato,payload,plato->size_nombre_plato);
	plato->nombre_plato[plato->size_nombre_plato]='\0';
	payload+=plato->size_nombre_plato;

	memcpy(&(plato->cantidad_plato),payload,sizeof(uint32_t));
	payload+=sizeof(uint32_t);

	log_info(logger,"Se recibio el size del nombre del plato: %d",plato->size_nombre_plato);
	log_info(logger,"Se recibio el plato %s",plato->nombre_plato);
	log_info(logger,"Se recibio la cantidad de platos %d",plato->cantidad_plato);

	return plato;

}

int 	   enviar_plato_listo    (char* p_ip,char* p_puerto, char * nombre_restaurante,uint32_t id_pedido, char * nombre_plato){

	t_plato_listo * plato=malloc(sizeof(t_plato_listo));
	plato->pedido=malloc(sizeof(t_pedido));
	t_header * encabezado=malloc(sizeof(t_header));
	int offset=0;

	plato->pedido->id_pedido=id_pedido;
	plato->pedido->size_nombre_restaurante=string_length(nombre_restaurante);
	plato->pedido->nombre_restaurante=malloc(plato->pedido->size_nombre_restaurante);
	plato->pedido->nombre_restaurante=nombre_restaurante;
	plato->size_nombre_plato=string_length(nombre_plato);
	plato->nombre_plato=malloc(plato->size_nombre_plato);
	plato->nombre_plato=nombre_plato;

	int size_buffer=3*sizeof(uint32_t)+plato->pedido->size_nombre_restaurante+plato->size_nombre_plato;
	void * buffer=malloc(size_buffer);

	memcpy(buffer+offset,&plato->pedido->id_pedido,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(buffer+offset,&plato->pedido->size_nombre_restaurante,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(buffer+offset,plato->pedido->nombre_restaurante,plato->pedido->size_nombre_restaurante);
	offset+=plato->pedido->size_nombre_restaurante;

	memcpy(buffer+offset,&plato->size_nombre_plato,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(buffer+offset,plato->nombre_plato,plato->size_nombre_plato);

	encabezado->payload=buffer;
	encabezado->size=size_buffer;
	encabezado->id_proceso=2;
	encabezado->modulo=3;
	encabezado->nro_msg=PLATO_LISTO;

	int conexion =crear_socket_y_conectar(p_ip,p_puerto);

	if(enviar_buffer(conexion,encabezado)==FALSE)log_error(logger,"No se pudo enviar el guardado del plato");

	return conexion;

}

t_plato_listo *	recibir_plato_listo     (void * payload){

	t_plato_listo * plato=malloc(sizeof(t_plato_listo));
	plato->pedido=malloc(sizeof(t_pedido));

	plato->pedido=recibir_pedido(payload);

	int size_pedido=2*sizeof(uint32_t)+plato->pedido->size_nombre_restaurante;
	payload+=size_pedido;

	memcpy(&(plato->size_nombre_plato),payload,sizeof(uint32_t));
	payload+=sizeof(uint32_t);

	plato->nombre_plato=malloc(plato->size_nombre_plato+1);
	memcpy(plato->nombre_plato,payload,plato->size_nombre_plato);
	plato->nombre_plato[plato->size_nombre_plato]='\0';


	log_info(logger,"Se recibio el size del nombre del plato: %d",plato->size_nombre_plato);
	log_info(logger,"Se recibio el plato %s",plato->nombre_plato);

	return plato;

}

void deserializar_respuesta_obtener_pedido(t_header * encabezado){

	t_list * lista_platos_del_pedido=list_create();
	int cantidad_total_platos=encabezado->size/sizeof(t_comida);
	t_pedido_seg * pedido=malloc(sizeof(t_pedido));
	int offset=0;

			void mostrar_platos_pedido(void * elemento){
				t_comida * comida=(t_comida*)elemento;
				printf("-------------------\n");
				log_info(logger,"El plato es: %s",comida->nombre_comida);
				log_info(logger,"La cantidad total pedida del plato es: %d",comida->cantidad_total_comida);
				log_info(logger,"La cantidad total pedida que ya se encuentra lista es: %d",comida->cantidad_lista_comida);
				printf("\n");
			}

	log_info(logger,"Por empezar a deserializar");
	log_info(logger,"cantidad total de los diferentes tipos de platos: %d",cantidad_total_platos);
//	log_error(logger,"el tamaño del payload es: %d",encabezado->size);
//	log_error(logger,"el sizeof de t_comida es: %d",sizeof(t_comida));

	memcpy(&(pedido->estado),encabezado->payload+offset,sizeof(estado_pedido));
	offset+=sizeof(estado_pedido);

	for(int i=0;i<cantidad_total_platos;i++){
		t_comida * comida=malloc(sizeof(t_comida));

		memcpy(&(comida->cantidad_lista_comida),encabezado->payload+offset,sizeof(uint32_t));
		offset+=sizeof(uint32_t);

		memcpy(&(comida->cantidad_total_comida),encabezado->payload+offset,sizeof(uint32_t));
		offset+=sizeof(uint32_t);

		memcpy(comida->nombre_comida,encabezado->payload+offset,SIZE_VECTOR_NOMBRE_PLATO);
		offset+=SIZE_VECTOR_NOMBRE_PLATO;

		list_add(lista_platos_del_pedido,comida);
//		log_error(logger,"El numero de iteracion es: %d",i);
	}

	pedido->comidas_del_pedido=lista_platos_del_pedido;

	log_info(logger,"El pedido se encuentra en estado: %s",nro_estado_pedido_a_texto(pedido->estado));
	log_info(logger,"Los platos son los siguientes:");
	printf("\n");

	list_iterate(pedido->comidas_del_pedido,mostrar_platos_pedido);

	list_destroy(lista_platos_del_pedido);
}

int serializar(void* buffer, const char* format, ...){
	va_list objs;
	int i = 0, buffIndex = 0, sizeObj;
	va_start(objs, format);
	void * pObj;

	while(format[i] != '\0'){
		if( format[i] == '%'){
			i++;
			switch( format[i] ){
				case 'h':{
					short obj = (short)va_arg( objs, int );
					sizeObj = sizeof(short);
					pObj = &obj;
					break;}
				case 'd':{
					int obj = (int)va_arg( objs, int );
					sizeObj = sizeof(int);
					pObj = &obj;
					break;}
				case 'c':{
					char obj = (char)va_arg(objs, int);
					sizeObj  = sizeof(char);
					pObj = &obj;
					break;}
				case 's':{
					char *obj = va_arg(objs, char*);
					sizeObj = strlen(obj)+1;
					pObj = obj;
					break;}
				case 'z':{
					uint32_t obj = (uint32_t)va_arg( objs, uint32_t );
					sizeObj = sizeof(int);
					pObj = &obj;
					break;}
				default:
					va_end(objs);
					return -1;
			} /* FIN DE DETECCION DE CARACTER POST-'%' */
		} /* FIN DEL IF '%' */
		
		memcpy(buffer+buffIndex,pObj, sizeObj);
		buffIndex += sizeObj;
		i++;
	} /* FIN DEL WHILE RECORRE-CADENA */

	va_end(objs);

	return buffIndex;
}

int deserializar(void* buffer, const char* format, ...){
	va_list objs;
	int i = 0, buffIndex = 0, sizeObj;
	va_start(objs, format);
	void * obj;

	while(format[i] != '\0'){
		if( format[i] == '%'){
			i++;
			switch( format[i] ){
				case 'h':
					obj = (short *)va_arg( objs, int );
					sizeObj = sizeof(short);
					break;
				case 'd':
					obj = (int *)va_arg( objs, int );
					sizeObj = sizeof(int);
					break;
				case 'z':
					obj = (uint32_t *)va_arg( objs, uint32_t );
					sizeObj = sizeof(uint32_t);
					break;
				case 'c':
					obj = (char *)va_arg(objs, int);
					sizeObj  = sizeof(char);
					break;
				case 's':
					obj = (char *)va_arg(objs, char*);
					sizeObj = strlen(buffer+buffIndex)+1;
					break;
				default:
					va_end(objs);
					return -1;
			} // FIN DE DETECCION DE CARACTER POST-'%'
		} // FIN DEL IF '%'
		memcpy(obj, buffer+buffIndex, sizeObj);
		buffIndex += sizeObj;
		i++;
	} // FIN DEL WHILE RECORRE-CADENA

	va_end(objs);

	return buffIndex;
}

t_header * serializar_respuesta_info_restaurante(t_respuesta_info_restaurante * respuesta_info) {
	int offset = 0;
	t_header * header = malloc(sizeof(t_header));

	int size_buffer = sizeof(uint32_t) * 7 + respuesta_info->size_afinidad_cocineros
			+ respuesta_info->size_platos + respuesta_info->size_precio_platos;
	void * buffer = malloc(size_buffer);

	memcpy(buffer + offset, &respuesta_info->cantidad_cocineros, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer + offset, &respuesta_info->posicion_x, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer + offset, &respuesta_info->posicion_y, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer + offset, &respuesta_info->size_afinidad_cocineros, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer+offset, respuesta_info->afinidad_cocineros, respuesta_info->size_afinidad_cocineros);
	offset += respuesta_info->size_afinidad_cocineros;

	memcpy(buffer + offset, &respuesta_info->size_platos, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer+offset, respuesta_info->platos, respuesta_info->size_platos);
	offset += respuesta_info->size_platos;

	memcpy(buffer + offset, &respuesta_info->size_precio_platos, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer+offset, respuesta_info->precio_platos, respuesta_info->size_precio_platos);
	offset += respuesta_info->size_precio_platos;

	memcpy(buffer + offset, &respuesta_info->cantidad_hornos, sizeof(uint32_t));

	header->payload = buffer;
	header->size = size_buffer;
	header->id_proceso = 2; //TODO: esta hardcodeado el 2, hay que modificarlo desp
	header->modulo = SINDICATO;
	header->nro_msg = OBTENER_RESTAURANTES;

	return header;
}

void sigint(int a) {

	log_destroy(logger);
	config_destroy(config);
	close( g_socket_cliente );

	exit(1);

}

uint32_t random_id_generator( void ) {

	sleep(1);

	srand(time(NULL));    // Initialization, should only be called once.
	uint32_t r = rand();  // Returns a pseudo-random integer between 0 and RAND_MAX.

	return r;

}

void _string_destroyer( void * p_elem ) {

	if ( p_elem != NULL ) free(p_elem);

}
//  ACA TENGO Q TERMINAR
char ** enviar_04_consultar_platos_app_a_resto(uint32_t socket_conectado){

	t_header header_response;
	char ** vector_platos=NULL;

	header_response.modulo     = APP;
	header_response.id_proceso = 0;
	header_response.nro_msg    = CONSULTAR_PLATOS;
	header_response.size       = 0;
	header_response.payload    = NULL;

	enviar_buffer( socket_conectado, &header_response);

	t_header * encabezado=recibir_buffer(socket_conectado);

	//Tengo q deserializar y cargarlo en un vector y retornarlo

	vector_platos=deserializar_respuesta_consultar_platos(encabezado);

	return vector_platos;

}

char ** deserializar_respuesta_consultar_platos(t_header * encabezado){

	char * platos_cadena=NULL;
	//int size_string=0;
	char ** vector_platos=NULL;


	platos_cadena=malloc(encabezado->size+1);//------------------------------ver si viene con o sin el fin de cadena
	memcpy(platos_cadena,encabezado->payload+sizeof(uint32_t),encabezado->size);

	vector_platos=string_split(platos_cadena,",");

	return vector_platos;
}


int enviar_11_consultar_pedido(char* p_ip,char* p_puerto,uint32_t id_proceso,uint32_t id_pedido){

	t_header header_response;
	void * payload=malloc(sizeof(uint32_t));

	memcpy(payload,&id_pedido,sizeof(uint32_t));


	header_response.modulo     = APP;
	header_response.id_proceso = id_proceso;
	header_response.nro_msg    = CONSULTAR_PEDIDO;
	header_response.size       = sizeof(uint32_t);
	header_response.payload    = payload;

	int conexion =crear_socket_y_conectar(p_ip,p_puerto);

	enviar_buffer( conexion, &header_response);

	return conexion;

}

void deserializar_11_respuesta_consultar_pedido(t_header * encabezado){

	t_list * lista_platos_del_pedido=list_create();
	int cantidad_total_platos=(encabezado->size-2*sizeof(uint32_t))/sizeof(t_comida);
	t_pedido_seg * pedido=malloc(sizeof(t_pedido));
	int size_nombre_resto=0;
	int offset=0;

			void mostrar_platos_pedido(void * elemento){
				t_comida * comida=(t_comida*)elemento;
				printf("-------------------\n");
				log_info(logger,"El plato es: %s",comida->nombre_comida);
				log_info(logger,"La cantidad total pedida del plato es: %d",comida->cantidad_total_comida);
				log_info(logger,"La cantidad total pedida que ya se encuentra lista es: %d",comida->cantidad_lista_comida);
				printf("\n");
			}

	log_info(logger,"Por empezar a deserializar");
	log_info(logger,"cantidad total de los diferentes tipos de platos: %d",cantidad_total_platos);
//	log_error(logger,"el tamaño del payload es: %d",encabezado->size);
//	log_error(logger,"el sizeof de t_comida es: %d",sizeof(t_comida));

	memcpy(&(size_nombre_resto),encabezado->payload+offset,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	char * nombre_resto=malloc(size_nombre_resto+1);
	memcpy(nombre_resto,encabezado->payload+offset,size_nombre_resto);
	nombre_resto[size_nombre_resto]='\0';
	offset+=size_nombre_resto;

	memcpy(&(pedido->estado),encabezado->payload+offset,sizeof(estado_pedido));
	offset+=sizeof(estado_pedido);

	for(int i=0;i<cantidad_total_platos;i++){
		t_comida * comida=malloc(sizeof(t_comida));

		memcpy(&(comida->cantidad_lista_comida),encabezado->payload+offset,sizeof(uint32_t));
		offset+=sizeof(uint32_t);

		memcpy(&(comida->cantidad_total_comida),encabezado->payload+offset,sizeof(uint32_t));
		offset+=sizeof(uint32_t);

		memcpy(comida->nombre_comida,encabezado->payload+offset,SIZE_VECTOR_NOMBRE_PLATO);
		offset+=SIZE_VECTOR_NOMBRE_PLATO;

		list_add(lista_platos_del_pedido,comida);
//		log_error(logger,"El numero de iteracion es: %d",i);
	}

	pedido->comidas_del_pedido=lista_platos_del_pedido;

	log_info(logger,"El pedido se encuentra en el restaurante: %s",nombre_resto);
	log_info(logger,"El pedido se encuentra en estado: %s",nro_estado_pedido_a_texto(pedido->estado));
	log_info(logger,"Los platos son los siguientes:");
	printf("\n");

	list_iterate(pedido->comidas_del_pedido,mostrar_platos_pedido);

	list_destroy(lista_platos_del_pedido);



}

uint32_t recibir_11_consultar_pedido(void * payload){

	uint32_t id_pedido=0;

	memcpy(&id_pedido,payload,sizeof(uint32_t));

	return id_pedido;


}
