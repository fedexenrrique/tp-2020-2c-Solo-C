/*
 * configResaurante.c
 *
 *  Created on: 7 sep. 2020
 *      Author: utnso
 */
#include "restaurante.h"

int configValida(t_config* fd_configuracion) {
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

int cargarConfiguracion() {
	logger = log_create("LogRestaurante", "Restaurante", true, LOG_LEVEL_INFO);
	configuracion = malloc(sizeof(tConfiguracion));


	fd_configuracion = config_create("../restaurante.config");
	if (fd_configuracion == NULL) {
		fd_configuracion = config_create("restaurante.config");
	}

	if (fd_configuracion == NULL || !configValida(fd_configuracion)) {
		log_error(logger,"Archivo de configuración inválido.","ERROR");
		return -1;
	}
	configuracion->puertoEscucha = config_get_string_value(fd_configuracion, "PUERTO_ESCUCHA");
	configuracion->ipSindicato = config_get_string_value(fd_configuracion, "IP_SINDICATO");
	configuracion->puertoSindicato = config_get_int_value(fd_configuracion, "PUERTO_SINDICATO");
	configuracion->ipApp = config_get_string_value(fd_configuracion, "IP_APP");
	configuracion->puertoApp = config_get_int_value(fd_configuracion, "PUERTO_APP");
	configuracion->quantum= config_get_int_value(fd_configuracion, "QUANTUM");
	configuracion->pathArchivoLog = config_get_string_value(fd_configuracion, "ARCHIVO_LOG");
	configuracion->algoritmoPlanificacion = config_get_string_value(fd_configuracion, "ALGORITMO_PLANIFICACION");
	configuracion->nombreRestaurante = config_get_string_value(fd_configuracion, "NOMBRE_RESTAURANTE");


	log_info(logger,
		"\nPUERTO_ESCUCHA: %s\n"
		"IP_SINDICATO: %s\n"
		"PUERTO_SINDICATO: %d\n"
		"IP_APP: %s\n"
		"PUERTO_APP: %d\n"
		"QUANTUM: %d\n"
		"PATH_ARCHIVO_LOG: %s\n"
		"ALGORIMO_PLANIFICACION: %s\n"
		"NOMBRE_RESTAURANTE: %s\n",
		configuracion->puertoEscucha,
		configuracion->ipSindicato,
		configuracion->puertoSindicato,
		configuracion->ipApp,
		configuracion->puertoApp,
		configuracion->quantum,
		configuracion->pathArchivoLog,
		configuracion->algoritmoPlanificacion,
		configuracion->nombreRestaurante
		);
	return 0;
}

void limpiarConfiguracion() {
	free(configuracion);
	config_destroy(fd_configuracion);
	log_destroy(logger);

}


