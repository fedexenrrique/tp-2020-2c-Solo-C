#include "serializar.h"

void prueba_biblioteca_compartida(void) {
	printf("\n!!!Texto impreso desde la biblioteca compartida!!!\n");
}

int enviar_consultar_restaurante(char* pokemon, int pos_x, int pos_y) {

	int    stream_size = sizeof( uint32_t ) * 3;
	void * stream      = malloc( stream_size );
	int    offset      = 0;

    memset( &stream         , 0                      , stream_size        );

	memset( &stream + offset, CLIENTE                , sizeof( uint32_t ) );

	offset += sizeof( uint32_t );

	memset( &stream + offset, sizeof( uint32_t )     , sizeof( uint32_t ) );

	offset += sizeof( uint32_t );

	memset( &stream + offset, CONSULTAR_RESTAURANTES , sizeof( uint32_t ) );

	offset += sizeof( uint32_t );

	int conexion = crear_conexion(g_broker_ip, g_broker_puerto);
	bool resultado = true;

	if ( conexion != -1 ) {

		if( -1 == send(conexion, stream_paquete, size_paquete, 0) ) resultado = false;

	} else

		resultado = false;

	free(stream);

	if ( resultado )

		return recibir_confirmacion(conexion);

	else {

		close(conexion);

		return -1;

	}

}

int crear_conexion(char*ip, char* puerto) {

	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family   = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags    = AI_PASSIVE;



	getaddrinfo(ip, puerto, &hints, &server_info);

	int socket_cliente   = socket (server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
	int validar_conexion = connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen);

	if( validar_conexion== -1|| socket_cliente==-1)
		log_info(logger,"error");
	else
		log_info(logger,"Se conecto correctamente");

	freeaddrinfo(server_info);

	return socket_cliente;

}

int recibir_confirmacion(int socket_cliente) {

	return 000;

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
