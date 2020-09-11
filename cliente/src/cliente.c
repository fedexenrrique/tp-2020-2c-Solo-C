/*
 ============================================================================
 Name        : cliente.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "cliente.h"

int main(int argc, char **argv) {

	logger = log_create("cliente.log","CLIENTE",1,LOG_LEVEL_INFO);
	config = leer_config();

	if ( argc == 1 ) {

		listar_comandos();

		return EXIT_SUCCESS;

	} else if ( argc > 2 ) {

		printf("\nUsted ingresó el módulo: %s.\n", argv[1]);

		switch( detectar_modulo( argv[1] ) ) {

		case APP: // 01- CONSULTAR_RESTAURANTES  HACIA: APP

			printf(" MÓDULO DESTINO: APP                                  \n");

			switch( detectar_comando( argv[2] ) ) {
		    	
				case CONSULTAR_RESTAURANTES: // 01- CONSULTAR_RESTAURANTES  HACIA: APP

					printf(" 01- CONSULTAR_RESTAURANTES  HACIA: APP \n");
    		    	enviar_consultar_restaurante(g_ip_app, g_puerto_app);
    		    	break;
    		    
				case SELECCIONAR_RESTAURANTE:
    		    	printf(" 02- SELECCIONAR_RESTAURANTE HACIA: APP \n");
		    		break;
    		    
				case CONSULTAR_PLATOS:
    		    	printf(" 04- CONSULTAR_PLATOS HACIA: APP \n");
		    		break;

				case CREAR_PEDIDO:
    		    	printf(" 05- CREAR_PEDIDO HACIA: SINDICATO \n");
		    		break;
    		    
				case ANIADIR_PLATO:
    		    	printf(" 07- ANIADIR_PLATO HACIA: SINDICATO \n");
		    		break;

				case CONFIRMAR_PEDIDO:
    		    	printf(" 09- CONFIRMAR_PEDIDO HACIA: SINDICATO \n");
		    		break;

				case PLATO_LISTO:
    		    	printf(" 10- PLATO_LISTO HACIA: SINDICATO \n");
		    		break;

				case CONSULTAR_PEDIDO:
    		    	printf(" 11- CONSULTAR_PEDIDO HACIA: SINDICATO \n");
		    		break;

				default:
    		    	log_error(logger, "Comando no válido para el módulo APP.");
					break;

			}

			break;

		case COMANDA:

			printf(" MÓDULO DESTINO: COMANDA                              \n");
			break;

		case RESTAURANTE:

			printf(" MÓDULO DESTINO: RESTAURANTE                          \n");
			break;

		case SINDICATO:

			printf(" MÓDULO DESTINO: SINDICATO                            \n");
			break;

		default:

			log_error(logger, "Módulo inválido.");

		}

	} else {

		listar_comandos();

		return EXIT_SUCCESS;

	}

	return EXIT_SUCCESS;

}

t_config * leer_config(void) {

	t_config* config = config_create("cliente.config");

	if ( config_has_property( config, "IP_COMANDA"         ) )  g_ip_comanda         = config_get_string_value(config, "IP_COMANDA");
	if ( config_has_property( config, "PUERTO_COMANDA"     ) )  g_puerto_comanda     = config_get_int_value   (config, "PUERTO_COMANDA");

	if ( config_has_property( config, "IP_RESTAURANTE"     ) )  g_ip_restaurante     = config_get_string_value(config, "IP_RESTAURANTE");
	if ( config_has_property( config, "PUERTO_RESTAURANTE" ) )  g_puerto_restaurante = config_get_int_value   (config, "PUERTO_RESTAURANTE");

	if ( config_has_property( config, "IP_SINDICATO"       ) )  g_ip_sindicato       = config_get_string_value(config, "IP_SINDICATO");
	if ( config_has_property( config, "PUERTO_SINDICATO"   ) )  g_puerto_sindicato   = config_get_int_value   (config, "PUERTO_SINDICATO");

	if ( config_has_property( config, "IP_APP"             ) )  g_ip_app             = config_get_string_value(config, "IP_APP");
	if ( config_has_property( config, "PUERTO_APP"         ) )  g_puerto_app         = config_get_int_value   (config, "PUERTO_APP");

	if ( config_has_property( config, "ARCHIVO_LOG"        ) )  g_log_path           = config_get_string_value(config, "ARCHIVO_LOG");
	if ( config_has_property( config, "POSICION_X"         ) )  g_posicion_x         = config_get_int_value   (config, "POSICION_X");
	if ( config_has_property( config, "POSICION_Y"         ) )  g_posicion_y         = config_get_int_value   (config, "POSICION_Y");

	return config;

}

void listar_comandos(void) {

	printf("\nDebe ingresar un comando para comicarse con los módulos.               \n");
	printf("\nLos comandos permitidos son los siguientes:                            \n");
	printf(" 01- CONSULTAR_RESTAURANTES  HACIA: APP                                  \n");
	printf(" 02- SELECCIONAR_RESTAURANTE HACIA: APP                                  \n");
	printf(" 03- OBTENER_RESTAURANTES    HACIA: SINDICATO                            \n");
	printf(" 04- CONSULTAR_PLATOS        HACIA: APP, RESTAURANTE, SINDICATO          \n");
	printf(" 05- CREAR_PEDIDO            HACIA: APP, RESTAURANTE                     \n");
	printf(" 06- GUARDAR_PEDIDO          HACIA: COMANDA, SINDICATO                   \n");
	printf(" 07- ANIADIR_PLATO           HACIA: APP, RESTAURANTE                     \n");
	printf(" 08- GUARDAR_PLATO           HACIA: COMANDA, SINDICATO                   \n");
	printf(" 09- CONFIRMAR_PEDIDO        HACIA: APP, RESTAURANTE, COMANDA, SINDICATO \n");
	printf(" 10- PLATO_LISTO             HACIA: APP, COMANDA, SINDICATO              \n");
	printf(" 11- CONSULTAR_PEDIDO        HACIA: APP, RESTAURANTE                     \n");
	printf(" 12- OBTENER_PEDIDO          HACIA: COMANDA, SINDICATO                   \n");
	printf(" 13- FINALIZAR_PEDIDO        HACIA: COMANDA, CLIENTE                     \n");
	printf(" 14- TOMAR_PEDIDO            HACIA: COMANDA, CLIENTE                     \n");

}




