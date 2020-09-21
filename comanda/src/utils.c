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

	lista_restarurantes=list_create();

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

	log_info(logger,"Se recibio mensaje del modulo: %d",mensaje_recibido->modulo);
	log_info(logger,"Se recibio mensaje del modulo con id: %d",mensaje_recibido->id_proceso);
	log_info(logger,"Se recibio el tipo de mensaje numero: %d",mensaje_recibido->nro_msg);
	log_info(logger,"Se recibio un payload del tamaño: %d",mensaje_recibido->size);


	switch(mensaje_recibido->nro_msg){
		case GUARDAR_PEDIDO:
			aux=mensaje_recibido->payload;
			mensaje_recibido->payload=(void*)recibir_consulta_pedido(mensaje_recibido->payload);
			administrar_guardar_pedido(mensaje_recibido,*sock_cliente);
			free(aux);
			break;
		case GUARDAR_PLATO:
			aux=mensaje_recibido->payload;
			mensaje_recibido->payload=(void*)recibir_guardar_plato(mensaje_recibido->payload);
			administrar_guardar_plato(mensaje_recibido,*sock_cliente);
			free(aux);
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
			free(mensaje_recibido->payload);
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



void administrar_guardar_pedido(t_header * encabezado,int socket_cliente){

	t_pedido * pedido=(t_pedido*)encabezado->payload;

		bool buscar_restaurante(void * elemento){
			t_restaurante * restaurante=(t_restaurante*)elemento;

			if(string_equals_ignore_case(restaurante->nombre_restaurante,pedido->nombre_restaurante)){
					return TRUE;
					}
			return FALSE;
		}
	t_restaurante * restaurante=list_find(lista_restarurantes,buscar_restaurante);   //Busca si existe el restaurante

	if(restaurante==NULL)
		restaurante=crear_tabla_segmentos_restaurante(pedido->nombre_restaurante);   //Si no existe el restaurant, lo crea

	bool exito=agregar_pedido_a_tabla_segmentos(restaurante,pedido->id_pedido);  //Crea un nuevo segmento con el ID del pedido. EN caso de q exista, devuelve FALSE

	t_header * nuevo_encabezado=malloc(sizeof(t_header));

	nuevo_encabezado->id_proceso=100;
	nuevo_encabezado->modulo=COMANDA;
	if(exito==TRUE)
		nuevo_encabezado->nro_msg=OK;
	else
		nuevo_encabezado->nro_msg=FAIL;

	nuevo_encabezado->size=0;
	nuevo_encabezado->payload=NULL;

	bool exito_envio=enviar_buffer(socket_cliente,nuevo_encabezado);

	if(exito_envio==FALSE)log_error(logger,"No se envio correctamente la respuesta al modulo");

	free(nuevo_encabezado);

}






void  administrar_guardar_plato(t_header * encabezado,int socket_cliente){

	t_guardar_plato * plato=(t_guardar_plato*)encabezado->payload;

			bool buscar_restaurante(void * elemento){
				t_restaurante * restaurante=(t_restaurante*)elemento;

				if(string_equals_ignore_case(restaurante->nombre_restaurante,plato->pedido->nombre_restaurante)){
						return TRUE;
						}
				return FALSE;
			}

			bool buscar_pedido(void * elemento){
				t_pedido_seg * pedido=(t_pedido_seg*)elemento;
				printf("El numero de pedido que estoy iterando es: %d\n",pedido->id_pedido);

				if(pedido->id_pedido==plato->pedido->id_pedido){
						return TRUE;
						}
				return FALSE;
			}

			bool buscar_comida(void * elemento){
				t_pagina_comida * adm_comida=(t_pagina_comida*)elemento;
				t_comida * comida=(t_comida*)adm_comida->contenido;

				if(string_equals_ignore_case(comida->nombre_comida,plato->nombre_plato)){
						return TRUE;
						}
				return FALSE;
			}

	bool exito=FALSE;

	if(list_is_empty(lista_restarurantes))								//Verifico que no este vacia la lista
		goto envio_de_respuesta;

	t_restaurante * restaurante=NULL;
	restaurante=list_find(lista_restarurantes,buscar_restaurante);


	if(restaurante==NULL)
		printf("No se encontro el restaurante\n");   //Se informa que no existe el restaurante
	else
		printf("Se encontro el restaurant: %s \n",restaurante->nombre_restaurante);

	if(list_is_empty(restaurante->tabla_pedidos))						//Verifico que no este vacia la lista
		goto envio_de_respuesta;

	t_pedido_seg * pedido=NULL;
	pedido=list_find(restaurante->tabla_pedidos,buscar_pedido);

	if(pedido==NULL)
		printf("No se encontro el pedido\n");//Se informa que no existe el pedido
	else
		printf("Se encontro el pedido numero: %d\n",pedido->id_pedido);

	t_pagina_comida * comida=NULL;

	if(!list_is_empty(pedido->comidas_del_pedido))						//Verifico que no este vacia la lista
			comida=list_find(pedido->comidas_del_pedido,buscar_comida);

	if(comida!=NULL)
		printf("Se encontro el plato de comida en el pedido\n");//El caso de que ya exista ese plato en el pedido
	else{
		printf("No se encontro el plato de comida en el pedido, asique se va a crear\n");
		t_pagina_comida * adm_comida=malloc(sizeof(t_pagina_comida));
		adm_comida->esta_en_memoria=FALSE;
		adm_comida->direccion_memoria=NULL;

		t_comida * comida=malloc(sizeof(t_comida));
		comida->cantidad_lista_comida=0;
		comida->cantidad_total_comida=plato->cantidad_plato;
		strcpy(comida->nombre_comida,plato->nombre_plato);  //----------------TENGO Q VER ACA COMO PASO EL STRING AL VECTOR

		/*int indice=0;
		for(int i=0;i>23;i++){
			comida->nombre_comida[i]=*(plato->nombre_plato+indice);
			if(++indice==plato->size_nombre_plato)
				i=23;
		}*/
		printf("El nombre de la comida es: %s",comida->nombre_comida);

		adm_comida->contenido=(void*)comida;

		list_add(pedido->comidas_del_pedido,adm_comida);            //Guardo la nueva comida en el pedido

		exito=TRUE;
	}

	envio_de_respuesta:
	;
	t_header * nuevo_encabezado=malloc(sizeof(t_header));

	nuevo_encabezado->id_proceso=100;
	nuevo_encabezado->modulo=COMANDA;
	if(exito==TRUE)
		nuevo_encabezado->nro_msg=OK;
	else
		nuevo_encabezado->nro_msg=FAIL;

	nuevo_encabezado->size=0;
	nuevo_encabezado->payload=NULL;

	bool exito_envio=enviar_buffer(socket_cliente,nuevo_encabezado);

	if(exito_envio==FALSE)log_error(logger,"No se envio correctamente la respuesta al modulo");

	free(nuevo_encabezado);

}

t_plato_listo * administrar_plato_listo(void * payload){

	t_plato_listo * plato=recibir_plato_listo(payload);

	return plato;
}

t_pedido * recibir_consulta_pedido(void * payload){

	t_pedido * pedido=recibir_pedido(payload);

	return pedido;
}

