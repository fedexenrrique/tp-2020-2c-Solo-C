#include "sindicato.h"
int configValida(t_config* fd_configuracion) {
	return (config_has_property(fd_configuracion, "PUERTO_ESCUCHA")
		&& config_has_property(fd_configuracion, "PUNTO_MONTAJE"));
}

int cargarConfiguracion() {
	logger = log_create("LogCoordinador", "Coordinador", true, LOG_LEVEL_INFO);
	configuracion = malloc(sizeof(tConfiguracion));


	//en eclipse cambia el path desde donde se corre, asi que probamos desde /Debug y desde /Coordinador
	fd_configuracion = config_create("../sindicato.conf");
	if (fd_configuracion == NULL) {
		fd_configuracion = config_create("sindicato.conf");
	}

	if (fd_configuracion == NULL || !configValida(fd_configuracion)) {
		log_error(logger,"Archivo de configuración inválido.","ERROR");
		return -1;
	}
	configuracion->puertoEscucha = config_get_int_value(fd_configuracion, "PUERTO_ESCUCHA");
	configuracion->puntoMontaje = config_get_string_value(fd_configuracion, "PUNTO_MONTAJE");

	log_info(logger,
		"\nPUERTO_ESCUCHA: %d\n"
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
