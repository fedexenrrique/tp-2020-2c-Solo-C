
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

    CONSULTAR_RESTAURANTES  = 01;
    SELECCIONAR_RESTAURANTE = 02;
    OBTENER_RESTAURANTES    = 03;
    CONSULTAR_PLATOS        = 04;
    CREAR_PEDIDO            = 05;
    GUARDAR_PEDIDO          = 06;
    AÑADIR_PLATO            = 07;
    GUARDAR_PLATO           = 08;
    CORFIRMAR_PEDIDO        = 09;
    PLATO_LISTO             = 10;
    CONSULTAR_PEDIDO        = 11;
    OBTENER_PEDIDO          = 12;
    FINALIZAR_PEDIDO        = 13;
    TOMAR_PEDIDO            = 14;

} cod_msg;

// FUNCIONES

int serializar(void* buffer, const char* format, ...);
int serializar(void* buffer, const char* format, ...);

void prueba_biblioteca_compartida(void);
