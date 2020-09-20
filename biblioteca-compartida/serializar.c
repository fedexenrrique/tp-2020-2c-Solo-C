#include "serializar.h"

void prueba_biblioteca_compartida(void) {
	printf("\n!!!Texto impreso desde la biblioteca compartida!!!\n");
}

t_list * enviar_consultar_restaurante(char* p_ip, char* p_puerto) {

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

				char * nombre_resto = malloc(act_size + 1);

				memcpy( nombre_resto , header_restaurantes->payload + despla, act_size );
				despla += act_size;

				nombre_resto[act_size] = '\0';

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

	int conexion = crear_socket_y_conectar(p_ip, p_puerto);

	if ( enviar_buffer( conexion, &l_header ) ) {

		t_list * restaurantes_recibidos = _recibir_lista_restaurantes(conexion);

		close( conexion );

		return restaurantes_recibidos;

	} else {

		close(conexion);

		return NULL;

	}

}

void recibir_consultar_restaurante_y_responder( int socket_cliente ) {

	void * buffer_response;
	int    buffer_size;
	int    despla = 0;

	int _aux_longitud_acumulada( t_list * p_list ) {

		int total = 0;

		void _aux_longitud_acumulada_iter( void * p_elem ) {

			total += string_length( (char *) p_elem );

		}

		list_iterate( p_list, _aux_longitud_acumulada_iter );

		return total;

	}

	void _aux_mostrar_restaurantes( void * p_elem ) {

		printf( "Palabra: '%s'; Longitud: %d\n", (char*)p_elem, string_length( (char*)p_elem ) );

	}

	void _aux_cargar_buffer( void * p_elem ) {

		int length = string_length((char *) p_elem);

		memcpy( buffer_response + despla, &length, sizeof(uint32_t) );

		despla += sizeof(uint32_t);

		memcpy( buffer_response + despla, p_elem, string_length((char *) p_elem) );

		despla += string_length((char *) p_elem);

	}

	t_list * obtener_restaurante_hardcodeado() {

		t_list * l_restaurantes = list_create();

		list_add( l_restaurantes, "McDonals" );
		list_add( l_restaurantes, "KFC" );
		list_add( l_restaurantes, "Wendy's" );
		list_add( l_restaurantes, "GreenEat" );

		return l_restaurantes;

	}

	t_list * lista_de_restaurante = obtener_restaurante_hardcodeado();

	uint32_t cant_restos = list_size(lista_de_restaurante);

	uint32_t cant_letras = _aux_longitud_acumulada(lista_de_restaurante);

	buffer_size = sizeof(uint32_t)
			+ cant_restos * sizeof(uint32_t)
			+ cant_letras;

	buffer_response = malloc(buffer_size);

	memcpy( buffer_response + despla, &cant_restos, sizeof(uint32_t) );

	despla += sizeof(uint32_t);

	list_iterate( lista_de_restaurante, _aux_cargar_buffer );

	list_iterate( lista_de_restaurante, _aux_mostrar_restaurantes );

	t_header header_response;

	header_response.modulo     = APP;
	header_response.id_proceso = 0;
	header_response.nro_msg    = CONSULTAR_RESTAURANTES;
	header_response.size       = buffer_size;
	header_response.payload    = buffer_response;

	enviar_buffer( socket_cliente, &header_response);

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

char * nro_comando_a_texto(uint32_t p_comando) {

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

	mem_hexdump(stream, stream_size);

	if ( p_conexion != -1 ) {

		int bytes_enviados=send( p_conexion, stream, stream_size, 0);
		log_info(logger,"Se enviaron la cantidad de bytes: %d",bytes_enviados);

		if( -1 == bytes_enviados ) resultado = false;

	} else

		resultado = false;

	free(stream);

	return resultado;

}

t_header * recibir_buffer ( uint32_t socket_cliente ) {

	uint32_t modulo, id_proceso, nro_msg, size;

	recv(socket_cliente, &modulo     , sizeof(uint32_t), 0);
	recv(socket_cliente, &id_proceso , sizeof(uint32_t), 0);
	recv(socket_cliente, &nro_msg    , sizeof(uint32_t), 0);
	recv(socket_cliente, &size       , sizeof(uint32_t), 0);

	t_header * l_header = malloc( sizeof(t_header) );

	l_header->modulo     = modulo;
	l_header->id_proceso = id_proceso;
	l_header->nro_msg    = nro_msg;
	l_header->size       = size;

	if ( size > 0 ) {

		l_header->payload= malloc(size);

		int bytes_recibidos = recv( socket_cliente, l_header->payload, size, 0 );

		mem_hexdump(l_header->payload, size);

		if ( bytes_recibidos < size )

			log_error(logger,"Se recibieron menos bytes de los que se esperaban");

	} else l_header->payload = NULL;

	return l_header;

}

bool enviar_seleccionar_restaurante( char* p_ip, char* p_puerto, int p_id_process, char * p_restaurante ) {

	bool _recibir_confirmacion_seleccionar_restaurante( int p_conexion ) {

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

	int size_resto = string_length( p_restaurante );

	void * l_payload = malloc( size_resto );

	memcpy( l_payload, p_restaurante, size_resto );

	t_header l_header;

	l_header.modulo     = CLIENTE;
	l_header.id_proceso = p_id_process;
	l_header.nro_msg    = SELECCIONAR_RESTAURANTE;
	l_header.size       = size_resto;
	l_header.payload    = l_payload;

	int conexion = crear_socket_y_conectar(p_ip, p_puerto);

	if ( enviar_buffer( conexion, &l_header ) ) {

		bool resultado = _recibir_confirmacion_seleccionar_restaurante(conexion);

		close(conexion);

		return resultado;

	} else {

		close(conexion);

		return NULL;

	}

}

void responder_seleccionar_restaurante( int socket_cliente, bool seleccionado ) {

	t_header header_response;

	header_response.modulo     = APP;
	header_response.id_proceso = 0;
	header_response.nro_msg    = ( seleccionado ) ? SELECCIONAR_RESTAURANTE_OK : SELECCIONAR_RESTAURANTE_FAIL ;
	header_response.size       = 0;
	header_response.payload    = NULL;

	enviar_buffer( socket_cliente, &header_response);

}

t_list * enviar_consultar_platos( char* p_ip, char* p_puerto, int p_id_process ) {

	t_list * _recibir_lista_platos( int p_conexion ) {

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

void responder_consultar_platos( int socket_cliente, char ** p_platos ) {

	void * buffer_response;
	uint32_t buffer_size;
	uint32_t despla = 0;

	uint32_t cantidad_platos = 0;
	uint32_t size_nombres    = 0;

	uint32_t aux = 0;

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

	t_header header_response;

	header_response.modulo     = APP;
	header_response.id_proceso = 0;
	header_response.nro_msg    = CONSULTAR_PLATOS;
	header_response.size       = buffer_size;
	header_response.payload    = buffer_response;

	enviar_buffer( socket_cliente, &header_response);

	free( buffer_response );

}

int enviar_guardar_pedido   (char* p_ip,char* p_puerto){

	uint32_t nro_msg=GUARDAR_PEDIDO;
	t_header * encabezado=serializar_pedido(nro_msg);

	int conexion =crear_socket_y_conectar(p_ip,p_puerto);

	if(enviar_buffer(conexion,encabezado)==FALSE)log_error(logger,"No se pudo enviar el guardado del pedido");

	return conexion;

}

void enviar_obtener_pedido   (char* p_ip,char* p_puerto){

	uint32_t nro_msg=OBTENER_PEDIDO;
	t_header * encabezado=serializar_pedido(nro_msg);

	int conexion =crear_socket_y_conectar(p_ip,p_puerto);

	if(enviar_buffer(conexion,encabezado)==FALSE)log_error(logger,"No se pudo enviar el guardado del pedido");
}

void enviar_confirmar_pedido   (char* p_ip,char* p_puerto){

	uint32_t nro_msg=CONFIRMAR_PEDIDO;
	t_header * encabezado=serializar_pedido(nro_msg);

	int conexion =crear_socket_y_conectar(p_ip,p_puerto);

	if(enviar_buffer(conexion,encabezado)==FALSE)log_error(logger,"No se pudo enviar el guardado del pedido");

}

void enviar_finalizar_pedido   (char* p_ip,char* p_puerto){

	uint32_t nro_msg=FINALIZAR_PEDIDO;
	t_header * encabezado=serializar_pedido(nro_msg);

	int conexion =crear_socket_y_conectar(p_ip,p_puerto);

	if(enviar_buffer(conexion,encabezado)==FALSE)log_error(logger,"No se pudo enviar el guardado del pedido");

}

t_header * serializar_pedido       (uint32_t nro_msg        ){

	t_pedido * pedido=malloc(sizeof(t_pedido));
	t_header * encabezado=malloc(sizeof(t_header));
	int offset=0;

	pedido->id_pedido=1;
	pedido->size_nombre_restaurante=6;
	pedido->nombre_restaurante=malloc(pedido->size_nombre_restaurante);
	pedido->nombre_restaurante="Farola";

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
	log_info(logger,"Se recibio el size del nombre %d",pedido->size_nombre_restaurante);
	log_info(logger,"Se recibio el restaurante %s",pedido->nombre_restaurante);

	return pedido;

}

void 	   enviar_guardar_plato    (char* p_ip,char* p_puerto){

	t_guardar_plato * plato=malloc(sizeof(t_guardar_plato));
	plato->pedido=malloc(sizeof(t_pedido));
	t_header * encabezado=malloc(sizeof(t_header));
	int offset=0;

	plato->pedido->id_pedido=1;
	plato->pedido->size_nombre_restaurante=9;
	plato->pedido->nombre_restaurante=malloc(plato->pedido->size_nombre_restaurante);
	plato->pedido->nombre_restaurante="Mataderos";
	plato->size_nombre_plato=4;
	plato->nombre_plato="papa";
	plato->cantidad_plato=5;

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

}

t_guardar_plato * recibir_guardar_plato   (void * payload         ){

	t_guardar_plato * plato=malloc(sizeof(t_guardar_plato));
	plato->pedido=malloc(sizeof(t_pedido));

	plato->pedido=recibir_pedido(payload);

	int size_pedido=2*sizeof(uint32_t)+plato->pedido->size_nombre_restaurante;
	payload+=size_pedido;

	memcpy(&(plato->size_nombre_plato),payload,sizeof(uint32_t));
	payload+=sizeof(uint32_t);

	plato->nombre_plato=malloc(plato->size_nombre_plato+1);
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

void 	   enviar_plato_listo    (char* p_ip,char* p_puerto){

	t_plato_listo * plato=malloc(sizeof(t_plato_listo));
	plato->pedido=malloc(sizeof(t_pedido));
	t_header * encabezado=malloc(sizeof(t_header));
	int offset=0;

	plato->pedido->id_pedido=15;
	plato->pedido->size_nombre_restaurante=7;
	plato->pedido->nombre_restaurante=malloc(plato->pedido->size_nombre_restaurante);
	plato->pedido->nombre_restaurante="Tinajas";
	plato->size_nombre_plato=6;
	plato->nombre_plato="batata";

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
