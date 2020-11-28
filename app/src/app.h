/*
 * app.h
 *
 *  Created on: 3 sep. 2020
 *      Author: utnso
 */

#ifndef APP_H_
#define APP_H_

#include <serializar.h>

#include <commons/log.h>
#include <commons/config.h>
#include <commons/memory.h>
#include "commons/collections/list.h"

t_log    * logger;
t_config * config;

// AMBIENTE

char *       g_ip_comanda;                    // IP_COMANDA=127.0.0.1
char *       g_puerto_comanda;                // PUERTO_COMANDA=5001
char *       g_puerto_escucha;                // PUERTO_ESCUCHA=5004
uint32_t     g_retardo_ciclo_cpu;             // RETARDO_CICLO_CPU
uint32_t     g_grado_de_multiprocesamiento;   // GRADO_DE_MULTIPROCESAMIENTO=1

char *       g_algoritmo_de_planificacion;    // ALGORITMO_DE_PLANIFICACION=FIFO
uint32_t     g_alpha;                         // ALPHA=0,5
uint32_t     g_estimacion_inicial;            // ESTIMACION_INICIAL=2
char **      g_repartidores;                  // REPARTIDORES=3
char **      g_frecuencia_de_descanso;        // FRECUENCIA_DE_DESCANSO=1

char **      g_tiempo_de_descanso;            // TIEMPO_DE_DESCANSO=5
char *       g_log_path;                      // ARCHIVO_LOG=app.log
char **      g_platos_default;                // PLATOS_DEFAULT=3
uint32_t     g_posicion_rest_default_x;       // POSICION_REST_DEFAULT_X=0
uint32_t     g_posicion_rest_default_y;       // POSICION_REST_DEFAULT_Y=0

uint32_t   g_generador_id_repartidor;

pthread_t  g_thread_long_term_scheduler;
pthread_t  g_thread_medium_term_scheduler;
pthread_t  g_thread_short_term_scheduler;

sem_t g_nro_cpus ;

typedef enum {

	SIN_CONFIRMAR     = 1,
	PENDIENTE_PLANIF  = 2,
    NUEVO             = 3,
    LISTO             = 4,
	EJEC              = 5,
    BLOQ              = 6,
	FINAL             = 7,

} enum_estado;

typedef struct {
	uint32_t    resto_x;
	uint32_t    resto_y;
	char*       resto_nombre;
	uint32_t    socket_conectado;
	char**      list_platos;
} t_info_restarante;

t_list * lista_clientes; // t_cliente_resto

t_queue * queue_confirmados_cliente_resto; // t_cliente_a_resto

t_list * lista_resto_conectados; // t_info_restarante

typedef struct { // uint32_t modulo, id_proceso, nro_msg, size;
	uint32_t            id_cliente;
	uint32_t            cliente_x;
	uint32_t            cliente_y;
	char * 				nombre_resto;
	uint32_t            id_pedido;
	uint32_t            resto_x;
	uint32_t            resto_y;
} t_cliente_a_resto;

typedef struct { // uint32_t modulo, id_proceso, nro_msg, size;
	uint32_t cantidad_plato;
	char *   nombre_plato;
} t_elem_pedido;

typedef enum {

	RESTO  = 100,
	CLI    = 101,

} e_yendo_a;

typedef struct { // g_cola_nuevos, g_cola_listos, g_cola_bloqueados

	pthread_t   thread_metadata;
	uint32_t    id_repartidor;
	uint32_t    repa_x;
	uint32_t    repa_y;
	e_yendo_a   yendo_a;
	uint32_t    freq_de_descanso;
	uint32_t    tiempo_de_descanso;
	uint32_t    cansancio;
	sem_t       semaforo;
	sem_t       sem_bloq;
	enum_estado estado;
	uint32_t    id_cliente;
	uint32_t    cliente_x;
	uint32_t    cliente_y;
	char * 		nombre_resto;
	uint32_t    id_pedido;
	uint32_t    resto_x;
	uint32_t    resto_y;
	sem_t       cpu;
	sem_t       bloq;

} t_pcb_repartidor;

t_queue * g_cola_nuevos;
t_queue * g_cola_listos;
t_list  * g_lista_listos;
t_queue * g_cola_bloqueados;

sem_t    sem_nuevos;
sem_t    sem_listos;
sem_t    sem_bloq;

// FUNCIONES

t_config * leer_config         ( void );

void       escuchar_cliente    ( int socket_cliente );

t_list   * obtener_restaurante_hardcodeado();

void bucle_resto_conectado ( uint32_t sock_aceptado );

void long_term_scheduler( void );

void short_term_scheduler( void );

void medium_term_scheduler( void );

void descanso_repartidor ( t_pcb_repartidor * p_pcb );

void ejecucion_repartidor ( t_pcb_repartidor * p_pcb );

bool procedimiento_02_seleccionar_restaurante( t_header * header_recibido );

char ** procedimiento_04_consultar_platos( t_header * header_recibido );

uint32_t procedimiento_05_crear_pedido( t_header * header_recibido );

bool procesamiento_07_aniadir_plato( t_header * header_recibido );

bool procesamiento_09_confirmar_pedido ( t_header * header_recibido );

bool procesamiento_09_confirmar_pedido_hack ( void );

void agregar_pedid_a_planificacion (t_cliente_a_resto * asociacion);

void auxiliar_aniadir_plato ( t_list * p_list_platos, uint32_t p_cant_plato, char * p_nom_plato );

void procesamiento_mensaje( void * socket_cliente );
void mostrar_info_pcb_repartidor ( t_pcb_repartidor * p_repa );

void _aux_long_term_scheduler(void);

void planificar_fifo(t_pcb_repartidor * p_pcb);

void sigint(int a);

#endif /* APP_H_ */
