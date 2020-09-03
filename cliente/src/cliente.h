
#ifndef CLIENTE_H_
#define CLIENTE_H_

#include <serializar.h>

// AMBIENTE

t_log    * logger;
t_config * config;

char * g_ip_comanda;         // IP_COMANDA=127.0.0.1
int    g_puerto_comanda;     // PUERTO_COMANDA=5001
char * g_ip_restaurante;     // IP_RESTAURANTE=127.0.0.1
int    g_puerto_restaurante; // PUERTO_RESTAURANTE=5002
char * g_ip_sindicato;       // IP_SINDICATO=127.0.0.1
int    g_puerto_sindicato;   // PUERTO_SINDICATO=5003
char * g_ip_app;             // IP_APP=127.0.0.1
int    g_puerto_app;         // PUERTO_APP=5004
char * g_log_path;           // ARCHIVO_LOG=/utnso/logs/cliente.log
int    g_posicion_x;         // POSICION_X=1
int    g_posicion_y;         // POSICION_Y=2


		printf("\nDebe ingresar un comando para comicarse con los módulos.");
		printf("\nLos comandos permitidos son los siguientes:\n");
		printf(" 01- CONSULTAR_RESTAURANTES  HACIA: APP                                  \n");
		printf(" 02- SELECCIONAR_RESTAURANTE HACIA: APP                                  \n");
		printf(" 03- OBTENER_RESTAURANTES    HACIA: SINDICATO                            \n");
		printf(" 04- CONSULTAR_PLATOS        HACIA: APP, RESTAURANTE, SINDICATO          \n");
		printf(" 05- CREAR_PEDIDO            HACIA: APP, RESTAURANTE                     \n");
		printf(" 06- GUARDAR_PEDIDO          HACIA: COMANDA, SINDICATO                   \n");
		printf(" 07- AÑADIR_PLATO            HACIA: APP, RESTAURANTE                     \n");
		printf(" 08- GUARDAR_PLATO           HACIA: COMANDA, SINDICATO                   \n");
		printf(" 10- CORFIRMAR_PEDIDO        HACIA: APP, RESTAURANTE, COMANDA, SINDICATO \n");
		printf(" 11- PLATO_LISTO             HACIA: APP, COMANDA, SINDICATO              \n");
		printf(" 12- CONSULTAR_PEDIDO        HACIA: APP, RESTAURANTE                     \n");
		printf(" 13- OBTENER_PEDIDO          HACIA: COMANDA, SINDICATO                   \n");
		printf(" 14- FINALIZAR_PEDIDO        HACIA: COMANDA, CLIENTE                     \n");
		printf(" 15- TOMAR_PEDIDO            HACIA: COMANDA, CLIENTE                     \n");


// FUNCIONES

t_config * leer_config(void);


#endif /* CLIENTE_H_ */
