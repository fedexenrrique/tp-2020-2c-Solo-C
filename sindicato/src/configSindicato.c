#include "sindicato.h"
int configValida(t_config* fd_configuracion) {
	return (config_has_property(fd_configuracion, "PUERTO_ESCUCHA")
		&& config_has_property(fd_configuracion, "PUNTO_MONTAJE"));
}

int cargarConfiguracion() {
	logger = log_create("LogSindicato", "Sindicato", true, LOG_LEVEL_INFO);
	configuracion = malloc(sizeof(tConfiguracion));
	diccionarioBloquesAsignadosARestos= dictionary_create();
	diccionarioBloquesAsignadosARecetas=dictionary_create();
	diccionarioBloquesAsignadosAPedidos=dictionary_create();
	diccionarioPosPropiedadesEnArchivo=dictionary_create();
	fd_configuracion = config_create("../sindicato.conf");
	pathAbsolutoBloquesAsignadosARestos=malloc(strlen("bloquesAsignadosARestos.bin")+1);
	pathAbsolutoBloquesAsignadosARecetas=malloc(strlen("bloquesAsignadosARecetas.bin")+1);
	pathAbsolutoBloquesAsignadosAPedidos=malloc(strlen("bloquesAsignadosAPedidos.bin")+1);
	memcpy(pathAbsolutoBloquesAsignadosARestos,"bloquesAsignadosARestos.bin",strlen("bloquesAsignadosARestos.bin"));
	memcpy(pathAbsolutoBloquesAsignadosARecetas,"bloquesAsignadosARecetas.bin",strlen("bloquesAsignadosARecetas.bin"));
	memcpy(pathAbsolutoBloquesAsignadosAPedidos,"bloquesAsignadosPedidos.bin",strlen("bloquesAsignadosAPedidos.bin"));
	pathAbsolutoBloquesAsignadosARestos[strlen("bloquesAsignadosARestos.bin")]='\0';
	pathAbsolutoBloquesAsignadosARecetas[strlen("bloquesAsignadosARecetas.bin")]='\0';
	pathAbsolutoBloquesAsignadosAPedidos[strlen("bloquesAsignadosAPedidos.bin")]='\0';

	if (fd_configuracion == NULL) {
		fd_configuracion = config_create("sindicato.conf");
	}

	if (fd_configuracion == NULL || !configValida(fd_configuracion)) {
		log_error(logger,"Archivo de configuración inválido.","ERROR");
		return -1;
	}
	configuracion->puertoEscucha = config_get_string_value(fd_configuracion, "PUERTO_ESCUCHA");
	configuracion->puntoMontaje = config_get_string_value(fd_configuracion, "PUNTO_MONTAJE");

	int offsetPedidos=0;
	int offsetRestos=0;
	int offsetRecetas=0;
	pathAbsolutoBloquesAsignadosARestos=malloc(strlen(configuracion->puntoMontaje)+1+strlen("bloquesAsignadosARestos.bin")+1);
	pathAbsolutoBloquesAsignadosARecetas=malloc(strlen(configuracion->puntoMontaje)+1+strlen("bloquesAsignadosARecetas.bin")+1);
	pathAbsolutoBloquesAsignadosAPedidos=malloc(strlen(configuracion->puntoMontaje)+1+strlen("bloquesAsignadosAPedidos.bin")+1);

	memcpy(pathAbsolutoBloquesAsignadosARestos,configuracion->puntoMontaje,strlen(configuracion->puntoMontaje));
	offsetRestos+=strlen(configuracion->puntoMontaje);
	memcpy(pathAbsolutoBloquesAsignadosARestos+offsetRestos,"/",1);
	offsetRestos++;
	memcpy(pathAbsolutoBloquesAsignadosARestos+offsetRestos,"bloquesAsignadosARestos.bin",strlen("bloquesAsignadosARestos.bin"));
	offsetRestos+=strlen("bloquesAsignadosARestos.bin");
	pathAbsolutoBloquesAsignadosARestos[offsetRestos]='\0';

	memcpy(pathAbsolutoBloquesAsignadosARecetas,configuracion->puntoMontaje,strlen(configuracion->puntoMontaje));
	offsetRecetas+=strlen(configuracion->puntoMontaje);
	memcpy(pathAbsolutoBloquesAsignadosARecetas+offsetRecetas,"/",1);
	offsetRecetas++;
	memcpy(pathAbsolutoBloquesAsignadosARecetas+offsetRecetas,"bloquesAsignadosARecetas.bin",strlen("bloquesAsignadosARecetas.bin"));
	offsetRecetas+=strlen("bloquesAsignadosARecetas.bin");
	pathAbsolutoBloquesAsignadosARecetas[offsetRecetas]='\0';

	memcpy(pathAbsolutoBloquesAsignadosAPedidos,configuracion->puntoMontaje,strlen(configuracion->puntoMontaje));
	offsetPedidos+=strlen(configuracion->puntoMontaje);
	memcpy(pathAbsolutoBloquesAsignadosAPedidos+offsetPedidos,"/",1);
	offsetPedidos++;
	memcpy(pathAbsolutoBloquesAsignadosAPedidos+offsetPedidos,"bloquesAsignadosAPedidos.bin",strlen("bloquesAsignadosAPedidos.bin"));
	offsetPedidos+=strlen("bloquesAsignadosAPedidos.bin");
	pathAbsolutoBloquesAsignadosAPedidos[offsetPedidos]='\0';


	log_info(logger,
			"\nPUERTO_ESCUCHA: %s\n"
			"PUNTO_MONTAJE: %s\n",

			configuracion->puertoEscucha,
			configuracion->puntoMontaje);
	return 0;
}

void limpiarConfiguracion() {
	free(configuracion);
	config_destroy(fd_configuracion);
	log_destroy(logger);

}
