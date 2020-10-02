/*
 * memoria.h
 *
 *  Created on: 13 sep. 2020
 *      Author: utnso
 */

#ifndef MEMORIA_H_
#define MEMORIA_H_

#include "serializar.h"


typedef struct {
	char * nombre_restaurante;
	t_list * tabla_pedidos;
}t_restaurante;


typedef struct{
	uint32_t      id_pedido    ;
	estado_pedido estado       ;
	t_list * comidas_del_pedido;
}t_pedido_seg;



typedef struct {
	bool     esta_en_memoria      ;
	void *   direccion_memoria    ;
	void *   contenido            ;
}t_pagina_comida;


//1 t_tabla_segmentos X restaurante  (contiene los t_segmneto)
//1 t_segmento  X cada pedido   (contiene los t_pagina)
//1 t_pagina contiene un t_comida


t_list * lista_restarurantes;


t_restaurante * crear_tabla_segmentos_restaurante(char *                  );
bool            agregar_pedido_a_tabla_segmentos (t_restaurante *,uint32_t);


#endif /* MEMORIA_H_ */
