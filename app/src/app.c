
#include "app.h"

int main(void) {

	prueba_biblioteca_compartida();

	logger = log_create("app.log","APP",1,LOG_LEVEL_INFO);
	config = leer_config();

	int socket_cliente = crear_socket_escucha("127.0.0.1", g_puerto_escucha );

	int socket_aceptado = aceptar_conexion(socket_cliente);

	if ( socket_aceptado > 0) recibir_consultar_restaurante_y_responder(socket_aceptado);

	close(socket_aceptado);

	return EXIT_SUCCESS;

}

t_config * leer_config(void) {

	t_config * config = config_create("app.config");

	if ( config_has_property( config, "IP_COMANDA"                  ) ) g_ip_comanda                  = config_get_string_value(config, "IP_COMANDA");
	if ( config_has_property( config, "PUERTO_COMANDA"              ) ) g_puerto_comanda              = config_get_int_value(config, "PUERTO_COMANDA");
	if ( config_has_property( config, "PUERTO_ESCUCHA"              ) ) g_puerto_escucha              = config_get_string_value(config, "PUERTO_ESCUCHA");
	if ( config_has_property( config, "RETARDO_CICLO_CPU"           ) ) g_retardo_ciclo_cpu           = config_get_int_value(config, "RETARDO_CICLO_CPU");
	if ( config_has_property( config, "GRADO_DE_MULTIPROCESAMIENTO" ) ) g_grado_de_multiprocesamiento = config_get_int_value(config, "GRADO_DE_MULTIPROCESAMIENTO");

	if ( config_has_property( config, "ALGORITMO_DE_PLANIFICACION"  ) ) g_algoritmo_de_planificacion  = config_get_int_value(config, "ALGORITMO_DE_PLANIFICACION");
	if ( config_has_property( config, "ALPHA"                       ) ) g_alpha                       = config_get_int_value(config, "ALPHA");
	if ( config_has_property( config, "ESTIMACION_INICIAL"          ) ) g_estimacion_inicial          = config_get_int_value(config, "ESTIMACION_INICIAL");
	if ( config_has_property( config, "REPARTIDORES"                ) ) g_repartidores                = config_get_array_value(config, "REPARTIDORES");
	if ( config_has_property( config, "FRECUENCIA_DE_DESCANSO"      ) ) g_frecuencia_de_descanso      = config_get_array_value(config, "FRECUENCIA_DE_DESCANSO");

	if ( config_has_property( config, "TIEMPO_DE_DESCANSO"          ) ) g_tiempo_de_descanso          = config_get_array_value(config, "TIEMPO_DE_DESCANSO");
	if ( config_has_property( config, "ARCHIVO_LOG"                 ) ) g_log_path                    = config_get_string_value(config, "ARCHIVO_LOG");
	if ( config_has_property( config, "PLATOS_DEFAULT"              ) ) g_platos_default              = config_get_array_value(config, "PLATOS_DEFAULT");
	if ( config_has_property( config, "POSICION_REST_DEFAULT_X"     ) ) g_posicion_rest_default_x     = config_get_int_value(config, "POSICION_REST_DEFAULT_X");
	if ( config_has_property( config, "POSICION_REST_DEFAULT_Y"     ) ) g_posicion_rest_default_y     = config_get_int_value(config, "POSICION_REST_DEFAULT_Y");

	return config;

}




