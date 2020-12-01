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

#define SIZE_COMANDO 400
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

void levantarConsola(tInfoBloques* infoBloques) {
	printf(" ");
	printf("*****************************CONSOLA SINDICATO*******************\n");

	while (1) {
		int sizeLineaComando=400;
		char lineaComando[SIZE_COMANDO]="";
		memset( lineaComando, 0, SIZE_COMANDO*sizeof(int));
		int i;
		for (i = 0; i <= strlen(lineaComando); i++) {
			lineaComando[i] = '\0';
		}
		printf("COMANDOS DISPONIBLES\n");
		printf("1 - Crear Restaurante\n");
		printf("2 - Crear Receta\n");
		fgets(lineaComando, sizeLineaComando, stdin);

		lineaComando[strlen(lineaComando) - 1] = '\0';
		char** substrings = string_n_split(lineaComando, 8, " ");
		char* nombreComando = substrings[0];



		if (string_equals_ignore_case(nombreComando, "crearRestaurante")) {
			log_info(logger, "Creación Restaurante");

			tCreacionRestaurante* restaurante=malloc(sizeof(tCreacionRestaurante));

			int sizeNombreResto=strlen(substrings[1]);
			restaurante->nombreRestaurante = malloc(sizeNombreResto+1);
			memcpy(restaurante->nombreRestaurante,substrings[1],sizeNombreResto);
			restaurante->nombreRestaurante[sizeNombreResto]='\0';

			restaurante->cantCocineros = atoi(substrings[2]);

			int sizePosicion=strlen(substrings[3]);
			restaurante->posicion = malloc(sizePosicion+1);
			memcpy(restaurante->posicion,substrings[3],sizePosicion);
			restaurante->posicion[sizePosicion]='\0';

			int sizeAfinidad=strlen(substrings[4]);
			restaurante->afinidadCocineros = malloc(sizeAfinidad+1);
			memcpy(restaurante->afinidadCocineros,substrings[4],sizeAfinidad);
			restaurante->afinidadCocineros[sizeAfinidad]='\0';

			int sizePlatos=strlen(substrings[5]);
			restaurante->platos = malloc(sizePlatos+1);
			memcpy(restaurante->platos,substrings[5],sizePlatos);
			restaurante->platos[sizePlatos]='\0';

			int sizePrecios=strlen(substrings[6]);
			restaurante->preciosPlatos = malloc(sizePrecios+1);
			memcpy(restaurante->preciosPlatos,substrings[6],sizePrecios);
			restaurante->preciosPlatos[sizePrecios]='\0';

			restaurante->cantidadHornos = atoi(substrings[7]);

			log_info(logger,"Creando Reestaurante...");
			log_info(logger,"Nombre: %s\n", restaurante->nombreRestaurante);
			log_info(logger,"Cantidad Cocineros: %d\n", restaurante->cantCocineros);

			int archivoGrabado=grabarArchivoRestaurante(restaurante,infoBloques);

			if(archivoGrabado<0){
				log_error(logger,"Archivo no grabado");

			}
			free(restaurante->preciosPlatos);
			free(restaurante->platos);
			free(restaurante->afinidadCocineros);
			free(restaurante->posicion);

			//free(restaurante->nombreRestaurante);
			free(restaurante);


		}

		else if (string_equals_ignore_case(nombreComando, "CrearReceta")) {
			log_info(logger, "Creacion Receta");

			tCreacionReceta* receta= malloc(sizeof(tCreacionReceta));

			int sizeNombreReceta=strlen(substrings[1]);
			receta->nombreReceta=malloc(sizeNombreReceta+1);
			receta->nombreReceta=substrings[1];
			receta->nombreReceta[sizeNombreReceta]='\0';

			int sizeNombrePasos=strlen(substrings[2]);
			receta->pasos=malloc(sizeNombrePasos+1);
			receta->pasos=substrings[2];
			receta->pasos[sizeNombrePasos]='\0';

			int sizeNombreTiempos=strlen(substrings[3]);
			receta->tiemposPasos=malloc(sizeNombrePasos+1);
			receta->tiemposPasos=substrings[3];
			receta->tiemposPasos[sizeNombreTiempos]='\0';

			log_info(logger,"Creando receta...\n");
			log_info(logger,receta->nombreReceta);

			int archivoGrabado=grabarArchivoReceta(receta,infoBloques);

			if(archivoGrabado==0){
						log_error(logger,"Archivo no grabado");

			}
			free(receta->nombreReceta);
			free(receta->pasos);
			free(receta->tiemposPasos);
			free(receta);

		} else
			log_error(logger, "Ingrese un comando Valido\n");
		//free(lineaComando);

		free(substrings);

	}

}

//COMPLETAR
tInfoBloques* leerInfoBloques(){
	infoBloques=malloc(sizeof(tInfoBloques));
	char* pathArchivo=malloc(strlen(pathMetadata)+14);
	struct stat info;
	int map=0;
	int offset=0;
	int e=0;

	//strcpy(pathArchivo,pathMetadata);
	//string_append(&pathArchivo,"Metadata.bin");

	memcpy(pathArchivo,pathMetadata,strlen(pathMetadata));
	offset=offset+strlen(pathMetadata);
	memcpy(pathArchivo+offset,"Metadata.bin",12);
	offset=offset+12;
	pathArchivo[offset]='\0';

	e=stat(pathArchivo,&info);

	FILE* archInfoBloques= fopen(pathArchivo,"r+");
	int fdArchInfoBloques=fileno(archInfoBloques);

	if(archInfoBloques!=NULL){
		log_info(logger, "Leyendo archivo Info de Bloques");
		map = mmap(0, info.st_size, PROT_READ | PROT_WRITE, MAP_SHARED,fdArchInfoBloques, 0);

		if (map == MAP_FAILED) {
			close(fileno(pathArchivo));
			perror("Error mapeando el archivo");
			exit(EXIT_FAILURE);

		}

		char** lineasArchivo = string_split(map, "\n");

		infoBloques->tamBloques=atoi(string_substring(lineasArchivo[0],11,2));
		infoBloques->cantBloques=atoi(string_substring(lineasArchivo[1],7,2));
	}
	if (munmap(map,info.st_size) == -1) {
					log_error(logger,"Error al liberar memoria mapeada");
					exit(EXIT_FAILURE);

	}
	free(pathArchivo);
	return infoBloques;
}


void asignarPaths(){
	pathFiles = malloc(strlen(configuracion->puntoMontaje)+7+1);
	pathBloques = malloc(strlen(configuracion->puntoMontaje)+9+1);
	pathMetadata = malloc(strlen(configuracion->puntoMontaje)+10+1);

	int offsetPathFiles=0;
	memcpy(pathFiles, configuracion->puntoMontaje,strlen(configuracion->puntoMontaje));
	offsetPathFiles=offsetPathFiles+strlen(configuracion->puntoMontaje);
	memcpy(pathFiles+offsetPathFiles, "/Files/",7);
	offsetPathFiles=offsetPathFiles+7;
	pathFiles[offsetPathFiles]='\0';


	int offsetPathBloques=0;
	memcpy(pathBloques,configuracion->puntoMontaje,strlen(configuracion->puntoMontaje));
	offsetPathBloques=offsetPathBloques+strlen(configuracion->puntoMontaje);
	memcpy(pathBloques+offsetPathBloques,"/Bloques/",9);
	offsetPathBloques=offsetPathBloques+9;
	pathBloques[offsetPathBloques]='\0';
	//string_append(&pathBloques, configuracion->puntoMontaje);
	//string_append(&pathBloques, "/Bloques/");

	int offsetPathMetadata=0;
	memcpy(pathMetadata,configuracion->puntoMontaje,strlen(configuracion->puntoMontaje));
	offsetPathMetadata=offsetPathMetadata+strlen(configuracion->puntoMontaje);
	memcpy(pathMetadata+offsetPathMetadata,"/Metadata/",10);
	offsetPathMetadata=offsetPathMetadata+10;
	pathMetadata[offsetPathMetadata]='\0';

	//string_append(&pathMetadata, configuracion->puntoMontaje);
	//string_append(&pathMetadata, "/Metadata/");

	int offsetPathRestaurante=0;
	pathRestaurantes = malloc(strlen(pathFiles)+13+1);
	memcpy(pathRestaurantes,pathFiles,strlen(pathFiles));
	offsetPathRestaurante=offsetPathRestaurante+strlen(pathFiles);
	memcpy(pathRestaurantes+offsetPathRestaurante,"Restaurantes/",13);
	offsetPathRestaurante=offsetPathRestaurante+13;
	pathRestaurantes[offsetPathRestaurante]='\0';
	//string_append(&pathRestaurantes, pathFiles);
	//string_append(&pathRestaurantes, "Restaurantes/");

	int offsetPathRecetas=0;
	pathRecetas = malloc(strlen(pathFiles)+8+1);
	memcpy(pathRecetas,pathFiles,strlen(pathFiles));
	offsetPathRecetas=offsetPathRecetas+strlen(pathFiles);
	memcpy(pathRecetas+offsetPathRecetas,"Recetas/",8);
	offsetPathRecetas=offsetPathRecetas+8;
	pathRecetas[offsetPathRecetas]='\0';
	//string_append(&pathRecetas, pathFiles);
	//string_append(&pathRecetas, "Recetas/");

}

void mapearInfoResto(char* stringLeido, tMensajeInfoRestaurante*info){
	char** propiedades=string_split(stringLeido,"=");

	info->cantCocineros=atoi(strremove(propiedades[1],"POSICION"));
	info->posicion=strremove(propiedades[2],"AFINIDAD_COCINEROS");
	info->afinidadCocineros=strremove(propiedades[3],"PLATOS");
	info->platos=strremove(propiedades[4],"PRECIO_PLATOS");
	info->preciosPlatos=strremove(propiedades[5],"CANTIDAD_HORNOS");
	info->cantidadHornos=atoi(propiedades[6]);

}

void mapearInfoPedido(char* stringLeido, tMensajeInfoPedido*info){
	char** propiedades=string_split(stringLeido,"=");

	char* subEstadoPedido=strremove(propiedades[1],"LISTA_PLATOS");
	subEstadoPedido[strlen(subEstadoPedido)]='\0';
	info->estadoPedido=malloc(strlen(subEstadoPedido)+1);
	memcpy(info->estadoPedido,subEstadoPedido,strlen(subEstadoPedido));
	info->estadoPedido[strlen(subEstadoPedido)]='\0';

	char* subListaPlatos=strremove(propiedades[2],"CANTIDAD_PLATOS");
	subListaPlatos[strlen(subListaPlatos)]='\0';
	info->listaPlatos=malloc(strlen(subListaPlatos)+1);
	memcpy(info->listaPlatos,subListaPlatos,strlen(subListaPlatos));
	info->listaPlatos[strlen(subListaPlatos)]='\0';

	char* subListaCantidad=strremove(propiedades[3],"CANTIDAD_LISTA");
	subListaCantidad[strlen(subListaCantidad)]='\0';
	info->cantidadPlatos=malloc(strlen(subListaCantidad)+1);
	memcpy(info->cantidadPlatos,subListaCantidad,strlen(subListaCantidad));
	info->cantidadPlatos[strlen(subListaCantidad)]='\0';

	char* subListaCantidadLista=strremove(propiedades[4],"PRECIO_TOTAL");
	subListaCantidadLista[strlen(subListaCantidad)]='\0';
	info->cantidadLista=malloc(strlen(subListaCantidadLista)+1);
	memcpy(info->cantidadLista,subListaCantidadLista,strlen(subListaCantidadLista));
	info->cantidadLista[strlen(subListaCantidadLista)]='\0';

	info->precioTotal=atoi(propiedades[5]);

}

tMensajeInfoRestaurante *leerBloquesResto(int bloqueInicial,int sizeResto,t_list* bloquesAsigandosAResto){
	char* pathBloqueInicial=malloc(strlen(pathBloques)+10);
	strcpy(pathBloqueInicial,pathBloques);
	string_append_with_format(&pathBloqueInicial,"%d%s",bloqueInicial,".bin");
	uint32_t cantBloquesALeer=list_size(bloquesAsigandosAResto);
	int i;
	tMensajeInfoRestaurante* infoResto=malloc(sizeof(tMensajeInfoRestaurante));
	char* stringInfo= malloc(sizeResto+1);
	strcpy(stringInfo,"");
	int bloqueActual=0;
	struct stat info;
	int e=0;

	e=stat(pathBloqueInicial,&info);

	if(e==0){
		log_info(logger,"Leyendo bloques");

		for (i=0; i<cantBloquesALeer;i++){
			bloqueActual=atoi(list_get(bloquesAsigandosAResto,i));
			char* pathBloqueActual=malloc(strlen(pathBloques)+30);
			strcpy(pathBloqueActual,pathBloques);
			string_append_with_format(&pathBloqueActual,"%d%s",bloqueActual,".bin");
			int eActual=0;

			struct stat infoBloqueActual;
			eActual=stat(pathBloqueActual,&infoBloqueActual);

			if(eActual==0){
				FILE* archivoBloqueActual = fopen(pathBloqueActual, "r+");

				char* map = mmap(0, info.st_size-sizeof(int), PROT_READ | PROT_WRITE,MAP_SHARED, fileno(archivoBloqueActual), 0);

				if (map == MAP_FAILED) {
					close(fileno(archivoBloqueActual));
					perror("Error mapeando el archivo");
					exit(EXIT_FAILURE);

				}
				map[info.st_size-sizeof(int)]='\0';

				if(i==cantBloquesALeer-1){ //leo el ultimo bloque
					string_append(&stringInfo,map);

				}else {//bloques anteriores al ultimo
					//uint32_t bloqueSiguiente=atoi(list_get(bloquesAsigandosAResto,i+1));
					char* bloqueSiguiente=string_substring(map,strlen(map)-sizeof(uint32_t),strlen(map));
					(*bloqueSiguiente)="";
					char* newMap=malloc(strlen(map)-strlen(string_itoa(bloqueSiguiente)));
					(*newMap)="";
					newMap=removerBloqueSiguienteDeString(map,bloqueSiguiente);
					string_append(&stringInfo,newMap);
				}

				if (munmap(map,info.st_size) == -1) {
								log_error(logger,"Error al liberar memoria mapeada");
								exit(EXIT_FAILURE);

				}

			}


		}
		mapearInfoResto(stringInfo,infoResto);


	}else log_error(logger,"El archivo de bloque inicial no existe");

	free(pathBloqueInicial);
	free(stringInfo);
	return infoResto;
}





//MENSAJE 6: OBTENER RESTAURANTE
tMensajeInfoRestaurante *obtenerInfoRestaurante(char* nombreResto){
	char* pathArchivoInfoActual=malloc(strlen(pathRestaurantes)+strlen(nombreResto)+30);
	tMensajeInfoRestaurante* infoResto=malloc(sizeof(tMensajeInfoRestaurante));
	strcpy(pathArchivoInfoActual,pathRestaurantes);
	//string_append_with_format(&pathArchivoInfoActual,"%s%s","/",nombreResto);
	string_append_with_format(&pathArchivoInfoActual,"%s%s%s",nombreResto,"/","Info.AFIP");

	struct stat info;
	int e=0;
	int map=0;
	int bloqueInicial=0;
	int sizeResto=0;
	t_list* bloquesAsigandosAResto=list_create();
	bloquesAsigandosAResto=dictionary_get(diccionarioBloquesAsignadosARestos,nombreResto);


	e=stat(pathArchivoInfoActual,&info);

	if(e==0){
		log_info(logger,"Leyendo archivo resto...");
		FILE* archivoInfoResto=fopen(pathArchivoInfoActual,"r+");
		map = mmap(0, info.st_size, PROT_READ | PROT_WRITE, MAP_SHARED,fileno(archivoInfoResto), 0);

		if (map == MAP_FAILED) {
			close(fileno(archivoInfoResto));
			perror("Error mapeando el archivo");
			exit(EXIT_FAILURE);

		}

		char** lineasArchivo = string_split(map, "\n");

		sizeResto= atoi(string_substring_from(lineasArchivo[0], 5));
		//bloqueInicial = atoi(string_substring_from(lineasArchivo[1], 14));
		bloqueInicial=atoi(list_get(bloquesAsigandosAResto,0));

		infoResto=leerBloquesResto(bloqueInicial,sizeResto,bloquesAsigandosAResto);

		if (munmap(map,info.st_size) == -1) {
						log_error(logger,"Error al liberar memoria mapeada");
						exit(EXIT_FAILURE);

		}
		fclose(archivoInfoResto);


	}

	return infoResto;


}

void mapearInfoReceta(char* stringLeido, tMensajeInfoReceta*info){
	char** propiedades=string_split(stringLeido,"=");

	info->pasos=atoi(strremove(propiedades[1],"TIEMPO_PASOS"));
	info->tiempos=propiedades[2];

}

tMensajeInfoReceta *leerBloquesRecetas(int bloqueInicial,int sizeRecetas,t_list* bloquesAsigandosARecetas){
	char* pathBloqueInicial=malloc(strlen(pathBloques)+10);
	strcpy(pathBloqueInicial,pathBloques);
	string_append_with_format(&pathBloqueInicial,"%d%s",bloqueInicial,".bin");
	uint32_t cantBloquesALeer=list_size(bloquesAsigandosARecetas);
	int i;
	tMensajeInfoReceta* infoReceta=malloc(sizeof(tMensajeInfoReceta));
	char* stringInfo= malloc(sizeRecetas+1);
	strcpy(stringInfo,"");
	int bloqueActual=0;
	struct stat info;
	int e=0;

	e=stat(pathBloqueInicial,&info);

	if(e==0){
		log_info(logger,"Leyendo bloques");

		for (i=0; i<cantBloquesALeer;i++){
			bloqueActual=atoi(list_get(bloquesAsigandosARecetas,i));
			char* pathBloqueActual=malloc(strlen(pathBloques)+30);
			strcpy(pathBloqueActual,pathBloques);
			string_append_with_format(&pathBloqueActual,"%d%s",bloqueActual,".bin");
			int eActual=0;

			struct stat infoBloqueActual;
			eActual=stat(pathBloqueActual,&infoBloqueActual);

			if(eActual==0){
				FILE* archivoBloqueActual = fopen(pathBloqueActual, "r+");

				char* map = mmap(0, info.st_size-sizeof(int), PROT_READ | PROT_WRITE,MAP_SHARED, fileno(archivoBloqueActual), 0);

				if (map == MAP_FAILED) {
					close(fileno(archivoBloqueActual));
					perror("Error mapeando el archivo");
					exit(EXIT_FAILURE);

				}
				map[info.st_size-sizeof(int)]='\0';

				if(i==cantBloquesALeer-1){ //leo el ultimo bloque
					string_append(&stringInfo,map);

				}else {//bloques anteriores al ultimo
					//uint32_t bloqueSiguiente=atoi(list_get(bloquesAsigandosAResto,i+1));
					char* bloqueSiguiente=string_substring(map,strlen(map)-sizeof(uint32_t),strlen(map));
					(*bloqueSiguiente)="";
					char* newMap=malloc(strlen(map)-strlen(string_itoa(bloqueSiguiente)));
					(*newMap)="";
					newMap=removerBloqueSiguienteDeString(map,bloqueSiguiente);
					string_append(&stringInfo,newMap);
				}
				if (munmap(map,info.st_size) == -1) {
								log_error(logger,"Error al liberar memoria mapeada");
								exit(EXIT_FAILURE);

				}
			}

		}
		mapearInfoReceta(stringInfo,infoReceta);


	}else log_error(logger,"El archivo de bloque inicial no existe");

	free(pathBloqueInicial);
	free(stringInfo);
	return infoReceta;
}

//Mensaje 8: Obtener Receta
tMensajeInfoReceta *obtenerInfoReceta(char* nombreReceta){
	char* pathArchivoInfoActual=malloc(strlen(pathRecetas)+strlen(nombreReceta)+5+1);
	tMensajeInfoReceta* infoReceta=malloc(sizeof(tMensajeInfoReceta));
	strcpy(pathArchivoInfoActual,pathRecetas);
	string_append_with_format(&pathArchivoInfoActual,"%s%s%s","/",nombreReceta,".AFIP");

	struct stat info;
	int e=0;
	int map=0;
	int bloqueInicial=0;
	int sizeResto=0;
	t_list* bloquesAsigandosARecetas=list_create();
	bloquesAsigandosARecetas=dictionary_get(diccionarioBloquesAsignadosARestos,nombreReceta);


	e=stat(pathArchivoInfoActual,&info);

	if(e==0){
		log_info(logger,"Leyendo archivo recetas...");
		FILE* archivoInfoResto=fopen(pathArchivoInfoActual,"r+");
		map = mmap(0, info.st_size, PROT_READ | PROT_WRITE, MAP_SHARED,fileno(archivoInfoResto), 0);

		if (map == MAP_FAILED) {
			close(fileno(archivoInfoResto));
			perror("Error mapeando el archivo");
			exit(EXIT_FAILURE);

		}

		char** lineasArchivo = string_split(map, "\n");

		sizeResto= atoi(string_substring_from(lineasArchivo[0], 5));
		//bloqueInicial = atoi(string_substring_from(lineasArchivo[1], 14));
		bloqueInicial=atoi(list_get(bloquesAsigandosARecetas,0));

		infoReceta=leerBloquesResto(bloqueInicial,sizeResto,bloquesAsigandosARecetas);

		if (munmap(map,info.st_size) == -1) {
						log_error(logger,"Error al liberar memoria mapeada");
						exit(EXIT_FAILURE);

		}
		fclose(archivoInfoResto);


	}

	return infoReceta;


}

void hardcodearPedido(tCreacionPedido* pedido){
	pedido->estadoPedido=malloc(10);
	memcpy(pedido->estadoPedido,"Pendiente",9);
	pedido->estadoPedido[9]='\0';

	pedido->listaPlatos=malloc(28);
	memcpy(pedido->listaPlatos,"Milanesa,Empanadas,Ensalada",27);
	pedido->listaPlatos[27]='\0';

	pedido->cantidadPlatos=malloc(7);
	memcpy(pedido->cantidadPlatos,"2,12,1",6);
	pedido->cantidadPlatos[6]='\0';

	pedido->cantidadLista=malloc(6);
	memcpy(pedido->cantidadLista,"1,6,0",5);
	pedido->cantidadLista[5]='\0';

	pedido->precioTotal=1150;
}


//MENSAJE 2: CREAR PEDIDO
int crearPedido(tCreacionPedido* pedidoNuevo,char* nombreRestaurante,char* nombrePedido,tInfoBloques* infoBloques){
	char* pathRestaurante= malloc(strlen(pathRestaurantes)+strlen(nombreRestaurante)+1+1);
	memcpy(pathRestaurante,pathRestaurantes,strlen(pathRestaurantes));
	memcpy(pathRestaurante+strlen(pathRestaurantes),nombreRestaurante,strlen(nombreRestaurante));
	memcpy(pathRestaurante+strlen(pathRestaurantes)+strlen(nombreRestaurante),"/",1);
	pathRestaurante[strlen(pathRestaurantes)+strlen(nombreRestaurante)+1]='\0';
	char* pathPedidoNuevo= malloc(strlen(pathRestaurante)+strlen(nombrePedido)+5+1);

	memcpy(pathPedidoNuevo,pathRestaurante,strlen(pathRestaurante));
	//string_append_with_format(&pathPedidoNuevo,"%s%s",nombrePedido,".AFIP");
	memcpy(pathPedidoNuevo+strlen(pathRestaurante),nombrePedido,strlen(nombrePedido));
	memcpy(pathPedidoNuevo+strlen(pathRestaurante)+strlen(nombrePedido),".AFIP",5);
	pathPedidoNuevo[strlen(pathRestaurante)+strlen(nombrePedido)+5]='\0';
	//tring_append_with_format(&pathPedido,"%s%s",nombrePedido,".AFIP");

	struct stat infoPathRestaurante;
	struct stat infoPathPedido;

	int eRestaurante=0;
	int ePedido=0;
	int cantPedidosEnDirectorio=0;
	int resultadoOperacion=1;


	eRestaurante=stat(pathRestaurante,&infoPathRestaurante);

	if(eRestaurante==0){
	//	log_info(logger,"Leyendo Restaurante");
		ePedido=stat(pathPedidoNuevo,&infoPathPedido);

		if(ePedido==0){
			log_error(logger,"Ya existe un pedido con ese nombre");
			resultadoOperacion=0;
		}else{
		//	log_info(logger,"Creando pedido");
			cantPedidosEnDirectorio=contarPedidosEnDirectorio(pathRestaurante);
			//string_append_with_format(&pathPedidoNuevo,"%s%d%s",nombrePedido,cantPedidosEnDirectorio+1,".AFIP");
			resultadoOperacion=grabarArchivoPedido(pedidoNuevo,pathPedidoNuevo,nombrePedido);
		}
	}else {
		log_error(logger,"No existe el restaurante");
		resultadoOperacion=0;
	}
return resultadoOperacion;

}
tMensajeInfoPedido *leerBloquesPedido(int bloqueInicial,int sizePedido,t_list* bloquesAsigandosAPedido){
	char* pathBloqueInicial=malloc(strlen(pathBloques)+strlen(string_itoa(bloqueInicial))+3+1); //agrego el /0 al final
	strcpy(pathBloqueInicial,pathBloques);
	string_append_with_format(&pathBloqueInicial,"%d%s",bloqueInicial,".bin");
	uint32_t cantBloquesALeer=list_size(bloquesAsigandosAPedido);
	int i;
	tMensajeInfoPedido* infoPedido=malloc(sizeof(tMensajeInfoPedido));

	int bloqueActual=0;
	struct stat info;
	int e=0;
	char* stringInfo=malloc(sizePedido+10);
	int offsetLeido=0;

	e=stat(pathBloqueInicial,&info);

	if(e==0){
		log_info(logger,"Leyendo bloques");

		for (i=0; i<cantBloquesALeer;i++){
			bloqueActual=atoi(list_get(bloquesAsigandosAPedido,i));
			char* pathBloqueActual=malloc(strlen(pathBloques)+string_itoa(bloqueActual)+3+1);
			int offsetPathBloque=0;
			//strcpy(pathBloqueActual,pathBloques);
			memcpy(pathBloqueActual,pathBloques,strlen(pathBloques));
			offsetPathBloque=offsetPathBloque+strlen(pathBloques);
			memcpy(pathBloqueActual+strlen(pathBloques),string_itoa(bloqueActual),strlen(string_itoa(bloqueActual)));
			offsetPathBloque=offsetPathBloque+strlen(string_itoa(bloqueActual));
			memcpy(pathBloqueActual+strlen(pathBloques)+strlen(string_itoa(bloqueActual)),".bin",4);
			offsetPathBloque=offsetPathBloque+4;
			//string_append_with_format(&pathBloqueActual,"%d%s",bloqueActual,".bin");
			pathBloqueActual[offsetPathBloque]='\0';
			int eActual=0;

			struct stat infoBloqueActual;
			eActual=stat(pathBloqueActual,&infoBloqueActual);

			if(eActual==0){
				FILE* archivoBloqueActual = fopen(pathBloqueActual, "r+");

				char* map = mmap(0, info.st_size-sizeof(int), PROT_READ | PROT_WRITE,MAP_SHARED, fileno(archivoBloqueActual), 0);
				if (map == MAP_FAILED) {
					close(fileno(archivoBloqueActual));
					perror("Error mapeando el archivo");
					exit(EXIT_FAILURE);

				}

				if(i==cantBloquesALeer-1){
					//string_append(&stringInfo,map);
					//int offset=0;
					memcpy(stringInfo+offsetLeido,map,strlen(map));
					offsetLeido=offsetLeido+strlen(map);
					//stringInfo[offset]='\0';

				}else {
					int bloqueSiguiente=atoi(list_get(bloquesAsigandosAPedido,i+1));
					char* newMap=malloc(strlen(map)-strlen(string_itoa(bloqueSiguiente))+1);
					int offsetMap=0;
					int offsetStringInfo=0;
					//newMap=removerBloqueSiguienteDeString(map,string_itoa(bloqueSiguiente));
					newMap=string_substring(map,0,strlen(map)-4);
					offsetMap=offsetMap+strlen(map)-4;
					newMap[offsetMap]='\0';
					//string_append(&stringInfo,newMap);
					memcpy(stringInfo+offsetLeido,newMap,strlen(newMap));
					offsetStringInfo=offsetStringInfo+strlen(newMap);
					offsetLeido=offsetLeido+strlen(newMap);

				}
				if (munmap(map,info.st_size) == -1) {
								log_error(logger,"Error al liberar memoria mapeada");
								exit(EXIT_FAILURE);

				}
				fclose(archivoBloqueActual);

			}

		}
		stringInfo[offsetLeido]='\0';
		mapearInfoPedido(stringInfo,infoPedido);
		free(stringInfo);


	}else log_error(logger,"El archivo de bloque inicial no existe");
	free(pathBloqueInicial);
	return infoPedido;
}


tMensajeInfoPedido *obtenerInfoPedido(char* nombreResto,char* nombrePedido,int resultado){
	char* pathArchivoInfoActual=malloc(strlen(pathRestaurantes)+strlen(nombreResto)+30);
	char* idPedido=malloc(strlen(nombreResto)+strlen(nombrePedido)+1+1); //Nombre Resto*Nombre pedido+q byte de '-' + 1 byte final
	char* pathRestaurante=malloc(strlen(pathRestaurantes)+strlen(nombreResto)+1);
	tMensajeInfoPedido* infoPedido=malloc(sizeof(tMensajeInfoPedido));

	int offsetResto=0;

	memcpy(pathRestaurante,pathRestaurantes,strlen(pathRestaurantes));
	offsetResto+=strlen(pathRestaurantes);
	memcpy(pathRestaurante+offsetResto,nombreResto,strlen(nombreResto));
	offsetResto+=strlen(nombreResto);

	strcpy(pathArchivoInfoActual,pathRestaurantes);
	strcpy(idPedido,"");
	string_append(&pathArchivoInfoActual,nombreResto);
	string_append_with_format(&pathArchivoInfoActual,"%s%s%s","/",nombrePedido,".AFIP");

	struct stat infoPathRestaurante;
	int e1=0;
	struct stat infoPathPedido;
	int e=0;
	int map=0;
	int bloqueInicial=0;
	int sizePedido=0;
	int offset=0;
	t_list* bloquesAsigandosAPedido=list_create();
	//string_append_with_format(&idPedido,"%s%s%s",nombreResto,"-",nombrePedido);
	memcpy(idPedido,nombreResto,strlen(nombreResto));
	offset=offset+strlen(nombreResto);
	memcpy(idPedido+strlen(nombreResto),"-",1);
	offset=offset+1;
	memcpy(idPedido+strlen(nombreResto)+1,nombrePedido,strlen(nombrePedido));
	offset=offset+strlen(nombrePedido);
	idPedido[offset]='\0';

	bloquesAsigandosAPedido=dictionary_get(diccionarioBloquesAsignadosAPedidos,idPedido);


	e=stat(pathArchivoInfoActual,&infoPathPedido);
	e1=stat(pathRestaurante,&infoPathRestaurante);


	if(e1==0){
		if(e==0){
			log_info(logger,"Leyendo archivo resto...");
			FILE* archivoInfoPedido=fopen(pathArchivoInfoActual,"r+");
			map = mmap(0, infoPathPedido.st_size, PROT_READ | PROT_WRITE, MAP_SHARED,fileno(archivoInfoPedido), 0);

			if (map == MAP_FAILED) {
				close(fileno(archivoInfoPedido));
				perror("Error mapeando el archivo");
				exit(EXIT_FAILURE);

			}

			char** lineasArchivo = string_split(map, "\n");

			sizePedido= atoi(string_substring_from(lineasArchivo[0], 5));
			//bloqueInicial = atoi(string_substring_from(lineasArchivo[1], 14));
			bloqueInicial=atoi(list_get(bloquesAsigandosAPedido,0));

			infoPedido=leerBloquesPedido(bloqueInicial,sizePedido,bloquesAsigandosAPedido);
			if (munmap(map,infoPathPedido.st_size) == -1) {
							log_error(logger,"Error al liberar memoria mapeada");
							exit(EXIT_FAILURE);

			}
			fclose(archivoInfoPedido);


		}else {
			log_error(logger,"No existe el archivo de Pedido");
			resultado=-2;
		}


	}else {
		log_error(logger, "No existe el restaurante");
		resultado=-1;
	}

	return infoPedido;


}

uint32_t buscarPrecioPlatoEnRestaurante(char* nombrePlato,char* nombreRestaurante){
	char* subCadenaListaPlatos=string_new();
	char* subCadenaPreciosPlatos=string_new();
	int posPlatoEnLista=0;
	int i=0;

	tMensajeInfoRestaurante* info=malloc(sizeof(tMensajeInfoRestaurante));
	info=obtenerInfoRestaurante(nombreRestaurante);
	subCadenaListaPlatos= string_substring(info->platos,1,strlen(info->platos)-1);
	subCadenaPreciosPlatos=string_substring(info->preciosPlatos,1,strlen(info->preciosPlatos)-1);

	char** platos=string_split(subCadenaListaPlatos,",");
	while(platos[i]!=NULL){
		if(strcmp(platos[i],nombrePlato)==0){
			posPlatoEnLista=i;
		}

		i++;
	}

	char** preciosPlatos=string_split(subCadenaPreciosPlatos,",");

	return atoi(preciosPlatos[posPlatoEnLista]);

}

char* convertirArrayAString(char** array, int sizeArray){
	int i=0;
	int sizeString=0;
	char* parentesisAperturaConStringPlatos=malloc(1);
	memcpy(parentesisAperturaConStringPlatos,"[",1);
	//parentesisAperturaConStringPlatos[1]='\0';

	while(array[i]!=NULL){
		sizeString=sizeString+strlen(array[i]);
		i++;
	}
	char* stringCantidades=malloc(5+sizeArray+2+1); //Size= sizeString + cantidad de ','+ '[' + ']' + '\0'
	strcpy(stringCantidades,"[");
	//memcpy(stringCantidades,parentesisAperturaConStringPlatos,1);
	int offset=1;
	for(i=0;i<sizeArray;i++){
		int valorActual=atoi(array[i]);

		memcpy(stringCantidades+offset,string_itoa(valorActual),strlen(string_itoa(valorActual)));
		if(i<sizeArray-1){
			memcpy(stringCantidades+offset+strlen(string_itoa(valorActual)),",",1);
			offset=offset+1+strlen(string_itoa(valorActual));
		}else{
			offset=offset+strlen(string_itoa(valorActual));
		}
	}
	stringCantidades[offset]=']';
	stringCantidades[offset+1]='\0';

	return stringCantidades;


}
char* armarStringNuevoAGrabar(tMensajeInfoPedido* info, char* nombrePlato,int cantidad,char* nombreRestaurante,char*operacion){

	int offsetListaPlatos=(strlen(info->listaPlatos)-1);
	int offsetCantidadPlatos=(strlen(info->cantidadPlatos)-1);
	int offsetCantidadLista=(strlen(info->cantidadLista)-1);
	int sizeArray=0;

	 if(strcmp(operacion,"AGREGAR_PLATO")==0){
			if(strstr(info->listaPlatos,nombrePlato)==NULL){ //EL PLATO QUE QUIERO AGREGAR NO  EXISTE: LO AGREGO AL FINAL, AGREGO SU CANTIDAD, CANTIDAD LISTA EN 0 Y SUMO EL PRECIO AL TOTAL

				memcpy(info->listaPlatos+offsetListaPlatos,",",1);
				offsetListaPlatos++;
				memcpy(info->listaPlatos + offsetListaPlatos, nombrePlato,strlen(nombrePlato));
				offsetListaPlatos = offsetListaPlatos + strlen(nombrePlato);
				memcpy(info->listaPlatos + offsetListaPlatos, "]", 1);
				memcpy(info->cantidadPlatos + offsetCantidadPlatos, ",", 1);
				offsetCantidadPlatos++;
				memcpy(info->cantidadPlatos + offsetCantidadPlatos,string_itoa(cantidad), strlen(string_itoa(cantidad)));
				offsetCantidadPlatos = offsetCantidadPlatos+ strlen(string_itoa(cantidad));
				memcpy(info->cantidadPlatos + offsetCantidadPlatos, "]", 1);

				memcpy(info->cantidadLista+offsetCantidadLista,",",1);
				offsetCantidadLista++;
				int a=0;
				memcpy(info->cantidadLista+offsetCantidadLista,string_itoa(a),strlen(string_itoa(a)));
				offsetCantidadLista=offsetCantidadLista+strlen(string_itoa(a));
				memcpy(info->cantidadLista+offsetCantidadLista,"]",1);

				uint32_t precioPlatoActual=buscarPrecioPlatoEnRestaurante(nombrePlato,nombreRestaurante);
				info->precioTotal=info->precioTotal+(precioPlatoActual*cantidad);

			}else{ //EL PLATO YA EXISTE, SUMAR LA CANTIDAD, Y BUSCAR SU PRECIO PARA SUMARLO AL PRECIO TOTAL DEL PEDIDO

				char** arrayPlatos=string_get_string_as_array(info->listaPlatos);
				char** arrayCantidades=string_get_string_as_array(info->cantidadPlatos);
				//char** arrayCantidades=string_split(stringCantidades,",");
				int i=0;

				while(arrayPlatos[i]!=NULL){
					if(strcmp(arrayPlatos[i],nombrePlato)==0){
						char* cantidadNueva=string_itoa(atoi(arrayCantidades[i])+cantidad);
						arrayCantidades[i]=cantidadNueva;
					}
					i++;
				}

				sizeArray=i;

				//char* stringConPlatoNuevo=convertirArrayAString(arrayPlatos);

				char* stringConCantidadNueva=convertirArrayAString(arrayCantidades,sizeArray);

				/*info->listaPlatos=malloc(strlen(stringConPlatoNuevo)+1);
				memcpy(info->listaPlatos,stringConPlatoNuevo,strlen(stringConPlatoNuevo));
				stringConPlatoNuevo[strlen(stringConPlatoNuevo)]='\0';*/

				info->cantidadPlatos=malloc(strlen(stringConCantidadNueva)+1);
				memcpy(info->cantidadPlatos,stringConCantidadNueva,strlen(stringConCantidadNueva));
				stringConCantidadNueva[strlen(stringConCantidadNueva)]='\0';
				info->cantidadPlatos[strlen(stringConCantidadNueva)]='\0';

				uint32_t precioPlatoActual=buscarPrecioPlatoEnRestaurante(nombrePlato,nombreRestaurante);
				info->precioTotal=info->precioTotal+(precioPlatoActual*cantidad);
			}

		}

	 else if (strcmp(operacion,"PLATO_LISTO")==0){
		char** arrayPlatos=string_get_string_as_array(info->listaPlatos);
		char** arrayCantidadesListas=string_get_string_as_array(info->listaPlatos);
		int i=0;

		while (arrayPlatos[i] != NULL) {
			if (strcmp(arrayPlatos[i], nombrePlato) == 0) {
				char* cantidadNueva = string_itoa(
						atoi(arrayCantidadesListas[i]) + cantidad);
				arrayCantidadesListas[i] = cantidadNueva;
			}
			i++;
		}

		sizeArray = i;

		//char* stringConPlatoNuevo=convertirArrayAString(arrayPlatos);

		char* stringConCantidadNueva = convertirArrayAString(arrayCantidadesListas,sizeArray);

		/*info->listaPlatos=malloc(strlen(stringConPlatoNuevo)+1);
		 memcpy(info->listaPlatos,stringConPlatoNuevo,strlen(stringConPlatoNuevo));
		 stringConPlatoNuevo[strlen(stringConPlatoNuevo)]='\0';*/

		info->cantidadLista = malloc(strlen(stringConCantidadNueva) + 1);
		memcpy(info->cantidadLista, stringConCantidadNueva,strlen(stringConCantidadNueva));
		stringConCantidadNueva[strlen(stringConCantidadNueva)] = '\0';
		info->cantidadLista[strlen(stringConCantidadNueva)] = '\0';

		uint32_t precioPlatoActual = buscarPrecioPlatoEnRestaurante(nombrePlato,nombreRestaurante);
		//info->precioTotal = info->precioTotal + (precioPlatoActual * cantidad);

	 }else if(strcmp(operacion,"CONFIRMAR_PEDIDO")==0){

	 }else{

	 }



	//string_append_with_format(&listaPlatosNueva,"%s%s",",",nombrePlato);
	int sizeStringNuevo=strlen(info->estadoPedido)+strlen(info->listaPlatos)+strlen(info->cantidadPlatos)+strlen(info->cantidadLista)+sizeof(uint32_t)+
						strlen("ESTADO_PEDIDO=")+ strlen("LISTA_PLATOS=")+strlen("CANTIDAD_PLATOS=")+strlen("CANTIDAD_LISTA=")+strlen("PRECIO_TOTAL=")+1;
	char*stringNuevo=malloc(sizeStringNuevo);
	strcpy(stringNuevo,"");
	sprintf(stringNuevo,"%s%s%s%s%s%s%s%s%s%d","ESTADO_PEDIDO=",info->estadoPedido,
																  "LISTA_PLATOS=",info->listaPlatos,
																  "CANTIDAD_PLATOS=", info->cantidadPlatos,
																  "CANTIDAD_LISTA=", info->cantidadLista,
																  "PRECIO_TOTAL=", info->precioTotal);
	stringNuevo[strlen(stringNuevo)]='\0';
	free(info->estadoPedido);
	free(info->listaPlatos);
	free(info->cantidadPlatos);
	free(info->cantidadLista);
	free(info);
	return stringNuevo;
}

void grabarInfoEnBloquesPedidos(char* stringAGrabar,t_list* bloquesAsigandosAPedido,char* nombreRestaurante,char* nombrePedido){
	//recorrer la lista de bloques asignados
	//Truncar los archivos
	//Grabarlos
	//Actualizar la lista de bloques asignados, actualizar el diccionario de bloques asignados a pedidos, actualizar el archivo persistido de bloques asignados
	int i;
	char* pathBloqueActual=malloc(strlen(pathBloques)+10);
	strcpy(pathBloqueActual,pathBloques);
	int cantBloquesAEscribir= list_size(bloquesAsigandosAPedido);
	int offsetEscritura=0;
	char* bytesAEscribir=malloc(infoBloques->tamBloques);
	char* idPedido=malloc(strlen(nombreRestaurante)+strlen(nombrePedido)+1);
	strcpy(idPedido,"");
	string_append_with_format(&idPedido,"%s%s%s",nombreRestaurante,"-",nombrePedido);

	for (i=0;list_size(bloquesAsigandosAPedido);i++){
		int bloqueActual=atoi(list_get(bloquesAsigandosAPedido,i));
		string_append_with_format(&pathBloqueActual,"%d%s",bloqueActual,".bin");

		FILE* archivoBloqueActual=fopen(pathBloqueActual,"w");

		if(-1==truncate((int)(fileno(archivoBloqueActual)),infoBloques->tamBloques)){
			log_error(logger,"Error al truncar el archivo");
		}

		if(i<cantBloquesAEscribir-1){

			bytesAEscribir=string_substring(stringAGrabar,offsetEscritura,infoBloques->tamBloques-sizeof(uint32_t));
			FILE* archivoBloqueActual = fopen(pathBloqueActual, "w");
			if (fwrite(bytesAEscribir,strlen(bytesAEscribir) + sizeof(uint32_t), 1,archivoBloqueActual) == 0) {
				log_error(logger,
						"Error al escribir en el archivo info de restaurante");
			}

			offsetEscritura = offsetEscritura + infoBloques->tamBloques- sizeof(uint32_t);

			fclose(archivoBloqueActual);
			strcpy(pathBloqueActual, "");
			strcpy(pathBloqueActual, pathBloques);

		}else{
			bytesAEscribir=string_substring(stringAGrabar,offsetEscritura,strlen(stringAGrabar));

			string_append_with_format(&pathBloqueActual, "%d%s", bloqueActual,".bin");

			FILE* archivoBloqueActual = fopen(pathBloqueActual, "w");
			if (fwrite(bytesAEscribir, strlen(bytesAEscribir), 1,
					archivoBloqueActual) == 0) {
				log_error(logger,"Error al escribir en el archivo info de restaurante");
			}

			fclose(archivoBloqueActual);

		}

	}
	t_list* listabloquesAsigandosAPedidoAnterior=dictionary_get(diccionarioBloquesAsignadosAPedidos,idPedido);
	free(listabloquesAsigandosAPedidoAnterior);
	dictionary_put(diccionarioBloquesAsignadosAPedidos,idPedido,bloquesAsigandosAPedido);
}


int grabarNuevoPedidoActualizado(char* stringAGrabar,char* nombreRestaurante,char* nombrePedido){

	//verificar si ocupa mas bloques de los que le fueron asignados
	int cantBloquesAEscribir=0;
	int bloqueSiguiente=0;
	int offset=0;
	int resultadoOperacion=1;
	char* nombreRestoPedido= malloc(strlen(nombreRestaurante)+strlen(nombrePedido)+2);
	//string_append_with_format(&nombreRestoPedido,"%s%s%s",nombreRestaurante,"-",nombrePedido);
	memcpy(nombreRestoPedido,nombreRestaurante,strlen(nombreRestaurante));
	offset=offset+strlen(nombreRestaurante);
	memcpy(nombreRestoPedido+strlen(nombreRestaurante),"-",1);
	offset=offset+1;
	memcpy(nombreRestoPedido+strlen(nombreRestaurante)+1,nombrePedido,strlen(nombrePedido));
	offset=offset+strlen(nombrePedido);
	nombreRestoPedido[offset]='\0';

	t_list* bloquesAsigandosAPedido=dictionary_get(diccionarioBloquesAsignadosAPedidos,nombreRestoPedido);

	if((strlen(stringAGrabar)%infoBloques->tamBloques)==0){ //Caso1: la cantidad bytes a escribir es multiplo del tamaño de los bloques
															//tengo que verificar si es que se lleno el ultimo bloque, y se deberan pedir mas
		cantBloquesAEscribir=strlen(stringAGrabar)/infoBloques->tamBloques;

		if(cantBloquesAEscribir==list_size(bloquesAsigandosAPedido)){//Caso en que se lleno el ultimo bloque si sobrepasarlo, es decir no asigno bloques adicionales
			//sobreescribir bloques asignados anteriormente
			//TRUNCAR ARCHIVOS Y ESCRIBIR LOS BLOQUES

		}else{//Caso en que sobrepasa la cantidad de bloques que tenia asignada, tengo que ver cuantos bloques extras tenog que asignar
			int nBloquesExtrasAAsignar=cantBloquesAEscribir - list_size(bloquesAsigandosAPedido);
			for(int i=0;nBloquesExtrasAAsignar;i++){ //asigno n bloques mas
				bloqueSiguiente=buscarBloqueLibre();
				list_add(bloquesAsigandosAPedido,bloqueSiguiente);
			}
			 //TRUNCO TODOS LOS ARCHIVOS Y ESCRIBO TODA LA INFO
			int bloqueInicial=atoi(list_get(bloquesAsigandosAPedido,0));
			resultadoOperacion=escribirBloques(stringAGrabar, strlen(stringAGrabar), bloqueInicial,nombrePedido, bloquesAsigandosAPedido,"PEDIDO","ACTUALIZAR");
		}

	}else{// Caso2: la cantidad de bytes no es multiplo del tamaño de bloques
			//se debera validar si se desplazo dentro del ultimo bloque o si deberan pedir bloques extras
		cantBloquesAEscribir=(strlen(stringAGrabar)/infoBloques->tamBloques) +1;
		if (cantBloquesAEscribir == list_size(bloquesAsigandosAPedido)) { //Caso en que no se movio del ultimo bloque, es decir no se agregan bloques nuevos
			//sobreescribir bloques asignados anteriormente
			//TRUNCAR ARCHIVOS Y ESCRIBIR LOS BLOQUES
			int bloqueInicial=atoi(list_get(bloquesAsigandosAPedido,0));
			resultadoOperacion=escribirBloques(stringAGrabar, strlen(stringAGrabar), bloqueInicial,nombrePedido, bloquesAsigandosAPedido,"PEDIDO","ACTUALIZAR");

		} else {
			int nBloquesExtrasAAsignar=cantBloquesAEscribir - list_size(bloquesAsigandosAPedido);

			for (int i = 0;i<nBloquesExtrasAAsignar;i++) { //asigno n bloques mas
				bloqueSiguiente = buscarBloqueLibre();
				list_add(bloquesAsigandosAPedido, bloqueSiguiente);
			}
			//TRUNCO TODOS LOS ARCHIVOS Y ESCRIBO TODA LA INFO

			int bloqueInicial=atoi(list_get(bloquesAsigandosAPedido,0));
			resultadoOperacion=escribirBloques(stringAGrabar, strlen(stringAGrabar), bloqueInicial,nombrePedido, bloquesAsigandosAPedido,"PEDIDO","ACTUALIZAR");

		}

	}

	return resultadoOperacion;
//MENSAJE 3: GRABAR PLATO

}
int agregarPlatoAPedido(char* nombreRestaurante,char* nombrePedido,char* nombrePlato, int cantidad ){
	tMensajeInfoPedido* info=malloc(sizeof(tMensajeInfoPedido));
	int resultado=1;

	//char* cadenaInfoPedido=malloc((14*5)*5);
	info=obtenerInfoPedido(nombreRestaurante,nombrePedido,resultado);

	if(resultado>0){
	if(string_equals_ignore_case(info->estadoPedido,"pendiente")){
		char* stringAGrabar=armarStringNuevoAGrabar(info,nombrePlato,cantidad,nombreRestaurante,"AGREGAR_PLATO");
		resultado=grabarNuevoPedidoActualizado(stringAGrabar,nombreRestaurante,nombrePedido);

		}
	}
	return resultado;

}

int confirmarPedido(char* nombreRestaurante,char* nombrePedido,char* nombrePlato, int cantidad ){
	tMensajeInfoPedido* info=malloc(sizeof(tMensajeInfoPedido));
	int resultado=1;

	//char* cadenaInfoPedido=malloc((14*5)*5)
	info=obtenerInfoPedido(nombreRestaurante,nombrePedido,resultado);

	if(resultado>0){
		if(string_equals_ignore_case(info->estadoPedido,"pendiente")){
			char* stringAGrabar=armarStringNuevoAGrabar(info,nombrePlato,cantidad,nombreRestaurante,"AGREGAR_PLATO");
			resultado=grabarNuevoPedidoActualizado(stringAGrabar,nombreRestaurante,nombrePedido);
		}
	}
	return resultado;

}

char* buscarPlatosRestaurante(char* nombreRestaurante){

}

int obtenerBloqueInicial(char*recurso, char*nombreRecurso,char*pathRecurso,uint32_t sizePedido){
	int bloqueInicial=0;
	if(strcmp(recurso,"RESTAURANTE")==0){

		}else if(strcmp(recurso,"RECETA")==0){

			}else {
				struct stat info;
				int e=0;


				e=stat(pathRecurso,&info);

				if(e==0){

					FILE* archivoInfoPedido=fopen(pathRecurso,"r");

					char* map=malloc(info.st_size+1);
					map=mmap(0, info.st_size, PROT_READ | PROT_WRITE, MAP_SHARED,fileno(pathRecurso), 0);
					if(map==-1){
						log_error(logger,"Error en mmap de info pedido");
					}
					map[info.st_size]='\0';

					char** propiedades=string_n_split(map,2,"\n");
					bloqueInicial=atoi(propiedades[1]);
					sizePedido=atoi(propiedades[0]);

					if (munmap(map,info.st_size) == -1) {
								log_error(logger,"Error al liberar memoria mapeada");
								exit(EXIT_FAILURE);
					}

					fclose(archivoInfoPedido);
					//free(pathInfoPedido);
				}

				return bloqueInicial;
			}

}

int aumentarCantidadPlatiListo(char* nombreRestaurante, uint32_t idPedido,char* nombrePlato){
	char* pathRestaurante= malloc(strlen(pathRestaurantes)+strlen(nombreRestaurante)+1);
	struct stat infoResto;
	int eResto=0;
	uint32_t sizePedido=0;
	char* idPedidoYNombreResto=malloc(strlen(nombreRestaurante)+strlen(string_itoa(idPedido))+1+1);
	memcpy(idPedidoYNombreResto,nombreRestaurante,strlen(nombreRestaurante));
	memcpy(idPedidoYNombreResto+strlen(nombreRestaurante),"-",1);
	memcpy(idPedidoYNombreResto+strlen(nombreRestaurante)+1,string_itoa(idPedido),strlen(string_itoa(idPedido)));
	idPedidoYNombreResto[strlen(nombreRestaurante)+1+strlen(string_itoa(idPedido))]='\0';
	t_list* bloquesAsigandosAPedido=dictionary_get(diccionarioBloquesAsignadosAPedidos,idPedidoYNombreResto);

	memcpy(pathRestaurante,pathRestaurantes,strlen(pathRestaurantes));
	memcpy(pathRestaurante+strlen(pathRestaurantes), nombreRestaurante,strlen(nombreRestaurante));

	eResto=stat(pathRestaurante,&infoResto);

	if(eResto==0){
		struct stat infoPathPedido;
		int ePedido=0;
		memcpy(pathRestaurante+strlen(nombreRestaurante),"/",1);
		char* pathInfoPedido=malloc(strlen(pathRestaurante)+strlen(string_itoa(idPedido))+5+1);
		int offsetArchivoPedido=0;
		memcpy(pathInfoPedido+offsetArchivoPedido,pathRestaurante,strlen(pathRestaurante));
		offsetArchivoPedido+=strlen(pathRestaurante);
		memcpy(pathInfoPedido+offsetArchivoPedido,string_itoa(idPedido),strlen(string_itoa(idPedido)));
		offsetArchivoPedido+=strlen(string_itoa(idPedido));
		memcpy(pathInfoPedido+offsetArchivoPedido,".AFIP",5);
		offsetArchivoPedido+=5;
		pathInfoPedido[offsetArchivoPedido]='\0';

		ePedido=stat(pathInfoPedido,&infoPathPedido);

		if(ePedido){
			int bloqueInicial=obtenerBloqueInicial("PEDIDO",string_itoa(idPedido),pathInfoPedido,sizePedido);

			tMensajeInfoPedido* infoPedido=malloc(sizeof(tMensajeInfoPedido));
			infoPedido=leerBloquesPedido(bloqueInicial,sizePedido,bloquesAsigandosAPedido);

			if(string_equals_ignore_case(infoPedido->estadoPedido,"confirmado")){
					char* stringAGrabar=armarStringNuevoAGrabar(infoPedido,nombrePlato,1,nombreRestaurante,"PLATO_LISTO");
					grabarNuevoPedidoActualizado(stringAGrabar,nombreRestaurante,string_itoa(idPedido));

			}


		}else{
			log_error(logger,"El pedido no exciste dentro del restaurante solicitado");
			return -2;
		}


	}else{
		log_error(logger, "El restaurante no existe");
		return -1;
	}

return 1;

}


void mapearInfoRestauranteARespuesta(tMensajeInfoRestaurante* infoRestauranteAEnviar,t_respuesta_info_restaurante* respuestaArchivoInfoResto){
	respuestaArchivoInfoResto->cantidad_cocineros=infoRestauranteAEnviar->cantCocineros;
	char*posicionAux=malloc(strlen(infoRestauranteAEnviar->posicion)-2);
	memcpy(posicionAux,infoRestauranteAEnviar->posicion+1,strlen(infoRestauranteAEnviar->posicion));
	char** coordenadasPosicion=string_split(posicionAux,",");

	respuestaArchivoInfoResto->posicion_x=atoi(coordenadasPosicion[0]);
	respuestaArchivoInfoResto->posicion_y=atoi(coordenadasPosicion[1]);

	respuestaArchivoInfoResto->afinidad_cocineros=malloc(strlen(infoRestauranteAEnviar->afinidadCocineros)+1);
	memcpy(respuestaArchivoInfoResto->afinidad_cocineros,infoRestauranteAEnviar->afinidadCocineros,strlen(infoRestauranteAEnviar->afinidadCocineros));
	respuestaArchivoInfoResto->afinidad_cocineros[strlen(infoRestauranteAEnviar->afinidadCocineros)]='\0';
	respuestaArchivoInfoResto->size_afinidad_cocineros=strlen(infoRestauranteAEnviar->afinidadCocineros);

	respuestaArchivoInfoResto->platos=malloc(strlen(infoRestauranteAEnviar->platos)+1);
	memcpy(respuestaArchivoInfoResto->platos,infoRestauranteAEnviar->platos,strlen(infoRestauranteAEnviar->platos));
	respuestaArchivoInfoResto->platos[strlen(infoRestauranteAEnviar->platos)]='\0';
	respuestaArchivoInfoResto->size_platos = strlen(infoRestauranteAEnviar->platos);

	respuestaArchivoInfoResto->precio_platos=malloc(strlen(infoRestauranteAEnviar->preciosPlatos)+1);
	memcpy(respuestaArchivoInfoResto->precio_platos,infoRestauranteAEnviar->preciosPlatos,strlen(infoRestauranteAEnviar->preciosPlatos));
	respuestaArchivoInfoResto->size_precio_platos=strlen(infoRestauranteAEnviar->preciosPlatos);
	respuestaArchivoInfoResto->precio_platos[strlen(infoRestauranteAEnviar->preciosPlatos)]='\0';
	respuestaArchivoInfoResto->cantidad_hornos=infoRestauranteAEnviar->cantidadHornos;

	free(posicionAux);
	free(infoRestauranteAEnviar->posicion);
	free(infoRestauranteAEnviar->afinidadCocineros);
	free(infoRestauranteAEnviar->platos);
	free(infoRestauranteAEnviar->preciosPlatos);
	free(infoRestauranteAEnviar);
}


void desrializarPayloadMensajeAPlato(char* payload,tSolicitudAniadirPlato* platoAAniadir){
	uint32_t offset=0;
	uint32_t sizeNombreResto=0;
	uint32_t sizeIdPedido=0;
	uint32_t sizePlato=0;


	memcpy(&platoAAniadir->sizeNombreRestaurante,payload,sizeof(uint32_t));
	offset += sizeof(sizeof(uint32_t));
	memcpy(platoAAniadir->nombreRestaurante,payload+offset,&platoAAniadir->sizeNombreRestaurante);
	offset+=strlen(platoAAniadir->nombreRestaurante);

	memcpy(&platoAAniadir->idPedido,payload,sizeof(uint32_t));
	offset += sizeof(sizeof(uint32_t));
	memcpy(platoAAniadir->idPedido,payload+offset,platoAAniadir->idPedido);
	offset+=strlen(platoAAniadir->nombreRestaurante);

	memcpy(&platoAAniadir->sizePlato,payload,sizeof(uint32_t));
	offset += sizeof(sizeof(uint32_t));
	memcpy(platoAAniadir->plato,payload+offset,platoAAniadir->sizePlato);
	offset+=strlen(platoAAniadir->nombreRestaurante);

	memcpy(&platoAAniadir->cantidad,payload+offset,sizeof(uint32_t));


}

t_header * serializar_respuesta_platos(tRespuestaConsultaPlatos * respuestaPlatos) {
	int offset = 0;
	t_header * header = malloc(sizeof(t_header));

	int size_buffer = sizeof(uint32_t) + respuestaPlatos->sizePlatos;
	void * buffer = malloc(size_buffer);

	memcpy(buffer + offset, &respuestaPlatos->sizePlatos, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer + offset, respuestaPlatos->platos, sizeof(uint32_t));
	offset += respuestaPlatos->sizePlatos;

	header->payload = buffer;
	header->size = size_buffer;
	header->id_proceso = 5; //TODO: esta hardcodeado el 2, hay que modificarlo desp
	header->modulo = SINDICATO;
	header->nro_msg = CONSULTAR_PLATOS;

	return header;
}

t_pedido* deserializarPedido(char* payload) {
	t_pedido * pedido = malloc(sizeof(t_pedido));
	tMensajeInfoPedido* infoPedido=malloc(sizeof(tMensajeInfoPedido));
	pedido=recibir_pedido(payload);

	return pedido;


}

t_header* serializarRespuestaOperacionPedidoNuevo(int resultadoOperacion){
	t_header* header=malloc(sizeof(t_header));
	int offset = 0;
	int sizeBuffer=sizeof(int);
	void * buffer = malloc(sizeBuffer);

	memcpy(buffer+offset,&resultadoOperacion,sizeof(int));
	offset+=sizeof(int);
	header->payload = buffer;
	header->size = sizeBuffer;
	header->id_proceso = 5; //TODO: esta hardcodeado el 2, hay que modificarlo desp
	header->modulo = SINDICATO;
	header->nro_msg = GUARDAR_PEDIDO;


	return header;

}

t_header* serializarRespuestaOperacionPlatoNuevo(int resultadoOperacion){
	t_header* header=malloc(sizeof(t_header));
	int offset = 0;
	int sizeBuffer=sizeof(int);
	void * buffer = malloc(sizeBuffer);

	memcpy(buffer+offset,&resultadoOperacion,sizeof(int));
	offset+=sizeof(int);
	header->payload = buffer;
	header->size = sizeBuffer;
	header->id_proceso = 5; //TODO: esta hardcodeado el 2, hay que modificarlo desp
	header->modulo = SINDICATO;
	header->nro_msg = GUARDAR_PLATO;


	return header;

}

t_header* serializarRespuestaReceta(tRespuestaReceta* receta){
	int offset = 0;
	t_header * header = malloc(sizeof(t_header));

	int sizeBuffer = sizeof(uint32_t) * 2 + receta->sizePasos+receta->sizeTiempos;
	void * buffer = malloc(sizeBuffer);

	memcpy(buffer+offset,&receta->sizePasos,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(buffer+offset,receta->pasos,receta->sizePasos);
	offset+=receta->sizePasos;

	memcpy(buffer+offset,&receta->sizeTiempos,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(buffer+offset,receta->tiempos,&receta->sizeTiempos);
	offset+=&receta->sizeTiempos;

	header->payload = buffer;
	header->size = sizeBuffer;
	header->id_proceso = 8; //TODO: esta hardcodeado el 2, hay que modificarlo desp
	header->modulo = SINDICATO;
	header->nro_msg = OBTENER_RECETA;

	return header;

}

void mapearInfoRecetaARespuesta(tMensajeInfoReceta* infoReceta,tRespuestaReceta* respuestaReceta){

		respuestaReceta->sizePasos=strlen(infoReceta->pasos);
		memcpy(respuestaReceta->pasos,infoReceta->pasos,strlen(infoReceta->pasos));

		respuestaReceta->sizeTiempos=strlen(infoReceta->tiempos);
		memcpy(respuestaReceta->sizeTiempos,infoReceta->tiempos,strlen(infoReceta->tiempos));

		free(infoReceta->pasos);
		free(infoReceta->tiempos);
		free(infoReceta);



}

void deserializarPayloadPedido(char* payload,tSolicitudPedido* solicitudPedido){
	uint32_t offset = 0;
	uint32_t sizeNombreResto = 0;
	uint32_t sizeIdPedido = 0;
	uint32_t sizePlato = 0;

	memcpy(&solicitudPedido->sizeNombreRestaurante, payload, sizeof(uint32_t));
	offset += sizeof(sizeof(uint32_t));
	memcpy(solicitudPedido->nombreRestaurante, payload + offset,solicitudPedido->sizeNombreRestaurante);
	offset += solicitudPedido->sizeNombreRestaurante;


}

t_header* serializarRespuestaPedido(tRespuestaSolicitudPedido* respuestaPedido){

	int offset = 0;
	t_header * header = malloc(sizeof(t_header));

	int sizeBuffer = sizeof(uint32_t)*5+ respuestaPedido->sizeListaPlatos+respuestaPedido->sizeEstadoPedido+respuestaPedido->sizeCantidadPlatos+
			respuestaPedido->sizeCantidadLista;
	void * buffer = malloc(sizeBuffer);

	memcpy(buffer+offset,&respuestaPedido->sizeEstadoPedido,sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(buffer+offset,respuestaPedido->estadoPedido,respuestaPedido->sizeEstadoPedido);
	offset+=respuestaPedido->sizeEstadoPedido;

	memcpy(buffer+offset,&respuestaPedido->sizeListaPlatos,sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(buffer+offset,respuestaPedido->listaPlatos,respuestaPedido->sizeListaPlatos);
	offset+=respuestaPedido->sizeListaPlatos;

	memcpy(buffer+offset,&respuestaPedido->sizeCantidadPlatos,sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(buffer+offset,respuestaPedido->cantidadPlatos,respuestaPedido->sizeCantidadPlatos);
	offset+=respuestaPedido->sizeCantidadPlatos;

	memcpy(buffer+offset,&respuestaPedido->sizeCantidadLista,sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(buffer+offset,respuestaPedido->cantidadLista,respuestaPedido->sizeCantidadLista);
	offset+=respuestaPedido->sizeCantidadLista;

	header->payload = buffer;
	header->size = sizeBuffer;
	header->id_proceso = 5; //TODO: esta hardcodeado el 2, hay que modificarlo desp
	header->modulo = SINDICATO;
	header->nro_msg = OBTENER_RECETA;

	return header;


}
void mapearInfoPedidoARespuesta(tMensajeInfoPedido* infoPedido,tRespuestaSolicitudPedido*respuesta){
	respuesta->sizeEstadoPedido=strlen(infoPedido->estadoPedido);
	respuesta->sizeListaPlatos=strlen(infoPedido->listaPlatos);
	respuesta->sizeCantidadPlatos=strlen(infoPedido->cantidadPlatos);
	respuesta->sizeCantidadLista=strlen(infoPedido->cantidadLista);
	respuesta->precioTotal=infoPedido->precioTotal;

	respuesta->estadoPedido=malloc(respuesta->sizeEstadoPedido+1);
	memcpy(respuesta->estadoPedido,infoPedido->estadoPedido,respuesta->sizeEstadoPedido);
	respuesta->listaPlatos=malloc(respuesta->sizeListaPlatos+1);
	memcpy(respuesta->listaPlatos,infoPedido->listaPlatos,respuesta->sizeListaPlatos);
	respuesta->cantidadPlatos=malloc(respuesta->sizeCantidadPlatos+1);
	memcpy(respuesta->cantidadPlatos,infoPedido->cantidadPlatos,respuesta->sizeCantidadPlatos);
	respuesta->cantidadLista=malloc(respuesta->sizeCantidadLista+1);
	memcpy(respuesta->cantidadLista,infoPedido->cantidadLista,respuesta->sizeCantidadLista);

	free(infoPedido->estadoPedido);
	free(infoPedido->listaPlatos);
	free(infoPedido->cantidadPlatos);
	free(infoPedido->cantidadLista);
	free(infoPedido);


}

void deserializarPayloadPlatoListo(char* payload,tSolicitudPlatiListo* solicitudPlatoListo){
	uint32_t offset = 0;
	uint32_t sizeNombreResto = 0;
	uint32_t sizeIdPedido = 0;
	uint32_t sizePlato = 0;

	memcpy(&solicitudPlatoListo->sizeNombreRestaurante, payload, sizeof(uint32_t));
	offset += sizeof(sizeof(uint32_t));
	memcpy(solicitudPlatoListo->nombreRestaurante, payload + offset,solicitudPlatoListo->sizeNombreRestaurante);
	offset += solicitudPlatoListo->sizeNombreRestaurante;

	memcpy(&solicitudPlatoListo->idPedido, payload+offset, sizeof(uint32_t));
	offset += sizeof(sizeof(uint32_t));


	memcpy(solicitudPlatoListo->sizeNombrePlatoListo, payload + offset,sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(solicitudPlatoListo->nombrePlatoListo, payload + offset,solicitudPlatoListo->sizeNombrePlatoListo);
	offset += solicitudPlatoListo->sizeNombrePlatoListo;


}

t_header* serializarRespuestaPlatoListo(int resultadoOperacion){
	int offset = 0;
	t_header * header = malloc(sizeof(t_header));

	int sizeBuffer = sizeof(int);
	void * buffer = malloc(sizeBuffer);

	memcpy(buffer+offset,&resultadoOperacion,sizeof(int));
	offset+=sizeof(int);

	header->payload = buffer;
	header->size = sizeBuffer;
	header->id_proceso = 7; //TODO: esta hardcodeado el 2, hay que modificarlo desp
	header->modulo = SINDICATO;
	header->nro_msg = PLATO_LISTO;

	return header;
}
void handleConexion(int socketCliente,tInfoBloques* infoBloques) {
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

	case CONSULTAR_PLATOS:{
		char* restauranteBuscado=malloc(strlen(headerRecibido->payload));
		tMensajeInfoRestaurante *infoRestaurante=malloc(sizeof(tMensajeInfoRestaurante));
		tRespuestaConsultaPlatos* respuestaConsultaPlatos=malloc(sizeof(tRespuestaConsultaPlatos));
		infoRestaurante=obtenerInfoRestaurante(restauranteBuscado);
		respuestaConsultaPlatos->platos=malloc(strlen(infoRestaurante->platos)+1);
		memcpy(respuestaConsultaPlatos->platos,infoRestaurante->platos,strlen(infoRestaurante->platos));
		respuestaConsultaPlatos->platos[strlen(infoRestaurante->platos)]='\0';
		respuestaConsultaPlatos->sizePlatos=strlen(respuestaConsultaPlatos->platos);
		t_header * header = malloc(sizeof(t_header));

		header=serializar_respuesta_platos(respuestaConsultaPlatos);
		if (enviar_buffer(socketCliente, header) == false) {
					log_error(logger,"No se pudo enviar la respuesta al pedido de info del restaurante");
		}

		free(restauranteBuscado);
		free(infoRestaurante->posicion);
		free(infoRestaurante->afinidadCocineros);
		free(infoRestaurante->platos);
		free(infoRestaurante->preciosPlatos);
		free(infoRestaurante);
		free(respuestaConsultaPlatos->platos);
		free(respuestaConsultaPlatos);
		break;
	}

	case GUARDAR_PEDIDO:{
		t_pedido* pedido=malloc(sizeof(t_pedido));
		pedido=deserializarPedido(headerRecibido->payload);
		tCreacionPedido* infoPedido=malloc(sizeof(tCreacionPedido));

		infoPedido->estadoPedido=malloc(strlen("pendiente"));
		memcpy(infoPedido->estadoPedido,"pendiente",strlen("pendiente"));

		infoPedido->listaPlatos=(char*)malloc(50 * sizeof(char));
		memcpy(infoPedido->listaPlatos,"[]",2);

		infoPedido->cantidadPlatos=(char*)malloc(50 * sizeof(char));
		memcpy(infoPedido->cantidadPlatos,"[]",2);

		infoPedido->cantidadLista=(char*)malloc(50*sizeof(char));
		memcpy(infoPedido->cantidadLista,"[]",2);

		infoPedido->precioTotal=0;

		int resultadoOperacion=0;

		resultadoOperacion=crearPedido(infoPedido,pedido->nombre_restaurante,string_itoa(pedido->id_pedido),infoBloques);

		t_header* header=malloc(sizeof(t_header));

		header=serializarRespuestaOperacionPedidoNuevo(resultadoOperacion);

		if (enviar_buffer(socketCliente, header) == false) {
							log_error(logger,"No se pudo enviar la respuesta al pedido de info del restaurante");
		}

		free(pedido->nombre_restaurante);
		free(pedido);
		free(infoPedido->estadoPedido);
		free(infoPedido->listaPlatos);
		free(infoPedido->cantidadPlatos);
		free(infoPedido->cantidadLista);

		break;

	}

		case OBTENER_RESTAURANTES: {
		/*t_header * respuesta_hardcodeada = malloc(sizeof(t_header));
		t_respuesta_info_restaurante * resp_resto_hard = malloc(sizeof(t_respuesta_info_restaurante));

		t_respuesta_info_restaurante* respuestaArchivoInfoResto= malloc(sizeof(t_respuesta_info_restaurante));

		leerInfoDeResto(headerRecibido->payload);

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
				resp_resto_hard);*/

		tMensajeInfoRestaurante* infoRestauranteAEnviar=malloc(sizeof(tMensajeInfoRestaurante));
		t_respuesta_info_restaurante* respuestaArchivoInfoResto= malloc(sizeof(t_respuesta_info_restaurante));

		infoRestauranteAEnviar=obtenerInfoRestaurante(headerRecibido->payload);
		mapearInfoRestauranteARespuesta(infoRestauranteAEnviar,respuestaArchivoInfoResto);

		t_header * header = malloc(sizeof(t_header));

		header=serializar_respuesta_info_restaurante(respuestaArchivoInfoResto);


		if (enviar_buffer(socketCliente, header) == false) {
			log_error(logger,
					"No se pudo enviar la respuesta al pedido de info del restaurante");
		}


		free(infoRestauranteAEnviar);
		break;
	}

		case GUARDAR_PLATO:{
			t_guardar_plato* platoNuevo=malloc(sizeof(t_guardar_plato));
			platoNuevo=recibir_guardar_plato(headerRecibido->payload);
			int resultadoOperacion=agregarPlatoAPedido(platoNuevo->pedido->nombre_restaurante,string_itoa(platoNuevo->pedido->id_pedido),platoNuevo->nombre_plato,platoNuevo->cantidad_plato);
			t_header* header=malloc(sizeof(t_header));

			header=serializarRespuestaOperacionPlatoNuevo(resultadoOperacion);

			if (enviar_buffer(socketCliente, header) == false) {
					log_error(logger,
					"No se pudo enviar la respuesta al pedido de info del restaurante");
			}

			free(platoNuevo->nombre_plato);
			free(platoNuevo);
			free(header->payload);
			free(header);

			break;
		}
		case CONFIRMAR_PEDIDO:{
			tMensajeInfoPedido* infoPedido=malloc(sizeof(tMensajeInfoPedido));
			tSolicitudPedido* solicitudPedido=malloc(sizeof(tSolicitudPedido));
			tRespuestaSolicitudPedido* respuestaPedido=malloc(sizeof(tRespuestaSolicitudPedido));

			deserializarPayloadPedido(headerRecibido->payload,solicitudPedido);

			int resultado=confirmarPedido(solicitudPedido->nombreRestaurante,string_itoa(solicitudPedido->idPedido),NULL,0);
			t_header* header=malloc(sizeof(t_header));

			header=serializarRespuestaOperacionPlatoNuevo(resultado);

			if (enviar_buffer(socketCliente, header) == false) {
				log_error(logger,"No se pudo enviar la respuesta al pedido de info del restaurante");
			}

			break;
		}

		case OBTENER_PEDIDO:{
			tMensajeInfoPedido* infoPedido=malloc(sizeof(tMensajeInfoPedido));
			tSolicitudPedido* solicitudPedido=malloc(sizeof(tSolicitudPedido));
			tRespuestaSolicitudPedido* respuestaPedido=malloc(sizeof(tRespuestaSolicitudPedido));

			deserializarPayloadPedido(headerRecibido->payload,solicitudPedido);
			int resultado=1;

			infoPedido=obtenerInfoPedido(solicitudPedido->nombreRestaurante,string_itoa(solicitudPedido->idPedido),resultado);

			mapearInfoPedidoARespuesta(infoPedido,respuestaPedido);

			t_header* header=serializarRespuestaPedido(respuestaPedido);

			if (enviar_buffer(socketCliente, header) == false) {
				log_error(logger,"No se pudo enviar la respuesta al pedido de info del restaurante");

			}

			free(solicitudPedido->nombreRestaurante);
			free(solicitudPedido);
			free(respuestaPedido->estadoPedido);
			free(respuestaPedido->listaPlatos);
			free(respuestaPedido->cantidadPlatos);
			free(respuestaPedido->cantidadLista);
			free(respuestaPedido);


			break;
		}

		case PLATO_LISTO:{
			tSolicitudPlatiListo* solicitudPlatoListo=malloc(sizeof(tSolicitudPlatiListo));
			deserializarPayloadPlatoListo(headerRecibido->payload,solicitudPlatoListo);
			int resultadoOperacion=1;

			resultadoOperacion=aumentarCantidadPlatiListo(solicitudPlatoListo->nombreRestaurante,solicitudPlatoListo->idPedido,solicitudPlatoListo->nombrePlatoListo);

			t_header* header=serializarRespuestaPlatoListo(resultadoOperacion);
			if (enviar_buffer(socketCliente, header) == false) {
				log_error(logger,"No se pudo enviar la respuesta al pedido de info del restaurante");
			}

			free(solicitudPlatoListo->nombreRestaurante);
			free(solicitudPlatoListo->nombrePlatoListo);
			free(solicitudPlatoListo);
			free(header->payload);
			free(header);
			break;
		}

		case OBTENER_RECETA:{

			tMensajeInfoReceta* infoReceta=malloc(sizeof(tMensajeInfoReceta));
			tRespuestaReceta* respuestaReceta=malloc(sizeof(tRespuestaReceta));
			tSolicitudPedido* solicitudPedido=malloc(sizeof(tSolicitudPedido));

			infoReceta=obtenerInfoReceta(headerRecibido->payload);
			mapearInfoRecetaARespuesta(infoReceta,respuestaReceta);


			t_header* header=malloc(sizeof(t_header));

			header=serializarRespuestaReceta(respuestaReceta);

			if (enviar_buffer(socketCliente, header) == false) {
				log_error(logger,"No se pudo enviar la respuesta al pedido de info del restaurante");

			}

			free(respuestaReceta->pasos);
			free(respuestaReceta->tiempos);
			free(header->payload);
			free(header);


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

void mensajeConsultasPlatosPrueba(char* nombreRestaurante){
			char* restauranteBuscado=malloc(10);
			memcpy(restauranteBuscado,nombreRestaurante,strlen(nombreRestaurante));
			restauranteBuscado[strlen(nombreRestaurante)]='\0';
			tMensajeInfoRestaurante *infoRestaurante=malloc(sizeof(tMensajeInfoRestaurante));
			tRespuestaConsultaPlatos* respuestaConsultaPlatos=malloc(sizeof(tRespuestaConsultaPlatos));
			infoRestaurante=obtenerInfoRestaurante(restauranteBuscado);
			respuestaConsultaPlatos->platos=malloc(strlen(infoRestaurante->platos)+1);
			memcpy(respuestaConsultaPlatos->platos,infoRestaurante->platos,strlen(infoRestaurante->platos));
			respuestaConsultaPlatos->platos[strlen(infoRestaurante->platos)]='\0';
			int sizePlatos=strlen(infoRestaurante->platos);
			respuestaConsultaPlatos->sizePlatos=sizePlatos;
			t_header * header = malloc(sizeof(t_header));

			header=serializar_respuesta_platos(respuestaConsultaPlatos);

			free(restauranteBuscado);
			free(infoRestaurante->posicion);
			free(infoRestaurante->afinidadCocineros);
			free(infoRestaurante->platos);
			free(infoRestaurante->preciosPlatos);
			free(infoRestaurante);
			free(respuestaConsultaPlatos->platos);
			free(respuestaConsultaPlatos);
}
int main(int argc, char *argv[]) {

	cargarConfiguracion();
	tInfoBloques* infoBloques=malloc(sizeof(tInfoBloques));
	infoBloques=importarInfoBloques();

	asignarPaths();

	montarFS(infoBloques);
	t_respuesta_info_restaurante* solicitudInfoResto=malloc(sizeof(t_respuesta_info_restaurante));

	tMensajeInfoRestaurante* info=malloc(sizeof(tMensajeInfoRestaurante));
	//info=obtenerInfoRestaurante("Resto1");


	//hardcodearPedido(pedidoCreado);

	//crearPedido(pedidoCreado,"Resto2","Mila1");

	char* nombreResto=malloc(30);
	strcpy(nombreResto,"Resto1");
	//agregarPlatoAPedido(nombreResto,"Mila1","Milanesa",2);

	levantarConsola(infoBloques);

	//mensajeConsultasPlatosPrueba("LaParri");


	//Escucho conexiones
	log_info(logger, "Inicio Escucha de conexiones...");
	int socketServer = crear_socket_escucha("127.0.0.1",
			configuracion->puertoEscucha);

	while (1) {
		int socketConectado = aceptar_conexion(socketServer);
		handleConexion(socketConectado,infoBloques);

		break;

	}

	free(pathFiles);
	free(pathBloques);
	free(pathMetadata);

	return 1;
}
