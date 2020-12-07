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

	iniciar_memoria();


	log_info(logger,"LEER CONFIG: IP, SOCKET");

	ip_comanda     = config_get_string_value(config, "IP_COMANDA"     );
	puerto_comanda = config_get_string_value(config, "PUERTO_ESCUCHA" );
//	id_modulo=config_get_int_value(config, "ID_MODULO" );

	log_info(logger,"IP     BROKER LEIDO: %s",ip_comanda);
	log_info(logger,"SOCKET BROKER LEIDO: %s",puerto_comanda);
//	log_info(logger,"EL Modulo tiene el id N°: %d",id_modulo);

	socket_comanda=crear_socket_escucha(ip_comanda,puerto_comanda);

	log_info(logger,"el socket escucha es: %d",socket_comanda);
	printf("\n");
	printf("\n");
	printf("\n");

	lista_restarurantes=list_create();

	return socket_comanda;


}

t_log * iniciar_logger(){

	char * archivo_log=config_get_string_value(config, "ARCHIVO_LOG"     );

	return log_create(archivo_log,"Comanda",1,LOG_LEVEL_INFO);
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

	log_info(logger,"Se recibio mensaje del modulo: %s",nro_modulo_a_texto(mensaje_recibido->modulo));
	log_info(logger,"Se recibio mensaje del modulo con id: %d",mensaje_recibido->id_proceso);
	log_info(logger,"Se recibio el tipo de mensaje numero: %s",nro_comando_a_texto(mensaje_recibido->nro_msg));
//	log_info(logger,"Se recibio un payload del tamaño: %d",mensaje_recibido->size);


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
			mensaje_recibido->payload=(void*)recibir_consulta_pedido(mensaje_recibido->payload);
			administrar_obtener_pedido(mensaje_recibido,*sock_cliente);
			free(aux);
			break;
		case CONFIRMAR_PEDIDO:
			aux=mensaje_recibido->payload;
			mensaje_recibido->payload=(void*)recibir_consulta_pedido(mensaje_recibido->payload);
			administrar_confirmar_pedido(mensaje_recibido,*sock_cliente);
			free(aux);
			break;
		case PLATO_LISTO:
			aux=mensaje_recibido->payload;
			mensaje_recibido->payload=(void*)recibir_plato_listo(mensaje_recibido->payload);
			administrar_plato_listo(mensaje_recibido,*sock_cliente);
			free(aux);
			break;
		case FINALIZAR_PEDIDO:
			aux=mensaje_recibido->payload;
			mensaje_recibido->payload=(void*)recibir_consulta_pedido(mensaje_recibido->payload);
			administrar_finalizar_pedido(mensaje_recibido,*sock_cliente);
			free(aux);
			break;
		default:
			log_error(logger,"La Comanda no soporta el mensaje solicitado");



	}

}



void administrar_guardar_pedido(t_header * encabezado,int socket_cliente){

	t_pedido * pedido=(t_pedido*)encabezado->payload;

	t_restaurante * restaurante=NULL;
	restaurante=busqueda_de_restaurante(pedido->nombre_restaurante);

	if(restaurante==NULL)
		restaurante=crear_tabla_segmentos_restaurante(pedido->nombre_restaurante);   //Si no existe el restaurant, lo crea

	bool exito=agregar_pedido_a_tabla_segmentos(restaurante,pedido->id_pedido);  //Crea un nuevo segmento con el ID del pedido. EN caso de q exista, devuelve FALSE

	cod_msg tipo_msj;
	if(exito==TRUE)
		tipo_msj=OK;
	else
		tipo_msj=FAIL;

	armar_y_enviar_respuesta(tipo_msj,socket_cliente);

	printf("\n");
	printf("\n");
	printf("\n");

}



void  administrar_guardar_plato(t_header * encabezado,int socket_cliente){

	t_guardar_plato * plato=(t_guardar_plato*)encabezado->payload;
	bool exito=FALSE;

					bool buscar_comida(void * elemento){
						t_pagina_comida * adm_comida=(t_pagina_comida*)elemento;
						t_comida * comida=(t_comida*)adm_comida->frame_swap->direccion_frame;

//						log_info(logger,"Estoy comprobando el nombre %s",comida->nombre_comida);
//						log_info(logger,"con el nombre %s",plato->nombre_plato);

						if(string_equals_ignore_case(comida->nombre_comida,plato->nombre_plato)){
								return TRUE;
								}
						return FALSE;
					}

	if(list_is_empty(lista_restarurantes))								//Verifico que no este vacia la lista
		goto envio_de_respuesta;

	t_restaurante * restaurante=NULL;
	restaurante=busqueda_de_restaurante(plato->pedido->nombre_restaurante);

	if(restaurante==NULL){
		log_info(logger,"No se encontro el restaurante\n");   //Se informa que no existe el restaurante
		goto envio_de_respuesta;}
	else
		log_info(logger,"Se encontro el restaurant: %s \n",restaurante->nombre_restaurante);

	if(list_is_empty(restaurante->tabla_pedidos))						//Verifico que no este vacia la lista
		goto envio_de_respuesta;

	t_pedido_seg * pedido=NULL;
	pedido=buscar_el_pedido(plato->pedido,restaurante->tabla_pedidos);

	if(pedido==NULL){
		log_info(logger,"No se encontro el pedido\n");//Se informa que no existe el pedido
		goto envio_de_respuesta;}
	else
		log_info(logger, "Se encontro el pedido numero: %d\n",pedido->id_pedido);

	if(pedido->estado!=PENDIENTE){														//Si el pedido ya esta cofirmado o terminado, no se pueden agregar platos.
		log_info(logger,"El pedido se encuentra en estado %s Por eso no se puede guardar el plato \n",nro_estado_pedido_a_texto(pedido->estado));
		goto envio_de_respuesta;
	}

	t_pagina_comida * adm_comida=NULL;

	if(!list_is_empty(pedido->comidas_del_pedido))						//Verifico que no este vacia la lista de platos del pedido
			adm_comida=list_find(pedido->comidas_del_pedido,buscar_comida);

	if(adm_comida!=NULL){																				//El caso de que ya exista ese plato en el pedido
			log_info(logger,"Se encontro el plato de comida en el pedido. Se va a sumar la nueva cantidad.\n");

			if(adm_comida->esta_en_memoria_principal!=TRUE){

				log_info(logger,"El plato que se quiere modificar, no se encuentra en Memoria Principal. Hay que cargarlo.");
				adm_comida=cargar_pagina_a_memoria_principal(adm_comida);			//Busca frame libre y copia lo q esta en SWAP en principal
			}
			exito=sumar_cantidad_total_plato(adm_comida,plato);
		}
	else{																								//No existe el plato todavia
			log_info(logger,"No se encontro el plato de comida en el pedido, asique se va a crear\n");

			exito=crear_nuevo_plato(plato,pedido);
	    }

	envio_de_respuesta:
	;

	cod_msg tipo_msj;
	if(exito==TRUE)
		tipo_msj=OK;
	else
		tipo_msj=FAIL;

	armar_y_enviar_respuesta(tipo_msj,socket_cliente);
	printf("\n");
	printf("\n");
	printf("\n");
}

void administrar_obtener_pedido(t_header * encabezado,int socket_cliente){

	t_pedido * pedido_solicitado=(t_pedido*)encabezado->payload;
	void * buffer=NULL;
	int offset=0;
	uint32_t size_payload=0;
//	t_comida * comida=malloc(sizeof(t_comida ));

			void _serializar_tabla_comida(void * elemento){
				t_pagina_comida * adm_comida=(t_pagina_comida *)elemento;
				t_comida * comida=malloc(sizeof(t_comida ));

				//printf("El valor del offset es: %d \n",offset);

				if (adm_comida->esta_en_memoria_principal!=TRUE){
					log_info(logger,"El plato que se quiere leer no se encuentra en Memoria Principal. Se tiene que cargar");
					adm_comida=cargar_pagina_a_memoria_principal(adm_comida);			//Busca frame libre y copia lo q esta en SWAP en principal
				}

				leer_pagina_en_memoria(adm_comida->frame->direccion_frame,comida);

				copiar_pagina_en_memoria(buffer+offset,comida);

				offset+=SIZE_PAGINA;

				adm_comida->last_used=timestamp();

/*				printf("Estoy iterando la lista del pedido\n");
				printf("Cantidad lista de comida: %d \n",comida->cantidad_lista_comida);
				printf("Cantidad total pedida del plato: %d \n", comida->cantidad_total_comida);
				printf("Nombre del plato: %s \n",comida->nombre_comida);
*/
			}


	if(list_is_empty(lista_restarurantes))								//Verifico que no este vacia la lista
		goto envio_de_respuesta;

	t_restaurante * restaurante=NULL;
	restaurante=busqueda_de_restaurante(pedido_solicitado->nombre_restaurante);


	if(restaurante==NULL){
		log_info(logger,"No se encontro el restaurante\n");   //Se informa que no existe el restaurante
		goto envio_de_respuesta;}
	else
		log_info(logger,"Se encontro el restaurant: %s \n",restaurante->nombre_restaurante);

	if(list_is_empty(restaurante->tabla_pedidos))						//Verifico que no este vacia la lista
		goto envio_de_respuesta;

	t_pedido_seg * pedido=NULL;
	pedido=buscar_el_pedido(pedido_solicitado,restaurante->tabla_pedidos);

	if(pedido==NULL){
		log_info(logger,"No se encontro el pedido\n");//Se informa que no existe el pedido
		goto envio_de_respuesta;}
	else{
		log_info(logger,"Se encontro el pedido numero: %d\n",pedido->id_pedido);

		int size_lista_pedido=list_size(pedido->comidas_del_pedido);
		log_info(logger,"El tamaño de la lista del pedido es: %d",size_lista_pedido);
		size_payload=(SIZE_PAGINA*size_lista_pedido)+sizeof(estado_pedido);
//		log_info(logger,"El size del payload es: %d",size_payload);
		buffer=malloc(size_payload);

		memcpy(buffer,&(pedido->estado),sizeof(estado_pedido));				//Copio el estado del pedido
		offset+=sizeof(estado_pedido);

		list_iterate(pedido->comidas_del_pedido,_serializar_tabla_comida); //Itero para copiar los datos de los platos

//		mem_hexdump(buffer, SIZE_PAGINA);
	    }


	envio_de_respuesta:
	;
	cod_msg tipo_msj=RESPUESTA_OBTENER_PEDIDO;
	armar_envio_obtener_pedido(tipo_msj,socket_cliente,size_payload,buffer);
	printf("\n");
	printf("\n");
	printf("\n");
}


void administrar_plato_listo(t_header * encabezado,int socket_cliente){

	t_guardar_plato * plato=(t_guardar_plato*)encabezado->payload;
	bool exito=FALSE;

							bool buscar_comida(void * elemento){
								t_pagina_comida * adm_comida=(t_pagina_comida*)elemento;

								t_comida * comida=(t_comida*)adm_comida->frame_swap->direccion_frame;

								if(string_equals_ignore_case(comida->nombre_comida,plato->nombre_plato)){
										return TRUE;
										}
								return FALSE;
							}

	if(list_is_empty(lista_restarurantes))								//Verifico que no este vacia la lista
		goto envio_de_respuesta;

	t_restaurante * restaurante=NULL;
	restaurante=busqueda_de_restaurante(plato->pedido->nombre_restaurante);


	if(restaurante==NULL){
		log_info(logger,"No se encontro el restaurante\n");   //Se informa que no existe el restaurante
		goto envio_de_respuesta;}
	else
		log_info(logger,"Se encontro el restaurant: %s \n",restaurante->nombre_restaurante);

	if(list_is_empty(restaurante->tabla_pedidos))						//Verifico que no este vacia la lista
		goto envio_de_respuesta;

	t_pedido_seg * pedido=NULL;
	pedido=buscar_el_pedido(plato->pedido,restaurante->tabla_pedidos);

	if(pedido==NULL){
		log_info(logger,"No se encontro el pedido\n");//Se informa que no existe el pedido
		goto envio_de_respuesta;}
	else
		log_info(logger,"Se encontro el pedido numero: %d\n",pedido->id_pedido);

	if(pedido->estado!=CONFIRMADO){
		log_info(logger,"El pedido se encuentra en el ESTADO: %s . Por lo que no se puede sumar el plato listo", nro_estado_pedido_a_texto(pedido->estado));
		goto envio_de_respuesta;
	}

	t_pagina_comida * adm_comida=NULL;

	if(!list_is_empty(pedido->comidas_del_pedido))						//Verifico que no este vacia la lista
			adm_comida=list_find(pedido->comidas_del_pedido,buscar_comida);

	if(adm_comida==NULL){
		log_info(logger,"No se encontro el plato en el pedido");
		goto envio_de_respuesta;
		}
	else{log_info(logger,"Se encontro el plato en el pedido");

		if(adm_comida->esta_en_memoria_principal!=TRUE){

			log_info(logger,"El plato que se quiere modificar, no se encuentra en Memoria Principal. Hay que cargarlo.");

			adm_comida=cargar_pagina_a_memoria_principal(adm_comida);			//Busca frame libre y copia lo q esta en SWAP en principal
		}


		 exito=sumar_plato_listo(adm_comida);

	     verificar_pedido_completo(pedido);

		}

	envio_de_respuesta:
	;
	cod_msg tipo_msj;
	if(exito==TRUE)
		tipo_msj=OK;
	else
		tipo_msj=FAIL;

	armar_y_enviar_respuesta(tipo_msj,socket_cliente);
	printf("\n");
	printf("\n");
	printf("\n");
}


void administrar_confirmar_pedido(t_header * encabezado,int socket_cliente){

	t_pedido * pedido_solicitado=(t_pedido*)encabezado->payload;
	bool exito=FALSE;

	if(list_is_empty(lista_restarurantes))								//Verifico que no este vacia la lista
		goto envio_de_respuesta;

	t_restaurante * restaurante=NULL;
	restaurante=busqueda_de_restaurante(pedido_solicitado->nombre_restaurante);

	if(restaurante==NULL){
		printf("No se encontro el restaurante\n");   //Se informa que no existe el restaurante
		goto envio_de_respuesta;}
	else
		log_info(logger,"Se encontro el restaurant: %s \n",restaurante->nombre_restaurante);

	if(list_is_empty(restaurante->tabla_pedidos))						//Verifico que no este vacia la lista
		goto envio_de_respuesta;

	t_pedido_seg * pedido=NULL;
	pedido=buscar_el_pedido(pedido_solicitado,restaurante->tabla_pedidos);

	if(pedido==NULL){
		log_info(logger,"No se encontro el pedido. No se puede finalizar.\n");//Se informa que no existe el pedido
		goto envio_de_respuesta;}
	else{
		log_info(logger, "Se encontro el pedido %d  y esta en el estado %s",pedido->id_pedido,nro_estado_pedido_a_texto(pedido->estado));
		if(pedido->estado==CONFIRMADO){
			 log_info(logger,"El pedido ya se encontraba CONFIRMADO");
			 goto envio_de_respuesta;}
		 else{
			 pedido->estado=CONFIRMADO;
			 exito=TRUE;
			 log_info(logger,"Se actualizo el estado del pedido y ahora se encuentra CONFIRMADO");
		     }

	    }


	envio_de_respuesta:

	;
	cod_msg tipo_msj;
	if(exito==TRUE)
		tipo_msj=OK;
	else
		tipo_msj=FAIL;

	armar_y_enviar_respuesta(tipo_msj,socket_cliente);
	printf("\n");
	printf("\n");
	printf("\n");
}

void administrar_finalizar_pedido(t_header * encabezado,int socket_cliente){

	t_pedido * pedido_solicitado=(t_pedido*)encabezado->payload;
	bool exito=FALSE;

						bool buscar_pedido(void * elemento){
							t_pedido_seg * pedido=(t_pedido_seg*)elemento;
							//printf("El numero de pedido que estoy iterando es: %d\n",pedido->id_pedido);

							if(pedido->id_pedido==pedido_solicitado->id_pedido){
									return TRUE;
									}
							return FALSE;
						}
						void agregar_marcos_libres(void * elemento){

							t_pagina_comida * adm_comida=(t_pagina_comida *)elemento;

							list_add(tabla_frames_libres_swap,adm_comida->frame_swap);
							adm_comida->frame_swap=NULL;

							if(adm_comida->esta_en_memoria_principal==TRUE){
								list_add(tabla_frames_libres,adm_comida->frame);
								adm_comida->frame=NULL;
							}
						}


	if(list_is_empty(lista_restarurantes))								//Verifico que no este vacia la lista
		goto envio_de_respuesta;

	t_restaurante * restaurante=NULL;
	restaurante=busqueda_de_restaurante(pedido_solicitado->nombre_restaurante);


	if(restaurante==NULL){
		printf("No se encontro el restaurante\n");   //Se informa que no existe el restaurante
		goto envio_de_respuesta;}
	else
		printf("Se encontro el restaurant: %s \n",restaurante->nombre_restaurante);

	if(list_is_empty(restaurante->tabla_pedidos))						//Verifico que no este vacia la lista
		goto envio_de_respuesta;

	t_pedido_seg * pedido=NULL;
	log_info(logger,"El tamaño de la lista de pedidos antes de la busqueda es: %d",list_size(restaurante->tabla_pedidos));
	pedido=list_remove_by_condition(restaurante->tabla_pedidos,buscar_pedido);

	if(pedido==NULL){
		printf("No se encontro el pedido\n");//Se informa que no existe el pedido
		goto envio_de_respuesta;}
	else{
		 if(pedido->estado==TERMINADO){		 															//Solo finaliza si esta en estado TERMINADO
			 log_info(logger, "Se encontro el pedido %d  y se elimina de la lista de pedidos",pedido->id_pedido);
			 exito=TRUE;
			 log_info(logger,"El tamaño de la lista de pedidos despues de eliminar el pedido es: %d",list_size(restaurante->tabla_pedidos));
			 log_info(logger,"Se FINALIZO el PEDIDO correctamente");

			 list_iterate(pedido->comidas_del_pedido,agregar_marcos_libres);     //Agrego en la tabla de frames libres, los espacios liberados
		 }else{
			  log_info(logger,"Se encontro el pedido, pero NO SE FINALIZA ya que no se encuentra en ESTADO TERMINADO");
		      }
		 }


	envio_de_respuesta:

	;
	cod_msg tipo_msj;
	if(exito==TRUE)
		tipo_msj=OK;
	else
		tipo_msj=FAIL;

	armar_y_enviar_respuesta(tipo_msj,socket_cliente);
	printf("\n");
	printf("\n");
	printf("\n");
}

void armar_y_enviar_respuesta(cod_msg tipo_msj,int socket_cliente){

	t_header * nuevo_encabezado=malloc(sizeof(t_header));

	nuevo_encabezado->id_proceso=100;
	nuevo_encabezado->modulo=COMANDA;
	nuevo_encabezado->nro_msg=tipo_msj;

	nuevo_encabezado->size=0;
	nuevo_encabezado->payload=NULL;

	bool exito_envio=enviar_buffer(socket_cliente,nuevo_encabezado);

	if(exito_envio==FALSE)log_error(logger,"No se envio correctamente la respuesta al modulo");
	else log_info(logger,"Se envio correctamente la respuesta al modulo");

	free(nuevo_encabezado);
	printf("----------------------------------------------------------------------------------------------\n");


}

void       armar_envio_obtener_pedido     (cod_msg tipo_msj,int socket_cliente,int size_payload,void * buffer){

	t_header * nuevo_encabezado=malloc(sizeof(t_header));

	nuevo_encabezado->id_proceso=100;
	nuevo_encabezado->modulo=COMANDA;
	nuevo_encabezado->nro_msg=RESPUESTA_OBTENER_PEDIDO;
	nuevo_encabezado->size=size_payload;
	nuevo_encabezado->payload=buffer;

//	mem_hexdump(buffer, size_payload);

	bool exito_envio=enviar_buffer(socket_cliente,nuevo_encabezado);

	if(exito_envio==FALSE)log_error(logger,"No se envio correctamente la respuesta al modulo");

	free(nuevo_encabezado);
	//free(comida);
	printf("-------------------------------------------------------------------------------------------------\n");
}

bool sumar_cantidad_total_plato(t_pagina_comida * adm_comida,t_guardar_plato * plato){

	t_comida * comida=malloc(sizeof(t_comida ));

	leer_pagina_en_memoria(adm_comida->frame->direccion_frame,comida);
	log_info(logger,"La cantidad total del plato antes de sumarse lo nuevo es %d",comida->cantidad_total_comida);

	comida->cantidad_total_comida+=plato->cantidad_plato;
	log_info(logger,"El plato %s ahora tiene una cantidad total de: %d",comida->nombre_comida,comida->cantidad_total_comida);

	copiar_pagina_en_memoria(adm_comida->frame->direccion_frame,comida);

	log_error(logger,"Se actualiza en Memoria Principal el nuevo valor de la comida hecha en el frame %d y la direccion de memoria %p",adm_comida->frame->nro_frame,adm_comida->frame->direccion_frame);;

	adm_comida->last_used=timestamp();
	adm_comida->modificado=TRUE;
	adm_comida->bit_de_uso=TRUE;

	log_info(logger,"Se actualiza el timestamp con el valor: %d", adm_comida->last_used);

	return TRUE;

}

bool crear_nuevo_plato (t_guardar_plato * plato,t_pedido_seg * pedido){

	t_pagina_comida * adm_comida=malloc(sizeof(t_pagina_comida));
	t_comida * comida=malloc(sizeof(t_comida));

					void inicializar_vector(){
						for(int i=0;i<SIZE_VECTOR_NOMBRE_PLATO;i++){
							comida->nombre_comida[i]='\0';
						}
					}


	log_info(logger,"Comienzo a buscar un frame libre en la Memoria Swap");
	adm_comida->frame_swap=buscar_frame_libre(tabla_frames_libres_swap);	//Busco un frame libre en SWAP para agregar el plato

	if(adm_comida->frame_swap==NULL){
		log_info(logger,"No hay mas espacio en la memoria SWAP. No se puede cargar el nuevo plato");
		return FALSE;
		}
										//Guardo el nuevo plato en SWAP
	comida->cantidad_lista_comida=0;
	comida->cantidad_total_comida=plato->cantidad_plato;
	inicializar_vector();
	strcpy(comida->nombre_comida,plato->nombre_plato);

	log_info(logger,"Se arranca a copiar el nuevo plato en Memoria SWAP");
//	log_info(logger,"Pruebo la direccion y el numero de frame devuelto por la funcion:  %p y %d", adm_comida->frame_swap->direccion_frame,adm_comida->frame_swap->nro_frame);
	copiar_pagina_en_memoria(adm_comida->frame_swap->direccion_frame,comida);

	log_error(logger,"Se copio la pagina en Memoria Swap, en el frame N°: %d , direccion de memoria: %p",adm_comida->frame_swap->nro_frame,adm_comida->frame_swap->direccion_frame);

	log_info(logger,"Se arranca con el procedimiento para guardar el nuevo plato en Memoria Principal");
	adm_comida=cargar_pagina_a_memoria_principal(adm_comida  );

	adm_comida->modificado=FALSE;

	list_add(pedido->comidas_del_pedido,adm_comida);            //Guardo la nueva comida en el pedido

//	printf("PRUEBO LOS DATOS DEL PLATO SI SE GUARDARON BIEN EN MEMORIA\n");
	//free(comida);
//	t_comida * comida_prueba=malloc(sizeof(t_comida));
//	leer_pagina_en_memoria(adm_comida->frame->direccion_frame,comida_prueba);
//	log_info(logger,"El nombre de la comida es: %s",comida_prueba->nombre_comida);
//	log_info(logger,"La cantidad de comida para cocinar es: %d",comida_prueba->cantidad_total_comida);
//	log_info(logger,"La cantidad de comida ya cocinada es: %d",comida_prueba->cantidad_lista_comida);


	//t_pagina_comida * prueba_comida=(t_pagina_comida *)list_get(pedido->comidas_del_pedido,0);
	//mem_hexdump(prueba_comida->contenido, sizeof(t_comida));
	//t_comida * contenido_comida=(t_comida *)prueba_comida->contenido;
	//log_error(logger,"Probando el nombre de la comida despues de haberse guardado, %s",contenido_comida->nombre_comida);

	log_info(logger,"Ahora la cantidad de frames libres en Memoria Principal es: %d",list_size(tabla_frames_libres));


	return TRUE;

}

void verificar_pedido_completo(t_pedido_seg * pedido){

				bool verificar_pedido_terminado(void * elemento){

					t_pagina_comida * adm_comida=(t_pagina_comida *)elemento;
					t_comida * comida=malloc(sizeof(t_comida));

					if(adm_comida->esta_en_memoria_principal!=TRUE){
						printf("\n");
						log_info(logger,"El plato que se quiere leer no se encuentra en Memoria Principal. Hay que cargarlo.");
						adm_comida=cargar_pagina_a_memoria_principal(adm_comida);			//Busca frame libre y copia lo q esta en SWAP en principal
					}

					leer_pagina_en_memoria(adm_comida->frame->direccion_frame,comida);
					adm_comida->last_used=timestamp();

					if(comida->cantidad_lista_comida!=comida->cantidad_total_comida){
						free(comida);
						return TRUE;}
					return FALSE;
				}


	t_pagina_comida * comida_pedido=NULL;											//Verifico si todos los platos del pedido estan completos. De ser asi, cambio el pedido a TERMINADO
	log_info(logger,"Se verifica que el pedido este completo, el cual esta compuesto por %d  comidas ", list_size(pedido->comidas_del_pedido));
	comida_pedido=list_find(pedido->comidas_del_pedido,verificar_pedido_terminado);

	printf("\n");
	if(comida_pedido==NULL){
		pedido->estado=TERMINADO;
		log_info(logger,"Se FINALIZO el pedido. Se encuentra con todos sus platos terminados");
		}
	else{
		log_info(logger,"Todavia no se encuentra completo el pedido");
	}

}

bool sumar_plato_listo(t_pagina_comida * adm_comida){

	t_comida * comida=malloc(sizeof(t_comida));
	leer_pagina_en_memoria(adm_comida->frame->direccion_frame,comida);

	if(comida->cantidad_lista_comida<comida->cantidad_total_comida){
			comida->cantidad_lista_comida++;
			log_info(logger,"Se va a proceder a sumar el plato listo correspondiente");
			if(comida->cantidad_lista_comida!=comida->cantidad_total_comida)				//Verifico si se completo la cantidad total del plato
				log_info(logger,"Ahora la cantidad lista del plato es: %d  con un total para hacer de: %d",comida->cantidad_lista_comida,comida->cantidad_total_comida);
			else
				log_info(logger,"Se encuentran listos todos los platos de %s con una cantidad de: %d",comida->nombre_comida,comida->cantidad_lista_comida);

			copiar_pagina_en_memoria(adm_comida->frame->direccion_frame,comida);
			log_info(logger,"Se actualizo la cantidad de platos listos en la Memoria Principal");
			adm_comida->last_used=timestamp();
			adm_comida->modificado=TRUE;
			free(comida);
			return TRUE;
	}else{
			log_info(logger,"Ya se habian cocinado todos los platos de: %s", comida->nombre_comida);
			free(comida);
			return FALSE;}

}

t_pedido * recibir_consulta_pedido(void * payload){

	t_pedido * pedido=recibir_pedido(payload);

	return pedido;
}

t_restaurante * busqueda_de_restaurante(char * nombre_restaurante){

		bool buscar_restaurante(void * elemento){
			t_restaurante * restaurante=(t_restaurante*)elemento;

			if(string_equals_ignore_case(restaurante->nombre_restaurante,nombre_restaurante)){
					return TRUE;
					}
			return FALSE;
		}
	t_restaurante * restaurante=list_find(lista_restarurantes,buscar_restaurante);   //Busca si existe el restaurante

	return restaurante;

}

t_pedido_seg * buscar_el_pedido(t_pedido * pedido_solicitado, t_list * tabla_pedidos){

			bool buscar_pedido(void * elemento){
				t_pedido_seg * pedido=(t_pedido_seg*)elemento;
				//printf("El numero de pedido que estoy iterando es: %d\n",pedido->id_pedido);

				if(pedido->id_pedido==pedido_solicitado->id_pedido){
						return TRUE;
						}
				return FALSE;
			}

	t_pedido_seg * pedido=NULL;
	pedido= list_find(tabla_pedidos,buscar_pedido);//Busco si se encuentra el pedido

	return pedido;
}
