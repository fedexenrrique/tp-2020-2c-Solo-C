/*
 * restaurante.h
 *
 *  Created on: 3 sep. 2020
 *      Author: utnso
 */

#ifndef RESTAURANTE_H_
#define RESTAURANTE_H_

#include "config_restaurante.h"
#include <semaphore.h>

// AMBIENTE

int					id_pedido_restaurante;
t_list			*	platos;
t_list			*	cocineros;
uint32_t			posicion_x;
uint32_t			posicion_y;
uint32_t			cantidad_hornos;
t_dictionary	*	platos_precios;

t_list			* 	colas_ready;
t_queue			*	cola_io;
t_queue			* 	cola_bloqueados;
t_queue			* 	cola_exit;

sem_t			*	sem_hornos;

typedef struct{
	uint32_t		id_pedido;
	char		* 	estado_pedido;
	char		* 	lista_platos;
	char		* 	cantidad_platos;
	uint32_t 		precio_total;
}pedido;

typedef struct {
	uint32_t 		id_pedido;
	uint32_t  		size_nombre_plato;
	char 		*	nombre_plato;
    uint32_t  		cantidad_plato;
}aniadir_plato;

typedef struct {
	uint32_t		id_pedido;
	char		*	nombre_plato;
	char		*	estado;
	t_list		*	receta;
	t_list		*	receta_faltante;
}pcb_plato;

// FUNCIONES

int									main													(void);
void 								obtener_info_restaurante								(void);
void								iniciar_planificacion									(void);
void								planificador_io											(void);
void								planificador_bloqueados									(void);
t_queue							*	obtener_cola_afinidad									(char *);
void								cargar_variables										(t_respuesta_info_restaurante *);
t_respuesta_info_restaurante 	* 	deserializar_respuesta_info_restaurante					(void *);
void								consultar_platos_restaurante							(void);
t_respuesta_platos_restaurante 	* 	deserializar_respuesta_consulta_platos					(void *);
void 								crear_pedido_restaurante								(pedido *);
int 								deserializar_respuesta_creacion_pedido					(void *);
void 								aniadir_plato_restaurante								(aniadir_plato *);
t_creacion_pedido				*	obtener_pedido											(int);
t_creacion_pedido				*	deserializar_respuesta_obtener_pedido_restaurante		(void *);
void								confirmar_pedido										(int);
t_respuesta_receta				*	obtener_receta											(char *);
t_respuesta_receta				*	deserializar_respuesta_receta							(void *);
void 								conectar_restaurante_a_applicacion						(void);

#endif /* RESTAURANTE_H_ */
