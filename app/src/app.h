/*
 * app.h
 *
 *  Created on: 3 sep. 2020
 *      Author: utnso
 */

#ifndef APP_H_
#define APP_H_

#include "serializar.h"

// AMBIENTE

char *  g_ip_comanda;                    // IP_COMANDA=127.0.0.1
int     g_puerto_comanda;                // PUERTO_COMANDA=5001
int     g_puerto_escucha;                // PUERTO_ESCUCHA=5004
int     g_retardo_ciclo_cpu;             // RETARDO_CICLO_CPU
int     g_grado_de_multiprocesamiento;   // GRADO_DE_MULTIPROCESAMIENTO=1

int     g_algoritmo_de_planificacion;    // ALGORITMO_DE_PLANIFICACION=FIFO
int     g_alpha;                         // ALPHA=0,5
int     g_estimacion_inicial;            // ESTIMACION_INICIAL=2
char ** g_repartidores;                  // REPARTIDORES=3
char ** g_frecuencia_de_descanso;        // FRECUENCIA_DE_DESCANSO=1

char ** g_tiempo_de_descanso;            // TIEMPO_DE_DESCANSO=5
char *  g_log_path;                      // ARCHIVO_LOG=app.log
char *  g_platos_default;                // PLATOS_DEFAULT=3
char *  g_posicion_rest_default_x;       // POSICION_REST_DEFAULT_X=0
char *  g_posicion_rest_default_y;       // POSICION_REST_DEFAULT_Y=0

// FUNCIONES

t_config * leer_config         ( void );

void       escuchar_cliente    ( int socket_cliente );



#endif /* APP_H_ */
