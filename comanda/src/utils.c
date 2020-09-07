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



	int * sock_cliente=(int*)socket_cliente;
	uint32_t size_buffer=4*sizeof(uint32_t);
	void * buffer=malloc(sizeof(size_buffer));

	t_header * mensaje_recibido;


	int byte_recibidos=recv(*sock_cliente,buffer,size_buffer,MSG_WAITALL);
	if(byte_recibidos<size_buffer)log_error(logger,"Se recibieron menos bytes de los que se esperaban");

	deserializar(buffer,"%z%z%z%z",&mensaje_recibido->modulo ,&mensaje_recibido->id_proceso,&mensaje_recibido->nro_msg,&mensaje_recibido->size);

	switch(mensaje_recibido->nro_msg){
		case GUARDAR_PEDIDO:
			administrar_guardar_pedido();
			break;
		case GUARDAR_PLATO:
			administrar_guardar_plato();
			break;
		case OBTENER_PEDIDO:
			administrar_obtener_pedido();
			break;
		case CONFIRMAR_PEDIDO:
			administrar_confirmar_pedido();
			break;
		case PLATO_LISTO:
			administrar_plato_listo();
			break;
		case FINALIZAR_PEDIDO:
			administrar_finalizar_pedido();
			break;
		default:
			log_error(logger,"La Comanda no soporta el mensaje solicitado");



	}







}

void administrar_guardar_pedido(){

}

void administrar_guardar_plato(){

}

void administrar_obtener_pedido(){

}

void administrar_confirmar_pedido(){

}

void administrar_plato_listo(){

}

void administrar_finalizar_pedido(){

}

