
#ifndef CLIENTE_H_
#define CLIENTE_H_

#include <serializar.h>

// AMBIENTE

char * g_ip_comanda;         // IP_COMANDA=127.0.0.1
int    g_puerto_comanda;     // PUERTO_COMANDA=5001
char * g_ip_restaurante;     // IP_RESTAURANTE=127.0.0.1
int    g_puerto_restaurante; // PUERTO_RESTAURANTE=5002
char * g_ip_sindicato;       // IP_SINDICATO=127.0.0.1
int    g_puerto_sindicato;   // PUERTO_SINDICATO=5003
char * g_ip_app;             // IP_APP=127.0.0.1
char * g_puerto_app;         // PUERTO_APP=5004
char * g_log_path;           // ARCHIVO_LOG=cliente.log
int    g_posicion_x;         // POSICION_X=1
int    g_posicion_y;         // POSICION_Y=2
int    g_id_proceso;         // ID_PROCESO=100

// FUNCIONES

t_config *  leer_config     ( void             );
void        listar_comandos ( void             );


#endif /* CLIENTE_H_ */
