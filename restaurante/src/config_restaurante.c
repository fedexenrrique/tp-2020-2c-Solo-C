/*
 * config_restaurante.c
 *
 *  Created on: 11 sep. 2020
 *      Author: utnso
 */

#include "config_restaurante.h"


void cargar_config(void) {
	logger_restaurante = iniciar_logger();
	config_restaurante = leer_config();

	if((config_restaurante == NULL ) || (!config_valida(config_restaurante))) {
		log_error(logger_restaurante,"ERROR al leer la config");
		perror("config restaurante");
	}

	puerto_escucha = config_get_string_value(config_restaurante, "PUERTO_ESCUCHA");
	ip_sindicato = config_get_string_value(config_restaurante, "IP_SINDICATO");
	puerto_sindicato = config_get_string_value(config_restaurante, "PUERTO_SINDICATO");
	ip_app = config_get_string_value(config_restaurante, "IP_APP");
	puerto_app = config_get_string_value(config_restaurante, "PUERTO_APP");
	quantum = config_get_int_value(config_restaurante, "QUANTUM");
	archivo_log = config_get_string_value(config_restaurante, "ARCHIVO_LOG");
	algoritmo_planificacion = config_get_string_value(config_restaurante, "ALGORITMO_PLANIFICACION");
	nombre_restaurante = config_get_string_value(config_restaurante, "NOMBRE_RESTAURANTE");

	log_info(logger_restaurante,
		"\nPUERTO_ESCUCHA: %s\n"
		"IP_SINDICATO: %s\n"
		"PUERTO_SINDICATO: %s\n"
		"IP_APP: %s\n"
		"PUERTO_APP: %s\n"
		"QUANTUM: %d\n"
		"PATH_ARCHIVO_LOG: %s\n"
		"ALGORIMO_PLANIFICACION: %s\n"
		"NOMBRE_RESTAURANTE: %s\n",
		puerto_escucha,
		ip_sindicato,
		puerto_sindicato,
		ip_app,
		puerto_app,
		quantum,
		archivo_log,
		algoritmo_planificacion,
		nombre_restaurante);

}

int config_valida(t_config* fd_configuracion) {
	return (config_has_property(fd_configuracion, "PUERTO_ESCUCHA")
		&& config_has_property(fd_configuracion, "IP_SINDICATO")
		&& config_has_property(fd_configuracion, "PUERTO_SINDICATO")
		&& config_has_property(fd_configuracion, "IP_APP")
		&& config_has_property(fd_configuracion, "PUERTO_APP")
		&& config_has_property(fd_configuracion, "QUANTUM")
		&& config_has_property(fd_configuracion, "ARCHIVO_LOG")
		&& config_has_property(fd_configuracion, "ALGORITMO_PLANIFICACION")
		&& config_has_property(fd_configuracion, "NOMBRE_RESTAURANTE"));
}

t_log * iniciar_logger(void){
	logger=log_create("restaurante.log", "restaurante", 1, LOG_LEVEL_INFO);
	return logger;


}

t_config* leer_config(void){
	t_config * fd_configuracion;

	fd_configuracion = config_create("../restaurante.config");
		if (fd_configuracion == NULL) {
			fd_configuracion = config_create("restaurante.config");
		}
	return fd_configuracion;
}

void limpiar_config() {
	config_destroy(config_restaurante);
	log_destroy(logger_restaurante);
}
