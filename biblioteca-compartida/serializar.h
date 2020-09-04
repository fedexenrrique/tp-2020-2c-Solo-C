
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

// FUNCIONES

int serializar(void* buffer, const char* format, ...);
int serializar(void* buffer, const char* format, ...);

void prueba_biblioteca_compartida(void);
