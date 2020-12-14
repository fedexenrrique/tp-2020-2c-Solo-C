/*
 * utils.c
 *
 *  Created on: 8 dic. 2020
 *      Author: utnso
 */


#include "utils.h"


t_info_restaurante * deserializar_info_resto(void * payload,uint32_t size){

	t_info_restaurante * info_resto=malloc(sizeof(t_info_restaurante));
	int offset=0;
	int size_nombre=0;
//	int cantidad_platos=0;

	memcpy(&info_resto->resto_x,payload+offset,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(&info_resto->resto_y,payload+offset,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(&size_nombre,payload+offset,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	info_resto->resto_nombre=malloc(size_nombre);
	memcpy(info_resto->resto_nombre,payload+offset,size_nombre);
	offset+=size_nombre;

/*	memcpy(&cantidad_platos,payload+offset,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	for(int i=0;i<cantidad_platos;i++){

		int size_nombre_plato=0;
		char * nombre_plato=NULL;

		memcpy(&size_nombre_plato,payload+offset,sizeof(uint32_t));
		offset+=sizeof(uint32_t);

		nombre_plato=malloc(size_nombre_plato);
		memcpy(nombre_plato,payload+offset,size_nombre_plato);
		offset+=size_nombre_plato;

		info_resto->list_platos[i]=nombre_plato;

	}
*/
	return info_resto;

}

bool  enviar_confirmar_pedido_a_resto(t_info_restaurante * info_resto,uint32_t id_pedido){

	uint32_t nro_msg=CONFIRMAR_PEDIDO;
	char * nombre_restaurante=NULL;


	//Le mando el nombre en null, para reutilizar la funcion
	t_header * encabezado=serializar_pedido(nro_msg, nombre_restaurante,id_pedido);

	//int conexion =crear_socket_y_conectar(p_ip,p_puerto);

	if(enviar_buffer(info_resto->socket_conectado,encabezado)==FALSE){
		log_error(logger,"No se pudo enviar el guardado del pedido");
	    return FALSE;}

	encabezado=recibir_buffer(info_resto->socket_conectado);

	log_info(logger,"Se recibio mensaje del modulo numero: %d",encabezado->id_proceso);
	log_info(logger,"Se recibio el mensaje: %s",nro_comando_a_texto(encabezado->nro_msg));
	if(encabezado->nro_msg==OK)return TRUE;
	return FALSE;


}


t_info_restaurante * buscar_info_de_restaurante(char * nombre_resto,t_list * lista_resto_conectados){

	bool  buscar_resto_conectado(void * elemento){

		t_info_restaurante * info_resto=(t_info_restaurante *)elemento;

		if(string_equals_ignore_case(info_resto->resto_nombre,nombre_resto))
			return TRUE;
		return FALSE;

	}

	t_info_restaurante * info_resto=list_find(lista_resto_conectados,buscar_resto_conectado);

	return info_resto;

}

uint32_t solicitar_id_a_restaurante(uint32_t socket){

	t_header header_response;
	uint32_t id_pedido=0;

	header_response.modulo     = APP;
	header_response.id_proceso = 0;
	header_response.nro_msg    = CREAR_PEDIDO;
	header_response.size       = 0;
	header_response.payload    = NULL;

	enviar_buffer( socket, &header_response);

	t_header * encabezado=recibir_buffer(socket);

	if(encabezado->size!=sizeof(uint32_t)){log_error(logger,"No se envio la cantidad de bytes correspondientes a un ID");return -1;}

	memcpy(&id_pedido,encabezado->payload,sizeof(uint32_t));

	return id_pedido;

}

void procesar_consultar_pedido(uint32_t id_pedido,t_header * header_recibido){

		bool _control_existe_asociacion_cliente_resto ( void * p_elem ) {

			return ( ((t_cliente_a_resto*)p_elem)->id_cliente == header_recibido->id_proceso ) ? true : false ;

		}

	t_cliente_a_resto * asociacion = list_find( lista_clientes , _control_existe_asociacion_cliente_resto );

	if ( asociacion != NULL ) {

		printf("Se encontró asociación.");

//	if(string_equals_ignore_case(asociacion->nombre_resto,"default"))//Verifico si es default y envio los platos default
//		return g_platos_default;

	int conexion=enviar_obtener_pedido(g_ip_comanda,g_puerto_comanda,asociacion->nombre_resto,id_pedido);

	header_recibido=recibir_buffer(conexion);
	log_info(logger,"Se recibio el mensaje: %s",nro_comando_a_texto(header_recibido->nro_msg));

	t_info_restaurante * info_resto= buscar_info_de_restaurante(asociacion->nombre_resto,lista_resto_conectados);

	recibir_desde_app_respuesta_obtener_pedido_y_responder(header_recibido,info_resto);



	} else {

	printf("No se encontró la asociación.");

	}
}

void recibir_desde_app_respuesta_obtener_pedido_y_responder(t_header * header_recibido,t_info_restaurante * info_resto){//Se recibe respuesta de comanda y se envia a cliente

	int offset=0;

	uint32_t size_nombre_resto=string_length(info_resto->resto_nombre);
	uint32_t size_payload_nuevo=sizeof(uint32_t)+size_nombre_resto+header_recibido->size;
	void * payload_nuevo=malloc(size_payload_nuevo);

	memcpy(payload_nuevo+offset,&size_nombre_resto,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(payload_nuevo+offset,info_resto->resto_nombre,size_nombre_resto);
	offset+=size_nombre_resto;

	memcpy(payload_nuevo+offset,header_recibido->payload,header_recibido->size);


	t_header header_response;

	header_response.modulo     = APP;
	header_response.id_proceso = 0;
	header_response.nro_msg    = CONSULTAR_PEDIDO;
	header_response.size       = size_payload_nuevo;
	header_response.payload    = payload_nuevo;

	bool envio=enviar_buffer(info_resto->socket_conectado , &header_response);

	if(envio==TRUE)log_info(logger,"Se envio correctamente la respuesta de consultar pedido al cliente");
	else log_info(logger,"No se envio correctamente la respuesta de consultar pedido al cliente");



}
