
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

#include <stdint.h>
#include <dirent.h>
#include <errno.h>
#include <setjmp.h>
#include <sys/types.h>
#include <sys/stat.h>
// AMBIENTE

#define TRUE  1
#define FALSE 0

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
    SELECCIONAR_RESTAURANTE_OK          = 102,
    SELECCIONAR_RESTAURANTE_FAIL        = 202,

} cod_msg;

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
	char     nombre_comida[24]    ;
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



t_log    * logger;
t_config * config;

t_list   * g_sockets_abiertos;
int        g_socket_cliente;

// FUNCIONES

int serializar(void* buffer, const char* format, ...);
int deserializar(void* buffer, const char* format, ...);

t_list *   enviar_consultar_restaurante   (char* p_ip,char* p_puerto);
void       recibir_consultar_restaurante_y_responder ( int socket_cliente );

void       enviar_guardar_pedido   (char* p_ip,char* p_puerto);
void       enviar_obtener_pedido   (char* p_ip,char* p_puerto);
void       enviar_confirmar_pedido (char* p_ip,char* p_puerto);
void       enviar_finalizar_pedido (char* p_ip,char* p_puerto);
t_header * serializar_pedido       (uint32_t nro_msg       );
t_pedido * recibir_pedido          (void * payload         );

bool enviar_seleccionar_restaurante( char* p_ip, char* p_puerto, int p_id_process, char * p_restaurante );
void responder_seleccionar_restaurante( int socket_cliente, int p_size, void * p_paylod );

void 	          enviar_guardar_plato    (char* p_ip,char* p_puerto);
t_guardar_plato * recibir_guardar_plato   (void * payload         );

void 	        enviar_plato_listo      (char* p_ip,char* p_puerto);
t_plato_listo *	recibir_plato_listo     (void * payload         );

void       prueba_biblioteca_compartida   (void                   );

int        crear_socket_y_conectar        (char* ip, char* puerto   );
int        crear_socket_escucha           ( char * p_ip, char * p_puerto );
uint32_t   aceptar_conexion               ( uint32_t p_socket_para_escuchar   );
int        recibir_confirmacion           ( int   socket_cliente  );
int        detectar_comando               ( char * p_comando      );

int     detectar_comando               ( char * p_comando      );
char *  nro_comando_a_texto            ( int    p_comando      );
int     detectar_modulo                ( char * p_modulo       );
char *  nro_modulo_a_texto             ( int    p_modulo       );

bool       enviar_buffer            ( int p_conexion, t_header * p_header );
t_header * recibir_buffer           ( int socket_cliente );





















