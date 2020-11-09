/*
 * utils.h
 *
 *  Created on: 5 sep. 2020
 *      Author: utnso
 */

#ifndef UTILS_H_
#define UTILS_H_

#include "memoria.h"


int       iniciar_comanda();
t_log *   iniciar_logger ();
t_config* leer_config    ();


void manejo_modulo_conectado(void*);




t_pedido       * recibir_consulta_pedido   (void*);


void administrar_guardar_pedido  (t_header*,int);
void administrar_guardar_plato   (t_header*,int);
void administrar_obtener_pedido  (t_header*,int);
void administrar_confirmar_pedido(t_header*,int);
void administrar_plato_listo     (t_header*,int);
void administrar_finalizar_pedido(t_header*,int);

uint64_t   timestamp                 (void       );
void       armar_y_enviar_respuesta  (cod_msg,int);
void       armar_envio_obtener_pedido(cod_msg,int,int,void*);
bool       sumar_cantidad_total_plato(t_pagina_comida*,t_guardar_plato*);
bool       crear_nuevo_plato         (t_guardar_plato*,t_pedido_seg *  );
bool       sumar_plato_listo         (t_pagina_comida *);
void       verificar_pedido_completo (t_pedido_seg *   );

t_restaurante * busqueda_de_restaurante(char * );
t_pedido_seg  * buscar_el_pedido       (t_pedido *,t_list *);


#endif /* UTILS_H_ */
