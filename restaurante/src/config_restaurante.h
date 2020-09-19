/*
 * config_restaurante.h
 *
 *  Created on: 11 sep. 2020
 *      Author: utnso
 */

#ifndef CONFIG_RESTAURANTE_H_
#define CONFIG_RESTAURANTE_H_

#include "serializar.h"

t_log    	*	logger_restaurante;
t_config	* 	config_restaurante;
char		*	puerto_escucha;
char 		*	ip_sindicato;
char		*	puerto_sindicato;
char		*	ip_app;
char		*	puerto_app;
uint32_t        quantum;
char		*	archivo_log;
char		*	algoritmo_planificacion;
char		*	nombre_restaurante;

uint32_t        g_id_proceso;

void      		cargar_config	(void);
int				config_valida	(t_config*);
t_log 		*   iniciar_logger	(void);
t_config	* 	leer_config		(void);
void			limpiar_config	(void);



#endif /* CONFIG_RESTAURANTE_H_ */
