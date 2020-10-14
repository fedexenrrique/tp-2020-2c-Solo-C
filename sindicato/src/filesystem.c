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
						log_error(logger,"El punto de montaje es un archivo.");
						return 0;
					}
					if (info.st_mode & S_IFDIR)
						log_warning(logger,"El bitmap ya existe...saliendo");
				}
			else {
					if (errno == ENOENT) {
						log_info(logger,"El Bitmap no existe. Se creará el archivo.");


							log_info(logger,"Creando Archivo Bitmap...");
							FILE* archivoBitmap= fopen(pathArchivoBitmap,"wb+");

							if (archivoBitmap!=NULL){
								fdArchivoBitmap=fileno(archivoBitmap);
								tamBitmap=(((sizeof(char))*infoBloques->cantBloques)/8)+1;

								if(ftruncate(fdArchivoBitmap, tamBitmap)==-1)log_error(logger,"No se pudo truncar el archivo");

										//void * bitmap_zone=malloc(size_bitmap);

								mapBitArray = mmap(NULL, tamBitmap, PROT_READ | PROT_WRITE,MAP_SHARED, fdArchivoBitmap, 0);

								bitMap = bitarray_create_with_mode(mapBitArray,tamBitmap, LSB_FIRST);
								if (bitMap == NULL)
									log_error(logger, "No se creo correctamente el bitarray");

								log_info(logger, "La cantidad de bytes del bitmap es %d",
										bitarray_get_max_bit(bitMap));

								for (off_t i = 1; i <infoBloques->cantBloques ; i++) {
									bitarray_clean_bit(bitMap, i);
								}
								msync(mapBitArray, fdArchivoBitmap, MS_SYNC);

							}


					}
					else {
						log_error(logger,"Se produjo un error accediendo al archivo. [%d - %s]", errno, strerror(errno));
						return 0;
					}
				}

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

	log_error(logger,"la posicion del bitmap seleccionado es  %d",i);

	return i;

}
void escribirBloques(char*propiedades,int cantEscritura, int bloqueInicial){
	int e=0;
	int bloqueActual=0;
	int bloqueSiguiente=0;
	int offsetEscritura=0;
	int offsetFinCadena= strlen(propiedades);
	char* bytesAEscribir=malloc(infoBloques->tamBloques);


	char* pathBloqueActual=malloc(60);
	strcpy(pathBloqueActual,pathBloques);
	struct stat infoArchivo;
	e = stat(pathBloqueActual, &infoArchivo);
	uint32_t cantBloquesAEscribir=0;

	//AGREGAR VERIFICACION DE BLOQUES LIBRES EN LAS SIGUINTES CONDICIONES

	if(cantEscritura==infoBloques->tamBloques){
		cantBloquesAEscribir= (cantEscritura)/ (infoBloques->tamBloques);
		bloqueActual=bloqueInicial;
		string_append_with_format(&pathBloqueActual,"%d%s",bloqueActual,".bin");
		FILE* archivoBloqueActual=fopen(pathBloqueActual,"w");

			if(fwrite(propiedades,cantEscritura,1,archivoBloqueActual)==0){
				log_error(logger,"Error al escribir en el archivo info de restaurante");
			}

	}else {
		cantBloquesAEscribir= ((cantEscritura) / (infoBloques->tamBloques)) +1;
		int i;
		bloqueActual=bloqueInicial;

		for (i=0; i<cantBloquesAEscribir;i++){
			if(i<cantBloquesAEscribir-1){
				bloqueSiguiente=buscarBloqueLibre();
				bytesAEscribir=string_substring(propiedades,offsetEscritura,infoBloques->tamBloques-sizeof(uint32_t));
				string_append_with_format(&bytesAEscribir,"%d",bloqueSiguiente);
				//string_append_with_format(&propiedades,"%d",bloqueSiguiente); //ESCRIBO EL SIZE DE PROPIEDADES+SIZE BLOQUE SIGUIENTE
				string_append_with_format(&pathBloqueActual,"%d%s",bloqueActual,".bin");
				FILE* archivoBloqueActual=fopen(pathBloqueActual,"w");
				if(fwrite(bytesAEscribir,strlen(bytesAEscribir)+sizeof(uint32_t),1,archivoBloqueActual)==0){
												log_error(logger,"Error al escribir en el archivo info de restaurante");
				}

				offsetEscritura=offsetEscritura+infoBloques->tamBloques-sizeof(uint32_t);
				bloqueActual=bloqueSiguiente;

				fclose(archivoBloqueActual);
				strcpy(pathBloqueActual,"");
				strcpy(pathBloqueActual,pathBloques);

			}else{
				bytesAEscribir=string_substring(propiedades,offsetEscritura,strlen(propiedades));

				string_append_with_format(&pathBloqueActual,"%d%s",bloqueActual,".bin");

				FILE* archivoBloqueActual=fopen(pathBloqueActual,"w");
				if(fwrite(bytesAEscribir,strlen(bytesAEscribir),1,archivoBloqueActual)==0){
					log_error(logger,"Error al escribir en el archivo info de restaurante");
				}

				fclose(archivoBloqueActual);


			}
		}
	}

}

int grabarInfoRestaurante(tCreacionRestaurante* restauranteNuevo,char* pathRestoNuevo){
	char* propiedades = malloc(strlen("CANTIDAD_COCINEROS=POSICION=AFINIDAD_COCINEROS=PLATOS=PRECIO_PLATOS=CANTIDAD_HORNOS=")*2);
	char* infoPropiedades = malloc(strlen("SIZE=INITIAL_BLOCK=")*2);

	char* pathArchivoInfo=malloc(80);
	strcpy(pathArchivoInfo,pathRestoNuevo);
	string_append(&pathArchivoInfo,"Info.AFIP");

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

	//CAMBIAR LA ASIGNACION DEL BLOQUE INICIAL YA QUE ES NECESARIO VALIDAR A CUAL BLOQUE SE LO DEBO ASIGNAR
	int bloquelibre=buscarBloqueLibre();

	int lengthInfoPropiedades= sprintf(infoPropiedades,"%s%d%s%s%d","SIZE=",lengthPropiedades,"\n",
														"INITIAL_BLOCK=",bloquelibre);



	if(fwrite(infoPropiedades,lengthInfoPropiedades,1,archivoRestauranteNuevo)==0){
		log_error(logger,"Error al escribir en el archivo info de restaurante");
	}

	//ESCRIBO LOS DATOS EN LOS BLOQUES


	escribirBloques(propiedades,lengthPropiedades,bloquelibre);


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


	escribirBloques(propiedades, lengthPropiedades, bloquelibre);

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
					log_error(logger,"Es un archivo.");
					return 0;
				}
				if (info.st_mode & S_IFDIR)
					log_warning(logger,"La receta ya existe...saliendo");
			}
		else {
				if (errno == ENOENT) {
					log_info(logger,"La receta no existe. Se creará el directorio.");

					e = grabarInfoReceta(recetaNueva,pathRecetaNueva);
					if (e != 0) {
						log_error(logger,"Se produjo un error al crear el directorio. [%d - %s]", errno, strerror(errno));
						return 0;
					}
					else
						log_info(logger,"La receta se creó correctamente.");

				}
				else {
					log_error(logger,"Se produjo un error accediendo alk diorectorio. [%d - %s]", errno, strerror(errno));
					return 0;
				}
			}
			return 1;


}

char* quitarCaracter(char* cadena,char caracter){
	int i=0;
	int tam=strlen(cadena);
	char* final=malloc(30);
	while (i<=tam){

		if(cadena[i]==caracter){
			final[i]=' ';
		}else final[i]=cadena[i];

		i++;
	}

	string_trim(&final);

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

#endif /* FILESYSTEM_C_ */
