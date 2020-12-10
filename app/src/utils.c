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
	int cantidad_platos=0;

	memcpy(&info_resto->resto_x,payload+offset,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(&info_resto->resto_y,payload+offset,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(&size_nombre,payload+offset,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	info_resto->resto_nombre=malloc(size_nombre);
	memcpy(info_resto->resto_nombre,payload+offset,size_nombre);
	offset+=size_nombre;

	memcpy(&cantidad_platos,payload+offset,sizeof(uint32_t));
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




