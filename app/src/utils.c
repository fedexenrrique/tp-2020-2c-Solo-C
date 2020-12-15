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


	//Le mando el nombre en null, para reutilizar la funcion
//	t_header * encabezado=serializar_pedido(nro_msg, nombre_restaurante,id_pedido);

	t_header * encabezado=malloc(sizeof(t_header));
	int offset=0;

	int size_buffer=sizeof(uint32_t);
	void * buffer=malloc(size_buffer);

	memcpy(buffer+offset,&id_pedido,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	encabezado->payload=buffer;
	encabezado->size=size_buffer;
	encabezado->id_proceso=1;
	encabezado->modulo=APP;
	encabezado->nro_msg=CONFIRMAR_PEDIDO;


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

	if(info_resto==NULL)log_error(logger,"No encontre el nombre del restaurant en los existentes");

	log_info(logger,"Se encontro la informacion del restaurant buscado: %s",info_resto->resto_nombre);

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

void procesar_consultar_pedido(uint32_t id_pedido,t_header * header_recibido,int socket_cliente){

		bool _control_existe_asociacion_cliente_resto ( void * p_elem ) {

			return ( ((t_cliente_a_resto*)p_elem)->id_cliente == header_recibido->id_proceso ) ? true : false ;

		}

	t_cliente_a_resto * asociacion = list_find( lista_clientes , _control_existe_asociacion_cliente_resto );

	if ( asociacion != NULL ) {

		printf("Se encontró asociación.\n");

//	if(string_equals_ignore_case(asociacion->nombre_resto,"default"))//Verifico si es default y envio los platos default
//		return g_platos_default;

	int conexion=enviar_obtener_pedido(g_ip_comanda,g_puerto_comanda,asociacion->nombre_resto,id_pedido);

	header_recibido=recibir_buffer(conexion);
	log_info(logger,"Se recibio el mensaje: %s",nro_comando_a_texto((uint32_t)header_recibido->nro_msg));

	t_info_restaurante * info_resto= buscar_info_de_restaurante(asociacion->nombre_resto,lista_resto_conectados);
	if(info_resto==NULL)log_info(logger,"No se encontro el restaurante en la lista de restos conectados");
	info_resto->socket_conectado=socket_cliente;

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

	//log_info(logger,"size nombre: %d - size payload: %d ",size_nombre_resto,size_payload_nuevo);

	memcpy(payload_nuevo+offset,&size_nombre_resto,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(payload_nuevo+offset,info_resto->resto_nombre,size_nombre_resto);
	offset+=size_nombre_resto;

	memcpy(payload_nuevo+offset,header_recibido->payload,header_recibido->size);

//	mem_hexdump(payload_nuevo, size_payload_nuevo);
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

t_plato_listo_para_app * recibir_10_plato_listo_resto_app(t_header * header_recibido){

	t_plato_listo_para_app * plato_listo=malloc(sizeof(t_plato_listo_para_app));
	uint32_t offset=0;

	//plato_listo->pedido=malloc(sizeof(t_pedido));

	memcpy(&plato_listo->id_pedido,header_recibido->payload+offset,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(&plato_listo->size_nombre_plato,header_recibido->payload+offset,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	plato_listo->nombre_plato=malloc(plato_listo->size_nombre_plato);

	memcpy(plato_listo->nombre_plato,header_recibido->payload+offset,sizeof(uint32_t));

	return plato_listo;


}


bool realizar_plato_listo(t_plato_listo_para_app * plato_listo,uint32_t id_proceso){//plato listo no va a tener el size de nombre resto

			bool  buscar_resto_conectado(void * elemento){

				t_info_restaurante * info_resto=(t_info_restaurante *)elemento;

				if(info_resto->id_resto==id_proceso)
					return TRUE;
				return FALSE;

			}
			bool verificar_existencia_pedido_resto(void * elemento){
				t_cliente_a_resto * asociacion=(t_cliente_a_resto*)elemento;

				if(string_equals_ignore_case(plato_listo->nombre_plato,asociacion->nombre_resto)&&plato_listo->id_pedido==asociacion->id_pedido)
					return TRUE;
				return FALSE;

			}

	t_info_restaurante * info_resto=list_find(lista_resto_conectados,buscar_resto_conectado); //Busco resto por el id

	t_cliente_a_resto * asociacion=list_find(lista_clientes,verificar_existencia_pedido_resto);//Busco el nombre del resto, en la cola de los pedidos confirmados


	if(info_resto==NULL || asociacion==NULL){
		log_info(logger,"No se encuentra el restaurante en la lista de conectados o el pedido no se encuentra confirmado");
		return FALSE;
	}
	//plato_listo->nombre_restaurante=asociacion->nombre_resto;

	bool resultado=enviar_plato_listo(g_ip_comanda,g_puerto_comanda,asociacion->nombre_resto,plato_listo->id_pedido,plato_listo->nombre_plato);

	int conexion=enviar_obtener_pedido(g_ip_comanda,g_puerto_comanda,asociacion->nombre_resto,plato_listo->id_pedido);

	t_header * encabezado=recibir_buffer(conexion);

	deserializar_obtener_pedido_y_verificar_estado_correcto(encabezado);

	return resultado;


}

bool deserializar_obtener_pedido_y_verificar_estado_correcto(t_header * encabezado){

	t_list * lista_platos_del_pedido=list_create();
	int cantidad_total_platos=(encabezado->size-sizeof(estado_pedido))/sizeof(t_comida);
	t_pedido_seg * pedido=malloc(sizeof(t_pedido));
	int offset=0;

			bool verificar_platos_terminados(void * elemento){
				t_comida * comida=(t_comida*)elemento;
				if(comida->cantidad_lista_comida!=comida->cantidad_total_comida)
					return TRUE;
				return FALSE;
			}

	memcpy(&(pedido->estado),encabezado->payload+offset,sizeof(estado_pedido));
	offset+=sizeof(estado_pedido);

	for(int i=0;i<cantidad_total_platos;i++){
		t_comida * comida=malloc(sizeof(t_comida));

		memcpy(&(comida->cantidad_lista_comida),encabezado->payload+offset,sizeof(uint32_t));
		offset+=sizeof(uint32_t);

		memcpy(&(comida->cantidad_total_comida),encabezado->payload+offset,sizeof(uint32_t));
		offset+=sizeof(uint32_t);

		memcpy(comida->nombre_comida,encabezado->payload+offset,SIZE_VECTOR_NOMBRE_PLATO);
		offset+=SIZE_VECTOR_NOMBRE_PLATO;

		list_add(lista_platos_del_pedido,comida);
//		log_error(logger,"El numero de iteracion es: %d",i);
	}

	pedido->comidas_del_pedido=lista_platos_del_pedido;

	log_info(logger,"El pedido se encuentra en estado: %s",nro_estado_pedido_a_texto(pedido->estado));

    if(list_is_empty(pedido->comidas_del_pedido)){
    	log_info(logger,"El pedido no contiene ningun plato");
    	list_destroy(lista_platos_del_pedido);
    	return TRUE;
    }

	t_comida * comida=NULL;
    comida=(t_comida*)list_find(pedido->comidas_del_pedido,verificar_platos_terminados);

	if(comida==NULL&&pedido->estado==TERMINADO){//Verifico si es correcto el estado

		log_info(logger,"El pedido esta TERMINADO. Ya puede ser buscado por su repartidor");

		//ACA TENDRIA QUE PERMITIR AL REPARTIDOR BUSCAR EL PEDIDO

	}else{if(comida!=NULL&&pedido->estado==CONFIRMADO){
		      log_info(logger,"El estado en el que se encuentra el pedido es CONFIRMADO");
		      return TRUE;
	      }else{
	    	  log_error(logger,"El estado del pedido es erroneo o se encuentra en estado PENDIENTE");

	      }

	}

	list_destroy(lista_platos_del_pedido);

	return FALSE;

}
