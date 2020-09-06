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


	uint32_t nro_modulo         ;
	uint32_t nro_identificatorio;
	uint32_t tipo_mensaje       ;
	uint32_t size_payload       ;
	int * sock_cliente=(int*)socket_cliente;
	uint32_t size_buffer=4*sizeof(uint32_t);
	void * buffer=malloc(sizeof(size_buffer));


	int byte_recibidos=recv(*sock_cliente,buffer,size_buffer,MSG_WAITALL);
	if(byte_recibidos<size_buffer)log_error(logger,"Se recibieron menos bytes de los que se esperaban");

	deserializar(buffer,"%z%z%z%z",&nro_modulo ,&nro_identificatorio,&tipo_mensaje,&size_payload);







}
