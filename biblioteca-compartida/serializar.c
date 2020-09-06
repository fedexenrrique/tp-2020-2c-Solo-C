#include "serializar.h"

void prueba_biblioteca_compartida(void) {
	printf("\n!!!Texto impreso desde la biblioteca compartida!!!\n");
}

t_list * enviar_consultar_restaurante(char* p_ip, int p_puerto) {

	t_list * _recibir_lista_restaurantes( int p_conexion ) {

		printf("Por recibir lista de restaurantes.");

		t_header * header_restaurantes = recibir_buffer( p_conexion );

		t_list   * lista_restaurantes;

		if ( header_restaurantes->size > 0 ) {

			lista_restaurantes = list_create();
			int despla = 0;

			for ( int i = 0; i < 4; i++ ) {

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

	int conexion = crear_socket_y_conectar(p_ip, p_puerto);

	if ( enviar_buffer( conexion, &l_header ) )

		return _recibir_lista_restaurantes(conexion);

	else {

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

		char * elem = (char *) p_elem;
		printf( "Palabra: '%s'; Longitud: %d\n", elem, string_length(elem) );

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

	t_header * header_recibido = recibir_buffer( socket_cliente );

	printf( "Módulo:       %d.\n" , header_recibido->modulo     );
	printf( "ID Proceso:   %d.\n" , header_recibido->id_proceso );
	printf( "Nro. mensaje: %d.\n" , header_recibido->nro_msg    );
	printf( "Bytes:        %d.\n" , header_recibido->size       );

	t_list * lista_de_restaurante = obtener_restaurante_hardcodeado();

	buffer_size = list_size(lista_de_restaurante) * sizeof(uint32_t) + _aux_longitud_acumulada(lista_de_restaurante);

	buffer_response = malloc(buffer_size);

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

int crear_socket_y_conectar(char* p_ip, int p_puerto) {

	char * puerto = string_itoa(p_puerto);

	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family   = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags    = AI_PASSIVE;

	getaddrinfo(p_ip, puerto, &hints, &server_info);

	int socket_cliente   = socket (server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
	int validar_conexion = connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen);

	if( validar_conexion== -1|| socket_cliente==-1)
		log_info(logger,"error");
	else
		log_info(logger,"Se conecto correctamente");

	freeaddrinfo(server_info);

	free(puerto);

	return socket_cliente;

}

int recibir_confirmacion(int socket_cliente) {

	return 000;

}

int crear_socket_escucha( char * p_ip, char * p_puerto ) {

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

int aceptar_conexion ( int p_socket_para_escuchar ) {

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



int detectar_comando(char * p_comando) {

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

bool enviar_buffer ( int p_conexion, t_header * p_header ) {

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

	if ( p_conexion != -1 ) {

		if( -1 == send( p_conexion, stream, stream_size, 0) ) resultado = false;

	} else

		resultado = false;

	free(stream);

	return resultado;

}

t_header * recibir_buffer ( int socket_cliente ) {

	uint32_t modulo, id_proceso, nro_msg, size;

	recv(socket_cliente, &modulo     , sizeof(uint32_t), MSG_WAITALL);
	recv(socket_cliente, &id_proceso , sizeof(uint32_t), MSG_WAITALL);
	recv(socket_cliente, &nro_msg    , sizeof(uint32_t), MSG_WAITALL);
	recv(socket_cliente, &size       , sizeof(uint32_t), MSG_WAITALL);

	t_header * l_header = malloc( sizeof(t_header) );

	l_header->modulo     = modulo;
	l_header->id_proceso = id_proceso;
	l_header->nro_msg    = nro_msg;
	l_header->size       = size;

	if ( size > 0 ) {

		l_header->payload= malloc(size);

		recv( socket_cliente, l_header->payload, size, MSG_WAITALL );

	} else l_header->payload = NULL;

	return l_header;

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
