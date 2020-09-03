
#include "app.h"

int main(void) {

	prueba_biblioteca_compartida();

	logger = log_create("app.log","APP",1,LOG_LEVEL_INFO);
	config = leer_config();


	return EXIT_SUCCESS;

}

t_config * leer_config(void) {

	t_config* config = config_create("app.config");

	/*
int g_ip_comanda;                    // IP_COMANDA=127.0.0.1
int g_puerto_comanda;                // PUERTO_COMANDA=5001
int g_puerto_escucha;                // PUERTO_ESCUCHA=5004
int g_grado_de_multiprocesamiento;   // GRADO_DE_MULTIPROCESAMIENTO=1
int g_algoritmo_de_planificacion;    // ALGORITMO_DE_PLANIFICACION=FIFO
int g_alpha;                         // ALPHA=0,5
int g_estimacion_inicial;            // ESTIMACION_INICIAL=2
int g_repartidores;                  // REPARTIDORES=3
int g_frecuencia_de_descanso;        // FRECUENCIA_DE_DESCANSO=1
int g_tiempo_de_descanso;            // TIEMPO_DE_DESCANSO=5
int g_log_path;                      // ARCHIVO_LOG=/utnso/logs/app.log
int g_platos_default;                // PLATOS_DEFAULT=3
int g_posicion_rest_default_x;       // POSICION_REST_DEFAULT_X=0
int g_posicion_rest_default_y;       // POSICION_REST_DEFAULT_Y=0
	 */

	// if ( config_has_property( config, "ID_PROCESO" ) )

		// g_nro_no_asignado = config_get_int_value(config, "ID_PROCESO");

	return config;

}

