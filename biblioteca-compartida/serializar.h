
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
#include <commons/log.h>
#include <commons/string.h>
#include <commons/collections/list.h>

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <setjmp.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
// AMBIENTE

typedef enum {

    CLIENTE     = 1,
    APP         = 2,
    COMANDA     = 3,
	RESTAURANTE = 4,
    SINDICATO   = 5,

} cod_mod;

typedef enum {

    CONSULTAR_RESTAURANTES  =  1,
    SELECCIONAR_RESTAURANTE =  2,
    OBTENER_RESTAURANTES    =  3,
    CONSULTAR_PLATOS        =  4,
    CREAR_PEDIDO            =  5,
    GUARDAR_PEDIDO          =  6,
    ANIADIR_PLATO           =  7,
    GUARDAR_PLATO           =  8,
    CONFIRMAR_PEDIDO        =  9,
    PLATO_LISTO             = 10,
    CONSULTAR_PEDIDO        = 11,
    OBTENER_PEDIDO          = 12,
    FINALIZAR_PEDIDO        = 13,
    TOMAR_PEDIDO            = 14,

} cod_msg;

typedef struct { // uint32_t modulo, id_proceso, nro_msg, size;
	int    modulo;
    int    id_proceso;
    int    nro_msg;
    int    size;
	void * payload;
} t_header;


t_log    * logger;
t_config * config;

// FUNCIONES

int serializar(void* buffer, const char* format, ...);
int serializar(void* buffer, const char* format, ...);

t_list * enviar_consultar_restaurante   (char* p_ip,int p_puerto);
void     recibir_consultar_restaurante_y_responder ( int socket_cliente );

void     prueba_biblioteca_compartida   (void                   );

int      crear_socket_y_conectar        (char* ip, int puerto   );

int      crear_socket_escucha           ( char * p_ip, int p_puerto );
int      aceptar_conexion               ( int p_socket_para_escuchar );

int      recibir_confirmacion           ( int   socket_cliente  );
int      detectar_comando               ( char * p_comando      );

bool       enviar_buffer            ( int p_conexion, t_header * p_header );
t_header * recibir_buffer           ( int socket_cliente );





















