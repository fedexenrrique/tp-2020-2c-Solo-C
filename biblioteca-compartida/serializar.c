#include "serializar.h"

void prueba_biblioteca_compartida(void) {
	printf("\n!!!Texto impreso desde la biblioteca compartida!!!\n");
}

int enviar_consultar_restaurante(char* p_ip, int p_puerto) {

	int    stream_size = sizeof( uint32_t ) * 4;
	void * stream      = malloc( stream_size );
	int    offset      = 0;

	int    valor;

    memset( stream         , 0                            , stream_size        );

    valor = CLIENTE; // 01 - MODULO
	memcpy( stream + offset, &valor                       , sizeof( uint32_t ) );

	offset += sizeof( uint32_t );

    valor = 0; // 02 - ID PROCESO
	memcpy( stream + offset, &valor                       , sizeof( uint32_t ) );

	offset += sizeof( uint32_t );

	valor = CONSULTAR_RESTAURANTES; // 03 - TIPO MENSAJE
	memcpy( stream + offset, &valor                       , sizeof( uint32_t ) );

	offset += sizeof( uint32_t );

	valor = 0; // 04 - TAMAÑO DE PAYLOAD
	memcpy( stream + offset, &valor                       , sizeof( uint32_t ) );

	offset += sizeof( uint32_t );

	int conexion = crear_socket_y_conectar(p_ip, p_puerto);

	bool resultado = true;

	if ( conexion != -1 ) {

		if( -1 == send(conexion, stream, stream_size, 0) ) resultado = false;

	} else

		resultado = false;

	mem_hexdump(stream, 3*sizeof(uint32_t) );

	free(stream);

	mem_hexdump(stream, 3*sizeof(uint32_t) );

	if ( resultado )

		return recibir_confirmacion(conexion);

	else {

		close(conexion);

		return -1;

	}

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

int crear_socket_escucha( char * p_ip, int p_puerto ) {

	char * puerto = string_itoa( p_puerto );

	int socket_servidor;

	struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags    = AI_PASSIVE;

    getaddrinfo( p_ip, puerto, &hints, &servinfo);

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

	free(puerto);

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
