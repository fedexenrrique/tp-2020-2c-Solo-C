/*
 * utils.h
 *
 *  Created on: 5 sep. 2020
 *      Author: utnso
 */

#ifndef UTILS_H_
#define UTILS_H_

#include "serializar.h"


int       iniciar_comanda();
t_log *   iniciar_logger ();
t_config* leer_config    ();


void manejo_modulo_conectado(void*);


t_guardar_plato* administrar_guardar_plato (void*);
t_plato_listo  * administrar_plato_listo   (void*);
t_pedido       * recibir_consulta_pedido   (void*);



#endif /* UTILS_H_ */
