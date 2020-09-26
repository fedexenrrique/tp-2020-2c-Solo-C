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

	fclose(archivoMetadata);

}

int crearDirectorioRestaurantes(){
	int e = 0;
	pathRestaurantes=string_new();

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

	return 1;

}

int montarFS(int blockSize, int cantBloques,char* magicNumber){

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
				int statusMetadata=crearDirectorioMetadata();
						int statusFiles=crearDirectorioFiles();
						int statusBloques=crearDirectorioBloques();
						int statusRestaurantes=crearDirectorioRestaurantes();
						int statusRecetas=crearDirectorioRecetas();
						crearArchivoMetadata(blockSize, cantBloques,magicNumber);

			}
			else {
				log_error(logger,"Se produjo un error accediendo al punto de montaje. [%d - %s]", errno, strerror(errno));
				return 0;
			}
		}
		return 1;
	}

int grabarInfoRestaurante(tCreacionRestaurante* restauranteNuevo,char* pathRestoNuevo){
	char* propiedades = malloc(strlen("CANTIDAD_COCINEROS=POSICION=AFINIDAD_COCINEROS=PLATOS=PRECIO_PLATOS=CANTIDAD_HORNOS=")*2);
	char* pathArchivoInfo=malloc(80);
	strcpy(pathArchivoInfo,pathRestoNuevo);
	string_append(&pathArchivoInfo,"Info.txt");

	FILE* archivoRestauranteNuevo = fopen(pathArchivoInfo, "w");
	int length=sprintf(propiedades,"%s%d%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%d"
														,"CANTIDAD_COCINEROS=",restauranteNuevo->cantCocineros,"\n",
														"POSICION=","[",restauranteNuevo->posicion,"]","\n",
														"AFINIDAD_COCINEROS=","[",restauranteNuevo->afinidadCocineros,"]","\n",
														"PLATOS=","[",restauranteNuevo->platos,"]","\n",
														"PRECIO_PLATOS=","[",restauranteNuevo->preciosPlatos,"]","\n",
														"CANTIDAD_HORNOS=",restauranteNuevo->cantidadHornos);

	if(fwrite(propiedades,length,1,archivoRestauranteNuevo)==0){
		log_error(logger,"Error al escribir en el archivo info de restaurante");
	}

	fclose(archivoRestauranteNuevo);


}


void grabarArchivoRestaurante(tCreacionRestaurante* restauranteNuevo){

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
					log_info(logger,"Punto de montaje encontrado.");
			}
		else {
				if (errno == ENOENT) {
					log_warning(logger,"El punto de montaje no existe. Se creará el directorio.");

					e = mkdir(pathRestoNuevo, ACCESSPERMS | S_IRWXU);
					if (e != 0) {
						log_error(logger,"Se produjo un error al crear el directorio. [%d - %s]", errno, strerror(errno));
						return 0;
					}
					else
						log_info(logger,"El directorio de Restaurante",restauranteNuevo->nombreRestaurante,"se creó correctamente.");
						int restoCreado=grabarInfoRestaurante(restauranteNuevo,pathRestoNuevo);


				}
				else {
					log_error(logger,"Se produjo un error accediendo al punto de montaje. [%d - %s]", errno, strerror(errno));
					return 0;
				}
			}
			return 1;


}



#endif /* FILESYSTEM_C_ */
