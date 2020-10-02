
#include <stdarg.h>


#include <stdint.h>


#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/memory.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <pthread.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>

#include <stdint.h>
#include <dirent.h>
#include <errno.h>
#include <setjmp.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <semaphore.h>

// AMBIENTE

#define TRUE  1
#define FALSE 0
#define SIZE_VECTOR_NOMBRE_PLATO 24

typedef enum {

    CLIENTE     = 1,
    APP         = 2,
    COMANDA     = 3,
	RESTAURANTE = 4,
    SINDICATO   = 5,

} cod_mod;

typedef enum {

    CONSULTAR_RESTAURANTES              =  1,
    SELECCIONAR_RESTAURANTE             =  2,
    OBTENER_RESTAURANTES                =  3,
    CONSULTAR_PLATOS                    =  4,
    CREAR_PEDIDO                        =  5,
    GUARDAR_PEDIDO                      =  6,
    ANIADIR_PLATO                       =  7,
    GUARDAR_PLATO                       =  8,
    CONFIRMAR_PEDIDO                    =  9,
    PLATO_LISTO                         = 10,
    CONSULTAR_PEDIDO                    = 11,
    OBTENER_PEDIDO                      = 12,
    FINALIZAR_PEDIDO                    = 13,
    TOMAR_PEDIDO                        = 14,
	OK                                  = 15,
    FAIL                                = 16,
	CONECTAR                            = 17,
	RESPUESTA_OBTENER_PEDIDO            = 18,
    SELECCIONAR_RESTAURANTE_OK          = 102,
    SELECCIONAR_RESTAURANTE_FAIL        = 202,

} cod_msg;

typedef enum{
	PENDIENTE =0,
	CONFIRMADO=1,
	TERMINADO =2,
} estado_pedido;

typedef struct {
	uint32_t    posx;
	uint32_t    posy;
	uint32_t    resto_nombre_size;
	char     *  resto_nombre;
} t_resto_conex;

typedef struct { // uint32_t modulo, id_proceso, nro_msg, size;
	uint32_t    modulo;
	uint32_t    id_proceso;
	uint32_t    nro_msg;
	uint32_t    size;
	void     *  payload;
} t_header;

typedef struct {
	uint32_t cantidad_total_comida;
	uint32_t cantidad_lista_comida;
	char     nombre_comida[SIZE_VECTOR_NOMBRE_PLATO]    ;
}t_comida;

typedef struct {  // Me sirve para guardar pedido, consultar pedido, obtener pedido y finalizar pedido
	uint32_t size_nombre_restaurante;
	char *   nombre_restaurante     ;
	uint32_t id_pedido              ;
}t_pedido;

typedef struct {
	t_pedido* pedido           ;
	uint32_t  size_nombre_plato;
	char *    nombre_plato     ;
    uint32_t  cantidad_plato   ;
}t_guardar_plato;

typedef struct {
	t_pedido* pedido           ;
	uint32_t  size_nombre_plato;
	char *    nombre_plato     ;
}t_plato_listo;

typedef struct{
	uint32_t		size_nombre_restaurante;
	char		* 	nombre_restaurante;
}t_solicitud_info_restaurante;

typedef struct{
	uint32_t		cantidad_cocineros;
	uint32_t		posicion_x;
	uint32_t		posicion_y;
	uint32_t		size_afinidad_cocineros;
	char		*	afinidad_cocineros;
	uint32_t		size_platos;
	char		*	platos;
	uint32_t		size_precio_platos;
	char		*	precio_platos;
	uint32_t		cantidad_hornos;
}t_respuesta_info_restaurante;


t_log    * logger;
t_config * config;

t_list   * g_sockets_abiertos;
uint32_t   g_socket_cliente;

uint32_t   g_tiempo_reconexion;

// FUNCIONES

int serializar(void* buffer, const char* format, ...);
int deserializar(void* buffer, const char* format, ...);

t_list * enviar_01_consultar_restaurantes   (char* p_ip,char* p_puerto);
void responder_01_consultar_restaurantes ( uint32_t socket_cliente, t_list * p_list_restaurantes );

int        enviar_guardar_pedido   (char* p_ip,char* p_puerto);//serializa pedido, le agrega el nro de mensaje y lo envia
int        enviar_obtener_pedido   (char* p_ip,char* p_puerto);//                 ""
int       enviar_confirmar_pedido (char* p_ip,char* p_puerto);//                 ""
void       enviar_finalizar_pedido (char* p_ip,char* p_puerto);//                 ""
t_header * serializar_pedido       (uint32_t nro_msg         );//Serializa id pedido, size nombre restaurant, nombre restaurant
t_pedido * recibir_pedido          (void * payload           );//Deserializa id pedido, size nombre restaurant, nombre restaurant
void       deserializar_respuesta_obtener_pedido(t_header *  );//Deserializa y muestra toda la informacion de un pedido


bool enviar_seleccionar_restaurante( char* p_ip, char* p_puerto, int p_id_process, char * p_restaurante );
void responder_seleccionar_restaurante( int socket_cliente, bool seleccionado );

t_list * enviar_04_consultar_platos( char* p_ip, char* p_puerto, uint32_t p_id_process );
void responder_04_consultar_platos( uint32_t socket_cliente, char ** p_platos );

uint32_t enviar_05_crear_pedido( char* p_ip, char* p_puerto, uint32_t p_id_process );
void responder_05_crear_pedido( uint32_t socket_cliente, uint32_t p_id_pedido_creado );

bool enviar_07_aniadir_plato( char * p_ip, char * p_puerto, uint32_t p_id_proceso, uint32_t p_id_pedido, char * p_plato );
void responder_07_aniadir_plato( uint32_t socket_cliente, bool p_resultado );

bool enviar_09_confirmar_pedido ( char* p_ip, char* p_puerto, uint32_t p_id_process );
void responder_09_confirmar_pedido ( uint32_t socket_cliente, bool p_resultado );

int 	          enviar_guardar_plato    (char* p_ip,char* p_puerto);
t_guardar_plato * recibir_guardar_plato   (void * payload         );

int  	        enviar_plato_listo      (char* p_ip,char* p_puerto);
t_plato_listo *	recibir_plato_listo     (void * payload         );

void       prueba_biblioteca_compartida   (void                   );

uint32_t   crear_socket_y_conectar        (char* ip, char* puerto   );
uint32_t   crear_socket_escucha           ( char * p_ip, char * p_puerto );
uint32_t   aceptar_conexion               ( uint32_t p_socket_para_escuchar   );
uint32_t   recibir_confirmacion           ( uint32_t socket_cliente  );
uint32_t   detectar_comando               ( char *   p_comando       );

uint32_t  detectar_comando               ( char *   p_comando     );
char *    nro_comando_a_texto            ( uint32_t p_comando     );
uint32_t  detectar_modulo                ( char *   p_modulo      );
char *    nro_modulo_a_texto             ( uint32_t p_modulo      );


t_header * serializar_respuesta_info_restaurante(t_respuesta_info_restaurante * respuesta_info);

bool       enviar_buffer            ( uint32_t p_conexion, t_header * p_header );
t_header * recibir_buffer           ( uint32_t socket_cliente );

void sigint(int a);

uint32_t random_id_generator( void );

void _string_destroyer( void * );




















