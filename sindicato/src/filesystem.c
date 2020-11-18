/*
 * filesystem.c
 *
 *  Created on: 4 sep. 2020
 *      Author: utnso
 */

#ifndef FILESYSTEM_C_
#define FILESYSTEM_C_
#include "filesystem.h"

int crearDirectorioFiles(){

	int e;
	struct  stat info;
	log_info(logger, "Creando directorio de files...");


	e = stat(configuracion->puntoMontaje, &info);

	mkdir(pathFiles, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

	if(e!=0){
		log_error(logger,"Se produjo un error al crear el directorio. [%d - %s]", errno, strerror(errno));
		return 0;
	}else
		log_info(logger,"Se creo el directorio para almacenar los restaurantes y las recetas");


	return 1;

}




int crearDirectorioBloques(){
	int e = 0;

	e = mkdir(pathBloques, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

	if (e != 0) {
		log_error(logger, "Error al crear el directorio de Bloques");
		return 0;

	} else
		log_info(logger, "Directorio de bloques creado");

	return 1;

}

int crearDirectorioMetadata(){
	int e=0;
	e=mkdir(pathMetadata, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

	if(e!=0){
		log_error(logger,"Error al crear el directorio Metadata");
		return 0;

	}else
		log_info(logger,"Directorio creado");

	return 1;
}

void crearArchivoMetadata(int blockSize,int cantBloques,char* magicNumber){
	char* propiedades = malloc(strlen("SIZE=BLOQUES=") + 60);
	char* pathArchivoMetadata=string_new();
	string_append(&pathArchivoMetadata,pathMetadata);
//	strcpy(pathArchivoMetadata,pathMetadata);
//	strcat(pathArchivoMetadata, "/Metadata");
//	strcat(pathArchivoMetadata, ".bin");
	string_append(&pathArchivoMetadata,"Metadata");
	string_append(&pathArchivoMetadata,".bin");


	FILE* archivoMetadata = fopen(pathArchivoMetadata, "w");
	int length=sprintf(propiedades,"%s%d%s%s%d%s%s%s","BLOCK_SIZE=",blockSize,"\n","BLOCKS=",cantBloques,"\n","MAGIC_NUMBER=",magicNumber);

	fwrite(propiedades,length,1,archivoMetadata);

	free(propiedades);
	free(pathArchivoMetadata);

	fclose(archivoMetadata);

}

int crearDirectorioRestaurantes(){
	int e = 0;

	e = mkdir(pathRestaurantes, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

	if (e != 0) {
		log_error(logger, "Error al crear el directorio de restaurantes");
		return 0;

	} else
		log_info(logger, "Directorio de restaurantes creado");

	return 1;

}

int crearDirectorioRecetas(){
	int e = 0;
	pathRecetas=string_new();

	string_append_with_format(&pathRecetas,"%s%s",pathFiles,"Recetas/");

	e = mkdir(pathRecetas, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

	if (e != 0) {
		log_error(logger, "Error al crear el directorio de recetas");
		return 0;

	} else
		log_info(logger, "Directorio de recetas creado");

	free(pathRecetas);
	return 1;

}

void crearBloques(int cantBloques){
	log_info(logger,"Creando archivos de bloques...");
	int i;
	char* pathBloqueActual=malloc(60);
	for(i=0;i<=cantBloques;i++){
		strcpy(pathBloqueActual,pathBloques);
		string_append_with_format(&pathBloqueActual,"%d%s",i,".bin");
		FILE* archivoRestauranteNuevo = fopen(pathBloqueActual, "w");
		fclose(archivoRestauranteNuevo);
		strcpy(pathBloqueActual,"");
	}

	log_info(logger,"Archivos de bloques creados.");
	free(pathBloqueActual);

}

void leerYMapearArchivoBitmap(tInfoBloques* infoBloques){
	log_info(logger,"Mapeando Archivo Bitmap...");
	int tamBitmap=0;
	char* pathArchivoBitmap=malloc(60);
	strcpy(pathArchivoBitmap,pathMetadata);
	string_append(&pathArchivoBitmap,"Bitmap.bin");
	FILE* archivoBitmap= fopen(pathArchivoBitmap,"r+");

	fdArchivoBitmap=fileno(archivoBitmap);
	tamBitmap=(((sizeof(char))*infoBloques->cantBloques)/8)+1;


	mapBitArray = mmap(NULL, tamBitmap, PROT_READ | PROT_WRITE, MAP_SHARED,fdArchivoBitmap, 0);

	bitMap = bitarray_create_with_mode(mapBitArray, tamBitmap, LSB_FIRST);
	if (bitMap == NULL)
		log_error(logger, "No se creo correctamente el bitarray");

	log_info(logger, "La cantidad de bytes del bitmap es %d",bitarray_get_max_bit(bitMap));

	free(pathArchivoBitmap);
}

int crearArchivoBitmap(tInfoBloques* infoBloques){
	log_info(logger,"Creando bitmap...");
	struct stat info;
	int e=0;
	char* pathArchivoBitmap=malloc(60);
	strcpy(pathArchivoBitmap,pathMetadata);
	string_append(&pathArchivoBitmap,"Bitmap.bin");
	int tamBitmap=0;

	e = stat(pathArchivoBitmap, &info);

	if (e == 0) {
		if (info.st_mode & S_IFREG) {
			log_error(logger, "El punto de montaje es un archivo.");
			return 0;
		}
		if (info.st_mode & S_IFDIR)
			log_warning(logger, "El bitmap ya existe...saliendo");
	} else {
		if (errno == ENOENT) {
			log_info(logger, "El Bitmap no existe. Se creará el archivo.");

			log_info(logger, "Creando Archivo Bitmap...");
			FILE* archivoBitmap = fopen(pathArchivoBitmap, "wb+");

			if (archivoBitmap != NULL) {
				fdArchivoBitmap = fileno(archivoBitmap);
				tamBitmap = (((sizeof(char)) * infoBloques->cantBloques) / 8)
						+ 1;

				if (ftruncate(fdArchivoBitmap, tamBitmap) == -1)
					log_error(logger, "No se pudo truncar el archivo");

				//void * bitmap_zone=malloc(size_bitmap);

				mapBitArray = mmap(NULL, tamBitmap, PROT_READ | PROT_WRITE,
						MAP_SHARED, fdArchivoBitmap, 0);

				bitMap = bitarray_create_with_mode(mapBitArray, tamBitmap,
						LSB_FIRST);
				if (bitMap == NULL)
					log_error(logger, "No se creo correctamente el bitarray");

				log_info(logger, "La cantidad de bytes del bitmap es %d",
						bitarray_get_max_bit(bitMap));

				for (off_t i = 1; i < infoBloques->cantBloques; i++) {
					bitarray_clean_bit(bitMap, i);
				}
				msync(mapBitArray, fdArchivoBitmap, MS_SYNC);

			}

		} else {
			log_error(logger,
					"Se produjo un error accediendo al archivo. [%d - %s]",
					errno, strerror(errno));
			return 0;
		}
	}

	free(pathArchivoBitmap);

}
void importarInfoBloques(tInfoBloques* infoBloques){
	char* pathArchivoInfoBloques= malloc(100);
	strcpy(pathArchivoInfoBloques,"/home/utnso/tp-2020-2c-Solo-C/sindicato/");
	string_append(&pathArchivoInfoBloques,"infoBloques.bin");
	char* map=malloc(200);
	struct stat info;
	stat(pathArchivoInfoBloques, &info);

	FILE* archivoInfoBloques=fopen(pathArchivoInfoBloques,"r+");

	map=mmap(0,info.st_size,PROT_READ | PROT_WRITE, MAP_SHARED, fileno(archivoInfoBloques), 0);

	char** propiedades= string_split(map,",");

	infoBloques->tamBloques=atoi(propiedades[0]);
	infoBloques->cantBloques=atoi(propiedades[1]);
	infoBloques->magicNumber=propiedades[2];

	if (munmap(map,info.st_size) == -1) {
				log_error(logger,"Error al liberar memoria mapeada");
				exit(EXIT_FAILURE);

	}

	//free(pathArchivoInfoBloques);
	//free(map);


}

void cargarBloquesAsigandosAResto(){

	struct stat info;
	int e=0;
	char* pathArchivoBloquesAsignados= malloc(100);
	strcpy(pathArchivoBloquesAsignados,"/home/utnso/tp-2020-2c-Solo-C/sindicato/");
	string_append(&pathArchivoBloquesAsignados,"bloquesAsignadosARestos.bin");

	e=stat(pathArchivoBloquesAsignados, &info);
	int i=0; //Para recorrer los restaurantes
	int j=0; //para recorrer los bloques asignados

	if(e==0){

		if(info.st_size!=0){
		FILE* archivoInfoBloques = fopen(pathArchivoBloquesAsignados, "r+");
		char* map=malloc(info.st_size);
		map = mmap(0, info.st_size, PROT_READ | PROT_WRITE, MAP_SHARED,fileno(archivoInfoBloques), 0);

		char** restos = string_split(map, "/n");

		while(restos[i]!=NULL){
			t_list* bloquesAsignadosAResto=list_create();

			char** propiedadesResto= string_split(restos[i]," "); //propiedades= Nombre + bloques
			char** bloquesResto= string_split(propiedadesResto[1],","); //lista de bloques separados por coma

			while(bloquesResto[j]!=NULL){
				list_add(bloquesAsignadosAResto,bloquesResto[j]);
				j++;
			}
			dictionary_put(diccionarioBloquesAsignadosARestos,propiedadesResto[0],bloquesAsignadosAResto);
			i++;
		}


		if (munmap(map, info.st_size) == -1) {
			log_error(logger, "Error al liberar memoria mapeada");
			exit(EXIT_FAILURE);

		}


		}


	}
	free(pathArchivoBloquesAsignados);


}

void cargarBloquesAsigandosAPedidos(){

	struct stat info;
	int e=0;
	char* pathArchivoBloquesAsignados= malloc(100);
	strcpy(pathArchivoBloquesAsignados,"/home/utnso/tp-2020-2c-Solo-C/sindicato/");
	string_append(&pathArchivoBloquesAsignados,"bloquesAsignadosAPedidos.bin");

	e=stat(pathArchivoBloquesAsignados, &info);
	int i=0; //Para recorrer los restaurantes
	int j=0; //para recorrer los bloques asignados

	if(e==0){
		if(info.st_size!=0){
		FILE* archivoInfoBloques = fopen(pathArchivoBloquesAsignados, "r+");
		char* map=malloc(info.st_size);
		map = mmap(0, info.st_size, PROT_READ | PROT_WRITE, MAP_SHARED,fileno(archivoInfoBloques), 0);

		char** pedidos = string_split(map, "/n");

		while(pedidos[i]!=NULL){
			t_list* bloquesAsignadosAPedido=list_create();

			char** propiedadesPedido= string_split(pedidos[i]," "); //propiedades= Nombre + bloques
			char** bloquesPedido= string_split(propiedadesPedido[1],","); //lista de bloques separados por coma

			while(bloquesPedido[j]!=NULL){
				list_add(bloquesAsignadosAPedido,bloquesPedido[j]);
				j++;
			}
			dictionary_put(diccionarioBloquesAsignadosAPedidos,propiedadesPedido[0],bloquesAsignadosAPedido);
			i++;
		}


		if (munmap(map, info.st_size) == -1) {
			log_error(logger, "Error al liberar memoria mapeada");
			exit(EXIT_FAILURE);

		}


		}

	}

}

int montarFS(tInfoBloques* infoBloques,char* magicNumber){

		int e;
		struct stat info;

		log_info(logger,"Preparando punto de montaje.");
		e = stat(configuracion->puntoMontaje, &info);

		if (e == 0) {
			if (info.st_mode & S_IFREG) {
				log_error(logger,"El punto de montaje es un archivo.");
				return 0;
			}
			if (info.st_mode & S_IFDIR)
				log_info(logger,"Punto de montaje encontrado.");
				infoBloques=leerInfoBloques();
				leerYMapearArchivoBitmap(infoBloques);
				cargarBloquesAsigandosAResto();
				//cargarBloquesAsigandosARecetas();
				cargarBloquesAsigandosAPedidos();


		}
		else {
			if (errno == ENOENT) {
				log_warning(logger,"El punto de montaje no existe. Se creará el directorio.");
				e = mkdir(configuracion->puntoMontaje, ACCESSPERMS | S_IRWXU);
				if (e != 0) {
					log_error(logger,"Se produjo un error al crear el directorio. [%d - %s]", errno, strerror(errno));
					return 0;
				}
				else
					log_info(logger,"El directorio se creó satisfactoriamente.");
					int statusMetadata = crearDirectorioMetadata();
					int statusFiles = crearDirectorioFiles();
					int statusBloques = crearDirectorioBloques();
					int statusRestaurantes = crearDirectorioRestaurantes();
					int statusRecetas = crearDirectorioRecetas();
					crearArchivoMetadata(infoBloques->tamBloques, infoBloques->cantBloques, magicNumber);
					crearBloques(infoBloques->cantBloques);
					crearArchivoBitmap(infoBloques);

			}
			else {
				log_error(logger,"Se produjo un error accediendo al punto de montaje. [%d - %s]", errno, strerror(errno));
				return 0;
			}
		}
		return 1;
	}

int buscarBloqueLibre(){

	int i=0;

	while(bitarray_test_bit(bitMap,i)&&i<infoBloques->cantBloques+1) i++;

	bitarray_set_bit(bitMap,i);

	if( -1 == msync(mapBitArray,fdArchivoBitmap,MS_SYNC) )
		log_error(logger,"No se actualizo el bitmap en el archivo");

	log_info(logger,"la posicion del bitmap seleccionado es  %d",i);

	return i;

}
void escribirBloques(char*propiedades,uint32_t cantEscritura, int bloqueInicial,char* nombreRecurso,t_list* bloquesAsignadosARecurso,char* tipoRecurso,char* operacion){
	int e=0;
	int bloqueActual=0;
	int bloqueSiguiente=0;
	int offsetEscritura=0;
	int offsetFinCadena= strlen(propiedades);
	int i;
	int j;


	uint32_t cantBloquesAEscribir=0;

	char* stringBloquesAsignados=string_new();
	strcpy(stringBloquesAsignados,"");
	char* lineaArchivoBloquesAsignados=string_new(); //malloc((list_size(bloquesAsignadosARecurso)*2) + strlen(nombreRecurso));
	char* pathArchivoBloquesAsignados=string_new();

	//AGREGAR VERIFICACION DE BLOQUES LIBRES EN LAS SIGUINTES CONDICIONES

	if((cantEscritura % infoBloques->tamBloques)==0 && cantEscritura==infoBloques->tamBloques){
		cantBloquesAEscribir= (cantEscritura)/ (infoBloques->tamBloques);
		bloqueActual=bloqueInicial;
		char* pathBloqueActual=malloc(strlen(pathBloques)+strlen(string_itoa(bloqueActual))+4+1);
		memcpy(pathBloqueActual,pathBloques,strlen(pathBloques));
		struct stat infoArchivo;
		e = stat(pathBloqueActual, &infoArchivo);
		int offset=0;
		offset=offset+strlen(pathBloques);
		//string_append_with_format(&pathBloqueActual,"%d%s",bloqueActual,".bin");
		memcpy(pathBloqueActual+offset,string_itoa(bloqueActual),strlen(string_itoa(bloqueActual)));
		offset=offset+strlen(string_itoa(bloqueActual));
		memcpy(pathBloqueActual+offset,".bin",4);
		offset=offset+4;

		pathBloqueActual[offset]='\0';

		FILE* archivoBloqueActual=fopen(pathBloqueActual,"w");

			if(fwrite(propiedades,cantEscritura,1,archivoBloqueActual)==0){
				log_error(logger,"Error al escribir en el archivo info de restaurante");
			}

	}else {
		int sizeStringConPunteros=cantEscritura+ (((cantEscritura) / (infoBloques->tamBloques))*sizeof(uint32_t));

		cantBloquesAEscribir= ((sizeStringConPunteros) / (infoBloques->tamBloques)) +1;

		bloqueActual=bloqueInicial;

		for (i=0; i<cantBloquesAEscribir;i++){
			if(i<cantBloquesAEscribir-1){
				if(string_equals_ignore_case(operacion,"ACTUALIZAR")){
					bloqueSiguiente=atoi(list_get(bloquesAsignadosARecurso,i+1));

				}else bloqueSiguiente=buscarBloqueLibre();

				list_add(bloquesAsignadosARecurso,bloqueSiguiente);
				if(string_equals_ignore_case(tipoRecurso,"RESTO")){
					dictionary_put(diccionarioBloquesAsignadosARestos,nombreRecurso,bloquesAsignadosARecurso);
				}else if(string_equals_ignore_case(tipoRecurso,"RECETA")){
					dictionary_put(diccionarioBloquesAsignadosARecetas,nombreRecurso,bloquesAsignadosARecurso);

				}else dictionary_put(diccionarioBloquesAsignadosAPedidos,nombreRecurso,bloquesAsignadosARecurso);

				char* bytesAEscribir=malloc(infoBloques->tamBloques+1);

				bytesAEscribir=string_substring(propiedades,offsetEscritura,infoBloques->tamBloques-sizeof(uint32_t));
				char* stringCeros=malloc(sizeof(uint32_t)-strlen(string_itoa(bloqueSiguiente))+1);
				int offsetCeros=0;
				offsetCeros=strlen(bytesAEscribir);
				strcpy(stringCeros,"");
				stringCeros=string_repeat('0',sizeof(uint32_t)-strlen(string_itoa(bloqueSiguiente)));
				stringCeros[strlen(stringCeros)]='\0';
				//string_append_with_format(&bytesAEscribir,"%s%d",,bloqueSiguiente);
				memcpy(bytesAEscribir+offsetCeros,stringCeros,strlen(stringCeros));
				offsetCeros=offsetCeros+strlen(stringCeros);
				memcpy(bytesAEscribir+offsetCeros,string_itoa(bloqueSiguiente),strlen(string_itoa(bloqueSiguiente)));
				offsetCeros=offsetCeros+strlen(string_itoa(bloqueSiguiente));
				bytesAEscribir[offsetCeros]='\0';
				//string_append_with_format(&propiedades,"%d",bloqueSiguiente); //ESCRIBO EL SIZE DE PROPIEDADES+SIZE BLOQUE SIGUIENTE
				//string_append_with_format(&pathBloqueActual,"%d%s",bloqueActual,".bin");
				int offset=0;
				char* pathBloqueActual=malloc(strlen(pathBloques)+strlen(string_itoa(bloqueActual))+4+1);
				memcpy(pathBloqueActual,pathBloques,strlen(pathBloques));
				offset=offset+strlen(pathBloques);
				memcpy(pathBloqueActual+offset,string_itoa(bloqueActual),strlen(string_itoa(bloqueActual)));
				offset=offset+strlen(string_itoa(bloqueActual));
				memcpy(pathBloqueActual+offset,".bin",4);
				offset=offset+4;
				pathBloqueActual[offset]='\0';

				FILE* archivoBloqueActual=fopen(pathBloqueActual,"w");

				int sizeTruncate=infoBloques->tamBloques;
				if(-1==truncate(archivoBloqueActual,sizeTruncate)){
							log_error(logger,"Error al truncar el archivo");
				}


				if(fwrite(bytesAEscribir,strlen(bytesAEscribir)+sizeof(uint32_t),1,archivoBloqueActual)==0){
												log_error(logger,"Error al escribir en el archivo info de restaurante");
				}

				offsetEscritura=offsetEscritura+infoBloques->tamBloques-sizeof(uint32_t);
				bloqueActual=bloqueSiguiente;

				fclose(archivoBloqueActual);
				strcpy(pathBloqueActual,"");
				strcpy(pathBloqueActual,pathBloques);

			}else{
				int offset=0;
				char* pathBloqueActual=malloc(strlen(pathBloques)+strlen(string_itoa(bloqueActual))+4+1);
				memcpy(pathBloqueActual,pathBloques,strlen(pathBloques));
				offset=offset+strlen(pathBloques);
				memcpy(pathBloqueActual+offset,string_itoa(bloqueActual),strlen(string_itoa(bloqueActual)));
				offset=offset+strlen(string_itoa(bloqueActual));
				memcpy(pathBloqueActual+offset,".bin",4);
				offset=offset+4;
				pathBloqueActual[offset]='\0';
				char* bytesAEscribir=malloc((strlen(propiedades)-offsetEscritura)+1);

				bytesAEscribir=string_substring(propiedades,offsetEscritura,strlen(propiedades));
				bytesAEscribir[(strlen(propiedades)-offsetEscritura)]='\0';

				//string_append_with_format(&pathBloqueActual,"%d%s",bloqueActual,".bin");

				FILE* archivoBloqueActual=fopen(pathBloqueActual,"w");
				int sizeTruncate=infoBloques->tamBloques;

				if(-1==truncate(archivoBloqueActual,sizeTruncate)){
										log_error(logger,"Error al truncar el archivo");
				}

				if(fwrite(bytesAEscribir,strlen(bytesAEscribir),1,archivoBloqueActual)==0){
					log_error(logger,"Error al escribir en el archivo info de restaurante");
				}

				fclose(archivoBloqueActual);
			}
		}

		//Actualizo el archivo de bloques asignados al recurso (receta o restaurante)
/*
		for(j=0;j<list_size(bloquesAsignadosARecurso);j++){
			int bloqueActual=atoi(list_get(bloquesAsignadosARecurso,j));
			string_append_with_format(&stringBloquesAsignados,"%d",bloqueActual);
		}
		sprintf(lineaArchivoBloquesAsignados,"%s%s%s",nombreRecurso," ",stringBloquesAsignados);

		if(string_equals_ignore_case(tipoRecurso,"RESTO")){
			strcpy(pathArchivoBloquesAsignados,"..//bloquesAsignadosARestos.bin");

			FILE* archivoBloquesAsignados = fopen(pathArchivoBloquesAsignados, "w");
			if (fwrite(lineaArchivoBloquesAsignados, strlen(bytesAEscribir), 1,archivoBloquesAsignados) == 0) {
						log_error(logger,"Error al escribir en el archivo de bloques asignados");
			}

		}else{
			strcpy(pathArchivoBloquesAsignados,"..//bloquesAsignadosAPedidos.bin");

			FILE* archivoBloquesAsignados = fopen(pathArchivoBloquesAsignados, "w");
			if (fwrite(lineaArchivoBloquesAsignados, strlen(bytesAEscribir), 1,archivoBloquesAsignados) == 0) {
						log_error(logger,"Error al escribir en el archivo de bloques asignados");
			}
		}
*/

	}

}

int grabarInfoRestaurante(tCreacionRestaurante* restauranteNuevo,char* pathRestoNuevo){
	char* propiedades = malloc(strlen("CANTIDAD_COCINEROS=POSICION=AFINIDAD_COCINEROS=PLATOS=PRECIO_PLATOS=CANTIDAD_HORNOS=")*2);
	char* infoPropiedades = malloc(strlen("SIZE=INITIAL_BLOCK=")*2);

	char* pathArchivoInfo=malloc(80);
	strcpy(pathArchivoInfo,pathRestoNuevo);
	string_append(&pathArchivoInfo,"Info.AFIP");
	t_list* bloquesAsignadosAResto=list_create();

	tPosicionesEnArchivoRestaurante* posEnArchivoRestaurante= malloc(sizeof(tPosicionesEnArchivoRestaurante));

	FILE* archivoRestauranteNuevo = fopen(pathArchivoInfo, "w");

/*
	int lengthPropiedades=sprintf(propiedades,"%s%d%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%d"
														,"CANTIDAD_COCINEROS=",restauranteNuevo->cantCocineros,"\n",
														"POSICION=","[",restauranteNuevo->posicion,"]","\n",
														"AFINIDAD_COCINEROS=","[",restauranteNuevo->afinidadCocineros,"]","\n",
														"PLATOS=","[",restauranteNuevo->platos,"]","\n",
														"PRECIO_PLATOS=","[",restauranteNuevo->preciosPlatos,"]","\n",
														"CANTIDAD_HORNOS=",restauranteNuevo->cantidadHornos);
*/

	int lengthPropiedades=sprintf(propiedades,"%s%d%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%d"
															,"CANTIDAD_COCINEROS=",restauranteNuevo->cantCocineros,
															"POSICION=","[",restauranteNuevo->posicion,"]",
															"AFINIDAD_COCINEROS=","[",restauranteNuevo->afinidadCocineros,"]",
															"PLATOS=","[",restauranteNuevo->platos,"]",
															"PRECIO_PLATOS=","[",restauranteNuevo->preciosPlatos,"]",
															"CANTIDAD_HORNOS=",restauranteNuevo->cantidadHornos);

	//Cargo en el diccionario la info de las posiciones de las porpiedades que se crean. Al agrega/modificar propiedades del archivo se debe actualizar este diccionario
	posEnArchivoRestaurante->posCantCocineros=strlen("CANTIDAD_COCINEROS")+1;
	posEnArchivoRestaurante->sizeCantCocineros=strlen(string_itoa(restauranteNuevo->cantCocineros));

	posEnArchivoRestaurante->posPosicion=strlen("CANTIDAD_COCINEROS")+strlen(string_itoa(restauranteNuevo->cantCocineros))+strlen("POSICION");
	posEnArchivoRestaurante->sizePosicion=strlen(string_itoa(restauranteNuevo->posicion));

	posEnArchivoRestaurante->posAfinidad=posEnArchivoRestaurante->posPosicion+posEnArchivoRestaurante->sizePosicion+strlen("AFINIDAD_COCINEROS")+1;
	posEnArchivoRestaurante->sizeAfinidad=strlen(string_itoa(restauranteNuevo->afinidadCocineros));

	posEnArchivoRestaurante->posPlatos=posEnArchivoRestaurante->posAfinidad+posEnArchivoRestaurante->sizeAfinidad+strlen("PLATOS")+1;
	posEnArchivoRestaurante->sizePlatos=strlen(string_itoa(restauranteNuevo->platos));

	posEnArchivoRestaurante->posPlatos=posEnArchivoRestaurante->posPlatos+posEnArchivoRestaurante->sizePlatos+strlen("PRECIOS_PLATOS")+1;
	posEnArchivoRestaurante->sizePlatos=strlen(string_itoa(restauranteNuevo->preciosPlatos));

	posEnArchivoRestaurante->posCantHornos=posEnArchivoRestaurante->posPreciosPlatos+posEnArchivoRestaurante->sizePreciosPlatos+strlen("CATIDAD_HORNOS")+1;
	posEnArchivoRestaurante->sizeCantHornos=strlen(string_itoa(restauranteNuevo->cantidadHornos));

	dictionary_put(diccionarioPosPropiedadesEnArchivo,restauranteNuevo->nombreRestaurante,posEnArchivoRestaurante);

	int bloquelibre=buscarBloqueLibre();

	int lengthInfoPropiedades= sprintf(infoPropiedades,"%s%d%s%s%d","SIZE=",lengthPropiedades,"\n",
														"INITIAL_BLOCK=",bloquelibre);



	if(fwrite(infoPropiedades,lengthInfoPropiedades,1,archivoRestauranteNuevo)==0){
		log_error(logger,"Error al escribir en el archivo info de restaurante");
	}

	//ESCRIBO LOS DATOS EN LOS BLOQUES:
	//TODO: ASIGNAR CON ANTICIPACION LOS BLOQUES, Y VALIDAR QUE EN CASO QUE ESTE LLENO EL FS LA OPERACION NO SE PODRA HACER

	list_add(bloquesAsignadosAResto,bloquelibre);

	dictionary_put(diccionarioBloquesAsignadosARestos,restauranteNuevo->nombreRestaurante,bloquesAsignadosAResto);
	escribirBloques(propiedades,lengthPropiedades,bloquelibre,restauranteNuevo->nombreRestaurante,bloquesAsignadosAResto,"RESTO","NUEVO");


	fclose(archivoRestauranteNuevo);


}


int grabarArchivoRestaurante(tCreacionRestaurante* restauranteNuevo){

	int e;
	struct  stat info;
	char* pathRestoNuevo=malloc(60);
	strcpy(pathRestoNuevo,pathRestaurantes);
	string_append_with_format(&pathRestoNuevo,"%s%s",restauranteNuevo->nombreRestaurante,"/"),
	log_info(logger, "Creando directorio de restaursnte nuevo...");
	e = stat(pathRestoNuevo, &info);

		if (e == 0) {
				if (info.st_mode & S_IFREG) {
					log_error(logger,"El punto de montaje es un archivo.");
					return 0;
				}
				if (info.st_mode & S_IFDIR)
					log_warning(logger,"El restaurante ya existe...saliendo");
			}
		else {
				if (errno == ENOENT) {
					log_info(logger,"El restaurante no existe. Se creará el directorio.");

					e = mkdir(pathRestoNuevo, ACCESSPERMS | S_IRWXU);
					if (e != 0) {
						log_error(logger,"Se produjo un error al crear el directorio. [%d - %s]", errno, strerror(errno));
						return 0;
					}
					else
						log_info(logger,"El directorio de Restaurante se creó correctamente.");
						int restoCreado=grabarInfoRestaurante(restauranteNuevo,pathRestoNuevo);

				}
				else {
					log_error(logger,"Se produjo un error accediendo al punto de montaje. [%d - %s]", errno, strerror(errno));
					return 0;
				}
			}
			return 1;


}
int grabarInfoReceta(tCreacionReceta* recetaNueva,char* pathRecetaNueva){
	char* propiedades = malloc(strlen("PASOS=TIEMPO_PASOS=")*2);
	char* infoPropiedades = malloc(strlen("SIZE=INITIAL_BLOCK=")*2);
	t_list* bloquesAsignadosAReceta=list_create();


	FILE* archivoRecetaNueva = fopen(pathRecetaNueva, "w");
	int lengthPropiedades=sprintf(propiedades,"%s%s%s%s%s%s%s%s"
														,"PASOS=","[",recetaNueva->pasos,"]",
														"TIEMPO_PASOS=","[",recetaNueva->tiemposPasos,"]");


	int bloquelibre=buscarBloqueLibre();

	int lengthInfoPropiedades= sprintf(infoPropiedades,"%s%d%s%s%d","SIZE=",lengthPropiedades,"\n",
															"INITIAL_BLOCK=",bloquelibre);



	if(fwrite(infoPropiedades,lengthInfoPropiedades,1,archivoRecetaNueva)==0){
			log_error(logger,"Error al escribir en el archivo info de receta");
	}

		//ESCRIBO LOS DATOS EN LOS BLOQUES

	list_add(bloquesAsignadosAReceta,bloquelibre);

	dictionary_put(diccionarioBloquesAsignadosARecetas,recetaNueva->nombreReceta,bloquesAsignadosAReceta);

	escribirBloques(propiedades, lengthPropiedades, bloquelibre,recetaNueva->nombreReceta,bloquesAsignadosAReceta,"RECETA","NUEVO");

	fclose(archivoRecetaNueva);


}
int grabarArchivoReceta(tCreacionReceta* recetaNueva){

	int e;
	struct  stat info;
	char* pathRecetaNueva=malloc(60);
	strcpy(pathRecetaNueva,pathRecetas);
	string_append_with_format(&pathRecetaNueva,"%s%s",recetaNueva->nombreReceta,".AFIP"),
	log_info(logger, "Creando directorio de receta nuevo...");
	e = stat(pathRecetaNueva, &info);
	int iLineaArch=0;

	if (e == 0) {
		if (info.st_mode & S_IFREG) {
			log_error(logger, "Es un archivo.");
			return 0;
		}
		if (info.st_mode & S_IFDIR)
			log_warning(logger, "La receta ya existe...saliendo");
	} else {
		if (errno == ENOENT) {
			log_info(logger, "La receta no existe. Se creará el directorio.");

			e = grabarInfoReceta(recetaNueva, pathRecetaNueva);
			if (e != 0) {
				log_error(logger,
						"Se produjo un error al crear el directorio. [%d - %s]",
						errno, strerror(errno));
				return 0;
			} else
				log_info(logger, "La receta se creó correctamente.");

		} else {
			log_error(logger,
					"Se produjo un error accediendo al directorio. [%d - %s]",
					errno, strerror(errno));
			return 0;
		}
	}
	return 1;


}

char* quitarCaracter(char* cadena,char caracter){
	int i=0;
	int tam=strlen(cadena);
	char* final=malloc(tam-1);
	while (i<=tam){

		if(cadena[i]==caracter){
			final[i]=' ';
		}else final[i]=cadena[i];

		i++;
	}

	return final;
}

void mapearParametrosDeLista(t_list* lista,t_respuesta_info_restaurante* info){
	char* cadenaPosicion=malloc(30);
	char* cadenaAfinidad=malloc(30);
	char* cadenaPlatos=malloc(30);
	char* cadenaPreciosPlatos=malloc(30);


	info->cantidad_cocineros=atoi(list_get(lista,0));

	char* posicion=list_get(lista,1);
	cadenaPosicion=quitarCaracter(posicion,'[');
	cadenaPosicion=quitarCaracter(cadenaPosicion,']');
	char** posicionXY= string_split(cadenaPosicion,",");
	info->posicion_x=atoi(posicionXY[0]);
	info->posicion_y=atoi(posicionXY[1]);

	char* afinidad=list_get(lista,2);
	cadenaAfinidad=quitarCaracter(afinidad,'[');
	cadenaAfinidad=quitarCaracter(cadenaAfinidad,']');
	info->afinidad_cocineros=cadenaAfinidad;
	info->size_afinidad_cocineros=strlen(cadenaAfinidad);


	char* platos=list_get(lista,3);
	cadenaPlatos=quitarCaracter(platos,'[');
	cadenaPlatos=quitarCaracter(cadenaPlatos,']');
	info->platos=cadenaPlatos;
	info->size_platos=strlen(cadenaPlatos);

	char* preciosPlatos=list_get(lista,4);
	cadenaPreciosPlatos=quitarCaracter(preciosPlatos,'[');
	cadenaPreciosPlatos=quitarCaracter(cadenaPreciosPlatos,']');
	info->precio_platos=cadenaPreciosPlatos;
	info->size_precio_platos=strlen(cadenaPreciosPlatos);


	info->cantidad_hornos=atoi(list_get(lista,5));

}
t_respuesta_info_restaurante* leerInfoDeResto(char* nombreResto){
	t_respuesta_info_restaurante* infoResto= malloc(sizeof(t_respuesta_info_restaurante));
	int iLineaArch=0;
	struct stat info;
	char* pathInfoResto=malloc(60);
	char* map=malloc(200);
	t_list* listaParametros=list_create();

	strcpy(pathInfoResto,pathRestaurantes);
	string_append_with_format(&pathInfoResto,"%s%s%s",nombreResto,"/","Info.AFIP");
	stat(pathInfoResto, &info);
	FILE* archResto=fopen(pathInfoResto,"r+");

	map=mmap(0,info.st_size,PROT_READ | PROT_WRITE, MAP_SHARED, fileno(archResto), 0);

	if(map==MAP_FAILED){
		close(fileno(pathInfoResto));
		perror("Error mapeando el archivo");
		exit(EXIT_FAILURE);

	}

	char** lineasArchivo = string_split(map, "\n");

	while (lineasArchivo[iLineaArch] != NULL) {
		char** lineaActual = string_n_split(lineasArchivo[iLineaArch], 2, "=");
		string_trim(lineaActual);

		list_add(listaParametros, lineaActual[1]);
		iLineaArch++;

	}

	mapearParametrosDeLista(listaParametros,infoResto);

return infoResto;
}

int grabarArchivoPedido(tCreacionPedido* pedidoNuevo,char* pathPedido,char* nombrePedido){
	char* propiedades = malloc(strlen("ESTADO_PEDIDO=LISTA_PLATOS=CANTIDAD_PLATOS=CANTIDAD_LISTA=PRECIO_TOTAL") * 10);
	char* infoPropiedades = malloc(strlen("SIZE=INITIAL_BLOCK=") * 2);
	t_list* bloquesAsignadosAPedido = list_create();

	FILE* archivoPedidoNuevo = fopen(pathPedido, "w");
	int lengthPropiedades = sprintf(propiedades, "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%d", "ESTADO_PEDIDO=", pedidoNuevo->estadoPedido, "LISTA_PLATOS=", "[",
			pedidoNuevo->listaPlatos, "]","CANTIDAD_PLATO=", "[",pedidoNuevo->cantidadPlatos, "]","CANTIDAD_LISTA=", "[",pedidoNuevo->cantidadLista, "]",
			"PRECIO_TOTAL=",pedidoNuevo->precioTotal);

	int bloquelibre = buscarBloqueLibre();

	int lengthInfoPropiedades = sprintf(infoPropiedades, "%s%d%s%s%d", "SIZE=",
			lengthPropiedades, "\n", "INITIAL_BLOCK=", bloquelibre);

	if (fwrite(infoPropiedades, lengthInfoPropiedades, 1, archivoPedidoNuevo)
			== 0) {
		log_error(logger, "Error al escribir en el archivo info de receta");
	}

	//ESCRIBO LOS DATOS EN LOS BLOQUES
	//TODO: ASIGNAR ANTICIPADAMENTE LOS BLOQUES, CONTEMPLANDO EL CASO QUE EL FS ESTE LLENO

	list_add(bloquesAsignadosAPedido, bloquelibre);

	dictionary_put(diccionarioBloquesAsignadosAPedidos,nombrePedido, bloquesAsignadosAPedido);

	escribirBloques(propiedades, lengthPropiedades, bloquelibre,nombrePedido, bloquesAsignadosAPedido,"PEDIDO","NUEVO");

	fclose(archivoPedidoNuevo);


}

#endif /* FILESYSTEM_C_ */
