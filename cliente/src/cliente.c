/*
 ============================================================================
 Name        : cliente.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "cliente.h"

int main(void) {

	prueba_biblioteca_compartida();

	logger = log_create("cliente.log","CLIENTE",1,LOG_LEVEL_INFO);
	config = leer_config();


	return EXIT_SUCCESS;

}

t_config * leer_config(void) {

	t_config* config = config_create("cliente.config");

	if ( config_has_property( config, "IP_COMANDA"         ) )  g_ip_comanda         = config_get_string_value(config, "IP_COMANDA");
	if ( config_has_property( config, "PUERTO_COMANDA"     ) )  g_puerto_comanda     = config_get_int_value   (config, "PUERTO_COMANDA");

	if ( config_has_property( config, "IP_RESTAURANTE"     ) )  g_ip_restaurante     = config_get_string_value(config, "IP_RESTAURANTE");
	if ( config_has_property( config, "PUERTO_RESTAURANTE" ) )  g_puerto_restaurante = config_get_int_value   (config, "PUERTO_RESTAURANTE");

	if ( config_has_property( config, "IP_SINDICATO"       ) )  g_ip_sindicato       = config_get_string_value(config, "IP_SINDICATO");
	if ( config_has_property( config, "PUERTO_SINDICATO"   ) )  g_puerto_sindicato   = config_get_int_value   (config, "PUERTO_SINDICATO");

	if ( config_has_property( config, "IP_APP"             ) )  g_ip_app             = config_get_string_value(config, "IP_APP");
	if ( config_has_property( config, "PUERTO_APP"         ) )  g_puerto_app         = config_get_int_value   (config, "PUERTO_APP");

	if ( config_has_property( config, "ARCHIVO_LOG"        ) )  g_log_path           = config_get_string_value(config, "ARCHIVO_LOG");
	if ( config_has_property( config, "POSICION_X"         ) )  g_posicion_x         = config_get_int_value   (config, "POSICION_X");
	if ( config_has_property( config, "POSICION_Y"         ) )  g_posicion_y         = config_get_int_value   (config, "POSICION_Y");

	return config;

}
