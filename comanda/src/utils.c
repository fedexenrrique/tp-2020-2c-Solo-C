/*
 * utils.c
 *
 *  Created on: 5 sep. 2020
 *      Author: utnso
 */


#include "utils.h"


int iniciar_comanda(){

	int    socket_comanda;
	char * ip_comanda;
	char * puerto_comanda;

	if( ( config = leer_config() ) == NULL ) {
		perror("config");
		log_error(logger,"ERROR al leer la config");
	}

	logger = iniciar_logger();


	log_info(logger,"LEER CONFIG: IP, SOCKET");

	ip_comanda     = config_get_string_value(config, "IP_COMANDA"     );
	puerto_comanda = config_get_string_value(config, "PUERTO_ESCUCHA" );

	log_info(logger,"IP     BROKER LEIDO: %s",ip_comanda);
	log_info(logger,"SOCKET BROKER LEIDO: %s",puerto_comanda);

	socket_comanda=crear_socket_escucha(ip_comanda,puerto_comanda);

	log_info(logger,"el socket escucha es: %d",socket_comanda);

	return socket_comanda;


}

t_log * iniciar_logger(){

	return log_create("comanda.log","Comanda",1,LOG_LEVEL_INFO);
}

t_config* leer_config(){
	return config_create("comanda.config");

}




void manejo_modulo_conectado(void * socket_cliente){


	void * aux=NULL;
	int * sock_cliente=(int*)socket_cliente;
	//uint32_t size_buffer=4*sizeof(uint32_t);
	//void * buffer=malloc(sizeof(size_buffer));

	t_header * mensaje_recibido=recibir_buffer (*sock_cliente);


	switch(mensaje_recibido->nro_msg){
		case GUARDAR_PEDIDO:
			aux=mensaje_recibido->payload;
			recibir_consulta_pedido(mensaje_recibido->payload);
			free(aux);
			break;
		case GUARDAR_PLATO:
			administrar_guardar_plato(mensaje_recibido->payload);
			break;
		case OBTENER_PEDIDO:
			aux=mensaje_recibido->payload;
			recibir_consulta_pedido(mensaje_recibido->payload);
			free(aux);
			break;
		case CONFIRMAR_PEDIDO:
			aux=mensaje_recibido->payload;
			recibir_consulta_pedido(mensaje_recibido->payload);
			free(aux);
			break;
		case PLATO_LISTO:
			administrar_plato_listo(mensaje_recibido->payload);
			break;
		case FINALIZAR_PEDIDO:
			aux=mensaje_recibido->payload;
			recibir_consulta_pedido(mensaje_recibido->payload);
			free(aux);
			break;
		default:
			log_error(logger,"La Comanda no soporta el mensaje solicitado");



	}







}


t_guardar_plato * administrar_guardar_plato(void * payload){

	t_guardar_plato * plato=malloc(sizeof(t_guardar_plato));
	plato->pedido=malloc(sizeof(t_pedido));
	void * stream=payload;

	plato->pedido=recibir_consulta_pedido(payload);

	int size_pedido=2*sizeof(uint32_t)+plato->pedido->size_nombre;
	stream+=size_pedido;

	memcpy(&(plato->size_nombre_plato),payload,sizeof(uint32_t));
	stream+=sizeof(uint32_t);

	plato->nombre_plato=malloc(plato->size_nombre_plato);
	memcpy(plato->nombre_plato,payload,sizeof(uint32_t));
	stream+=plato->size_nombre_plato;

	memcpy(&(plato->cantidad_plato),payload,sizeof(uint32_t));
	stream+=sizeof(uint32_t);

	free(payload);
	return plato;

}

t_plato_listo * administrar_plato_listo(void * payload){

	t_plato_listo * plato=malloc(sizeof(t_plato_listo));
	plato->pedido=malloc(sizeof(t_pedido));
	void * stream=payload;

	plato->pedido=recibir_consulta_pedido(payload);

	int size_pedido=2*sizeof(uint32_t)+plato->pedido->size_nombre;
	stream+=size_pedido;

	memcpy(&(plato->size_nombre_plato),payload,sizeof(uint32_t));
	stream+=sizeof(uint32_t);

	plato->nombre_plato=malloc(plato->size_nombre_plato);
	memcpy(plato->nombre_plato,payload,sizeof(uint32_t));

	free(payload);
	return plato;

}

t_pedido * recibir_consulta_pedido(void * payload){

	t_pedido * pedido=malloc(sizeof(t_pedido));
	void * stream=payload;

	memcpy(&(pedido->size_nombre),payload,sizeof(uint32_t));
	stream+=sizeof(uint32_t);

	pedido->nombre_restaurante=malloc(pedido->size_nombre);
	memcpy(pedido->nombre_restaurante,payload,sizeof(uint32_t));
	stream+=pedido->size_nombre;

	memcpy(&(pedido->id_pedido),payload,sizeof(uint32_t));

	return pedido;

}

