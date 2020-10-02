/*
 ============================================================================
 Name        : sindicato.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "sindicato.h"
//#include "/home/utnso/tp-2020-2c-Solo-C/biblioteca-compartida/serializar.h"
#define PATH_FILES "/Files/"

int paramValidos(char** parametros) {
	int cantParametros = string_length(*parametros);
	printf("Cant parametros: %d\n", cantParametros);
	if (cantParametros == 7)
		return 1;
	else
		return 0;

}

void levantarConsola() {
	printf(" ");
	printf("*****************************CONSOLA SINDICATO*******************\n");

	while (1) {
		char lineaComando[100];
		int i;
		for (i = 0; i <= strlen(lineaComando); i++) {
			lineaComando[i] = '\0';
		}
		printf("COMANDOS DISPONIBLES\n");
		printf("1 - Crear Restaurante\n");
		printf("2 - Crear Receta\n");
		fgets(lineaComando, 100, stdin);

		lineaComando[strlen(lineaComando) - 1] = '\0';
		char** substrings = string_n_split(lineaComando, 8, " ");
		char* nombreComando = substrings[0];



		if (string_equals_ignore_case(nombreComando, "crearRestaurante")) {
			log_info(logger, "Creación Restaurante");

			tCreacionRestaurante* restaurante=malloc(sizeof(tCreacionRestaurante));


			restaurante->nombreRestaurante = substrings[1];
			restaurante->cantCocineros = atoi(substrings[2]);
			restaurante->posicion = substrings[3];
			restaurante->afinidadCocineros = substrings[4];
			restaurante->platos = substrings[5];
			restaurante->preciosPlatos = substrings[6];
			restaurante->cantidadHornos = atoi(substrings[7]);

			log_info(logger,"Creando Reestaurante...");
			log_info(logger,"Nombre: %s\n", restaurante->nombreRestaurante);
			log_info(logger,"Cantidad Cocineros: %d\n", restaurante->cantCocineros);

			int archivoGrabado=grabarArchivoRestaurante(restaurante);

			if(archivoGrabado==0){
				log_error(logger,"Archivo no grabado");

			}

			free(restaurante);


		}

		else if (string_equals_ignore_case(nombreComando, "CrearReceta")) {
			log_info(logger, "Creacion Receta");

			tCreacionReceta* receta= malloc(sizeof(tCreacionReceta));

			receta->nombreReceta=substrings[1];
			receta->pasos=substrings[2];
			receta->tiemposPasos=substrings[3];

			log_info(logger,"Creando receta...\n");
			log_info(logger,receta->nombreReceta);
			printf(logger,receta->pasos);

			int archivoGrabado=grabarArchivoReceta(receta);

				if(archivoGrabado==0){
						log_error(logger,"Archivo no grabado");

				}

		} else
			log_error(logger, "Ingrese un comando Valido\n");
		//free(lineaComando);

		free(substrings);

	}

}
int main(int argc, char *argv[]) {

	cargarConfiguracion();
	pathFiles = string_new();
	pathBloques = string_new();
	pathMetadata = string_new();
	pathRestaurantes=string_new();
	pathRecetas=string_new();
	string_append(&pathFiles, configuracion->puntoMontaje);
	string_append(&pathFiles, "/Files/");

	string_append(&pathBloques, configuracion->puntoMontaje);
	string_append(&pathBloques, "/Bloques/");

	string_append(&pathMetadata, configuracion->puntoMontaje);
	string_append(&pathMetadata, "/Metadata/");

	string_append(&pathRestaurantes,pathFiles);
	string_append(&pathRestaurantes,"Restaurantes/");

	string_append(&pathRecetas,pathFiles);
	string_append(&pathRecetas,"Recetas/");

	t_respuesta_info_restaurante* solicitudInfoResto=malloc(sizeof(t_respuesta_info_restaurante));



	//Escucho conexiones
	log_info(logger, "Inicio Escucha de conexiones...");
	int socketServer = crear_socket_escucha("127.0.0.1",
			configuracion->puertoEscucha);

	while (1) {
		int socketConectado = aceptar_conexion(socketServer);
		handleConexion(socketConectado);

		break;

	}

	free(pathFiles);
	free(pathBloques);
	free(pathMetadata);

	return 1;
}


void handleConexion(int socketCliente) {
	log_info(logger, "Handle conexion aceptada...");
	uint32_t modulo, idProceso, nroMsg, size;

	t_header2* headerRecibido = malloc(sizeof(t_header2));

	recv(socketCliente, &modulo, sizeof(uint32_t), MSG_WAITALL);
	recv(socketCliente, &idProceso, sizeof(uint32_t), MSG_WAITALL);
	recv(socketCliente, &nroMsg, sizeof(uint32_t), MSG_WAITALL);
	recv(socketCliente, &size, sizeof(uint32_t), MSG_WAITALL);

	headerRecibido->id_proceso = idProceso;
	headerRecibido->modulo = modulo;
	headerRecibido->nro_msg = nroMsg;
	headerRecibido->size = size;
	headerRecibido->payload = malloc(headerRecibido->size);

	printf("Size payload antes recv: %d\n", strlen(headerRecibido->payload));

	if (size > 0) {
		headerRecibido->payload = malloc(size);
		recv(socketCliente, headerRecibido->payload, size, MSG_WAITALL);

	}
	printf("Size payload despues recv: %d\n", strlen(headerRecibido->payload));
	string_trim(&headerRecibido->payload);
	printf("Nombre Restaurante Recibido: %s\n", headerRecibido->payload);

	switch(headerRecibido->nro_msg){

		case OBTENER_RESTAURANTES: {
		t_header * respuesta_hardcodeada = malloc(sizeof(t_header));
		t_respuesta_info_restaurante * resp_resto_hard = malloc(sizeof(t_respuesta_info_restaurante));

		t_respuesta_info_restaurante* respuestaArchivoInfoResto= malloc(sizeof(t_respuesta_info_restaurante));

		solicitudInfoResto=leerInfoDeResto(headerRecibido->payload);

		resp_resto_hard->cantidad_cocineros = 2;
		resp_resto_hard->posicion_x = 3;
		resp_resto_hard->posicion_y = 4;
		resp_resto_hard->afinidad_cocineros = "[milanesas, otros]";
		resp_resto_hard->size_afinidad_cocineros = 18;
		resp_resto_hard->platos = "[cualquiera]";
		resp_resto_hard->size_platos = 12;
		resp_resto_hard->precio_platos = "[2,4]";
		resp_resto_hard->size_precio_platos = 5;
		resp_resto_hard->cantidad_hornos = 1;

		t_respuesta_info_restaurante* respuestInfoRestoArchivo = malloc(
				sizeof(t_header));
		respuesta_hardcodeada = serializar_respuesta_info_restaurante(
				resp_resto_hard);

		if (enviar_buffer(socketCliente, respuesta_hardcodeada) == false) {
			log_error(logger,
					"No se pudo enviar la respuesta al pedido de info del restaurante");
		}
		break;
	}


	}

	//Armo respuesta al restaurante
	/*
	 t_header* headerRespuesta = malloc(sizeof(t_header));


	 tMensajeInfoRestaurante* info = malloc(sizeof(tMensajeInfoRestaurante));

	 void* stream=malloc(92);  //SACAR ESTE HARDCODEO

	 armarBufferHardcodeadoRestaurante(headerRespuesta, info,stream);
	 int streamRespuestaSize = sizeof(uint32_t) * 4 + headerRespuesta->size;
	 */
	//ARMO RESPUESTA AL RESTAURANTE HARDCODEADA


}

void armarBufferHardcodeadoRestaurante(t_header2* header,
		tMensajeInfoRestaurante* info, void* stream) {

	llenarHeaderRespuesta(header);
	info->cantCocineros = 20;
	info->afinidadCocineros = string_new();
	info->posicion = string_new();
	info->platos = string_new();
	info->preciosPlatos = string_new();
	info->cantidadHornos = 5;

	string_append(&info->afinidadCocineros, "Milanesas,Pizza");
	string_append(&info->posicion, "4,5");
	string_append(&info->platos, "Milanesas,Pizza,Empanadas");
	string_append(&info->preciosPlatos, "200,40,30");

	//int payloadSize = (sizeof(int) * 6) + strlen(info->afinidadCocineros)+ strlen(info->posicion + strlen(info->platos)+ strlen(info->preciosPlatos));
	int payloadSize = 76; // SACAR ESTE SIZE HARDCODEADO
	void * payload = malloc(payloadSize);

	memset(payload, 0, payloadSize);

	header->size = payloadSize;

	armarPayloadRestaurante(info, payload);

	header->payload = payload;

	int stream_size = sizeof(uint32_t) * 4 + header->size;
	stream = malloc(stream_size);
	int offset = 0;

	int valor = 0;
	memset(stream, valor, stream_size);

	valor = header->modulo; // 01 - MODULO
	memcpy(stream + offset, &valor, sizeof(uint32_t));

	offset += sizeof(uint32_t);

	valor = header->id_proceso; // 02 - ID PROCESO
	memcpy(stream + offset, &valor, sizeof(uint32_t));

	offset += sizeof(uint32_t);

	valor = header->nro_msg; // 03 - TIPO MENSAJE
	memcpy(stream + offset, &valor, sizeof(uint32_t));

	offset += sizeof(uint32_t);

	valor = header->size; // 04 - TAMAÑO DE PAYLOAD
	memcpy(stream + offset, &valor, sizeof(uint32_t));

	offset += sizeof(uint32_t);

	if (header->size > 0) { // 05 - PAYLOAD

		memcpy(stream + offset, header->payload, header->size);

		offset += header->size;

	}

}
void llenarHeaderRespuesta(t_header2* header) {
	header->id_proceso = 0;
	header->modulo = 5;
	header->nro_msg = 1;

}
void armarPayloadRestaurante(tMensajeInfoRestaurante* info, void* stream) {

	int offset = 0;

	uint32_t valor = 0;

	valor = info->cantCocineros;
	memcpy(stream, &valor, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	valor = string_length(info->posicion);
	memcpy(stream + offset, &valor, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream + offset, info->posicion, string_length(info->posicion));
	offset += string_length(info->posicion);

	valor = string_length(info->afinidadCocineros);
	memcpy(stream + offset, &valor, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream + offset, info->afinidadCocineros,
			string_length(info->afinidadCocineros));
	offset += string_length(info->afinidadCocineros);

	valor = string_length(info->platos);
	memcpy(stream + offset, &valor, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream + offset, info->platos, string_length(info->platos));
	offset += string_length(info->platos);

	valor = string_length(info->preciosPlatos);
	memcpy(stream + offset, &valor, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream + offset, info->preciosPlatos,
			string_length(info->preciosPlatos));
	offset += string_length(info->preciosPlatos);

	valor = info->cantidadHornos;
	memcpy(stream + offset, &valor, sizeof(uint32_t));
	offset += sizeof(uint32_t);

}

