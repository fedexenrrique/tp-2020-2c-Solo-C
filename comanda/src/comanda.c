/*
 ============================================================================
 Name        : comanda.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "comanda.h"

int main(void) {

	int servidor_comanda=iniciar_comanda();
	signal(SIGUSR1, iniciar_dump_cache);


	while(1) {

		pthread_t  receptor_modulo_cliente_h;

		int socket_cliente = aceptar_conexion(servidor_comanda);

		if ( socket_cliente > 0)

			pthread_create(&receptor_modulo_cliente_h,NULL,(void*)&manejo_modulo_conectado,(void*)(&socket_cliente));

	}


	return EXIT_SUCCESS;
}
