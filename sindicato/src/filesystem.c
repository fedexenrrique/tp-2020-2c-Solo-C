/*
 * filesystem.c
 *
 *  Created on: 4 sep. 2020
 *      Author: utnso
 */

#ifndef FILESYSTEM_C_
#define FILESYSTEM_C_
#include "filesystem.h"

#define NOMBRE_INFO_RESTAURANTE "Info.AFIP"
#define SIZE 5
#define INITIAL_BLOCK 13

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

	return 1;
}
tInfoBloques* importarInfoBloques(){
	tInfoBloques* infoBloques=malloc(sizeof(tInfoBloques));
	int sizePathInfo=strlen("strlen(/home/utnso/tp-2020-2c-Solo-C/sindicato/infoBloques.bin");
	int sizeNombreArchivoInfo=15;
	char* pathArchivoInfoBloques= malloc(sizePathInfo+sizeNombreArchivoInfo+1);
	memcpy(pathArchivoInfoBloques,"/home/utnso/tp-2020-2c-Solo-C/sindicato/infoBloques.bin",sizePathInfo);
	pathArchivoInfoBloques[sizePathInfo]='\0';
	//string_append(&pathArchivoInfoBloques,"infoBloques.bin");
	char* map=malloc((sizeof(uint32_t)*2) +strlen("AFIP")+2+1);
	struct stat info;
	stat(pathArchivoInfoBloques, &info);

	FILE* archivoInfoBloques=fopen(pathArchivoInfoBloques,"r+");

	map=mmap(0,info.st_size,PROT_READ | PROT_WRITE, MAP_SHARED, fileno(archivoInfoBloques), 0);

	map[info.st_size]='\0';
	char** propiedades= string_split(map,",");

	infoBloques->tamBloques=atoi(propiedades[0]);
	infoBloques->cantBloques=atoi(propiedades[1]);
	infoBloques->magicNumber=propiedades[2];

	if (munmap(map,info.st_size) == -1) {
				log_error(logger,"Error al liberar memoria mapeada");
				exit(EXIT_FAILURE);

	}

	free(pathArchivoInfoBloques);
	return infoBloques;
	//free(map);


}

void cargarBloquesAsigandosAResto(){


	struct stat info;


	int e=0;
	//char* pathArchivoBloquesAsignados= malloc(100);

	//strcpy(pathArchivoBloquesAsignados,PATH_BLOQUES_ASIGNADOS_A_RESTAURANTES);
	//string_append(&pathArchivoBloquesAsignados,"bloquesAsignadosARestos.bin");

	e=stat(pathAbsolutoBloquesAsignadosARestos, &info);


	int i=0; //Para recorrer los restaurantes
	int j=0; //para recorrer los bloques asignados

	if(e==0){

		if(info.st_size!=0){
		FILE* archivoInfoBloques = fopen(pathAbsolutoBloquesAsignadosARestos, "r+");
		char* map=malloc(info.st_size);
		map = mmap(0, info.st_size, PROT_READ | PROT_WRITE, MAP_SHARED,fileno(archivoInfoBloques), 0);
		map[info.st_size]='\0';

		char** restos = string_split(map, "\n");

		while(restos[i]!=NULL){
			t_list* bloquesAsignadosAResto=list_create();

			char** propiedadesResto= string_n_split(restos[i],2," "); //propiedades= Nombre + bloques
			char** bloquesResto= string_split(propiedadesResto[1],","); //lista de bloques separados por coma

			while(bloquesResto[j]!=NULL){
				list_add(bloquesAsignadosAResto,atoi(bloquesResto[j]));
				j++;
			}
			//log_warning(logger,"%d",atoi(list_get(bloquesAsignadosAResto,0)));
			dictionary_put(diccionarioBloquesAsignadosARestos,propiedadesResto[0],bloquesAsignadosAResto);
			i++;
			j=0;
		}


		if (munmap(map, info.st_size) == -1) {
			log_error(logger, "Error al liberar memoria mapeada");
			exit(EXIT_FAILURE);

		}


		}


	}
	//free(pathArchivoBloquesAsignados);


}

void cargarBloquesAsigandosAPedidos(){

	struct stat info;
	int e=0;
	//char* pathArchivoBloquesAsignados= malloc(100);
	//strcpy(pathArchivoBloquesAsignados,PATH_BLOQUES_ASIGNADOS_A_PEDIDOS);
	//string_append(&pathArchivoBloquesAsignados,"bloquesAsignadosAPedidos.bin");
	log_warning(logger,"Leyendo archivo de bloques asignados a pedidos");

	e=stat(pathAbsolutoBloquesAsignadosAPedidos, &info);
	//e1=stat(pathArchivoBloquesAsignados, &infoPathRelativo);

	int i=0; //Para recorrer los restaurantes
	int j=0; //para recorrer los bloques asignados



	if(e==0){
		log_warning(logger,"Archivo de bloques de pedidos encontrado");

		if(info.st_size!=0){
		FILE* archivoInfoBloques = fopen(pathAbsolutoBloquesAsignadosAPedidos, "r+");
		char* map=malloc(info.st_size);
		map = mmap(0, info.st_size, PROT_READ | PROT_WRITE, MAP_SHARED,fileno(archivoInfoBloques), 0);
		map[info.st_size]='\0';

		char** pedidos = string_split(map, "/n");

		while(pedidos[i]!=NULL){
			log_warning(logger,"ID Pedido encontrado");

			t_list* bloquesAsignadosAPedido=list_create();

			char** propiedadesPedido= string_n_split(pedidos[i],2," "); //propiedades= Nombre + bloques
			char** bloquesPedido= string_split(propiedadesPedido[1],","); //lista de bloques separados por coma

			while(bloquesPedido[j]!=NULL){
				list_add(bloquesAsignadosAPedido,atoi(bloquesPedido[j]));
				j++;
			}
			//log_warning(logger,"%d",atoi(list_get(bloquesAsignadosAPedido,0)));
			dictionary_put(diccionarioBloquesAsignadosAPedidos,propiedadesPedido[0],bloquesAsignadosAPedido);
			log_warning(logger,"%d",list_get(bloquesAsignadosAPedido,0));
			i++;
			j=0;
		}


		if (munmap(map, info.st_size) == -1) {
			log_error(logger, "Error al liberar memoria mapeada");
			exit(EXIT_FAILURE);

		}


		}

	}
	//free(pathAbsolutoBloquesAsignadosAPedidos);

}

int cargarBitMap(){

}

int montarFS(tInfoBloques* infoBloques){

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
				cargarBitMap();


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
					crearArchivoMetadata(infoBloques->tamBloques, infoBloques->cantBloques, infoBloques->magicNumber);
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

int buscarBloqueLibre(tInfoBloques* infoBloques){

	int i=0;

	while(bitarray_test_bit(bitMap,i)&&i<infoBloques->cantBloques+1) i++;

	bitarray_set_bit(bitMap,i);

	if( -1 == msync(mapBitArray,fdArchivoBitmap,MS_SYNC) )
		log_error(logger,"No se actualizo el bitmap en el archivo");

	log_info(logger,"la posicion del bitmap seleccionado es  %d",i);

	return i;

}
int actualizarArchivosBloques(char* pathBloques,char* lineaArchivoBloquesAsignados,char*idPedido,char* tipoRecurso, char*nombreRecurso){
	struct stat info;
	int e=0;
	int resultado=1;
	int i=0;
	e=stat(pathBloques,&info);
	int registroEncontrado=0;
	if (e == 0) {
	FILE* archivoBloquesAsignados = fopen(pathBloques, "rw+");
	int fdArchivo = fileno(archivoBloquesAsignados);
	char* map = malloc(info.st_size + 1);
	int offsetFinArchivo = info.st_size;



		if (info.st_size == 0) {//EL ARCHIVO ESTA VACIO, SE GRABA DIRECTAMENTE
			if (fwrite(lineaArchivoBloquesAsignados,
					strlen(lineaArchivoBloquesAsignados), 1,
					archivoBloquesAsignados) == 0) {
				log_error(logger,
						"Error al escribir en el archivo info de restaurante");
				resultado = -3;
			}
		} else {//ACA DEBERIA ENTRAR PARA LOS PEDIDOS Y RESTAURANTES,ACTUALIZA SUS BLOQUES ASIGNADOS. SE AGREGA AL MAP LOS NUEVOS BLOQUES ASIGNADOS
			map = mmap(0, info.st_size, PROT_READ | PROT_WRITE, MAP_SHARED,fdArchivo, 0);
			map[offsetFinArchivo] = '\n';
			offsetFinArchivo++;
			char** lineasArchivo=string_split(map,"\n");
			while(lineasArchivo[i]!=NULL){
				int offsetActualizacion=0;

				char** propiedades=string_split(lineasArchivo[i]," ");
				//dictionary_put(bloquesAsignados,propiedades[0],propiedades[1]);
				if(idPedido!=NULL){/// PARA LOS PEDIDOS
					if(strcmp(propiedades[0],idPedido)==0){
						registroEncontrado=1;
										//offsetActualizacion=strlen(propiedades[0])+1;
						memcpy(map+offsetActualizacion,lineaArchivoBloquesAsignados,strlen(lineaArchivoBloquesAsignados));
						//msync(map, info.st_size, MS_SYNC);
						if(-1==ftruncate(fileno(archivoBloquesAsignados),info.st_size)){
							log_error(logger,"Error al truncar el archivo");

						}
						if (fwrite(map, strlen(map), 1, archivoBloquesAsignados) == 0) {
								log_error(logger,"Error al escribir en el archivo info de restaurante");
								resultado = 0;
							}
						break;

					}

				}else{ //ACA PARA LOS RESTAURANTES
					if(strcmp(propiedades[0],nombreRecurso)==0){
						//offsetActualizacion=strlen(propiedades[0])+1;
						registroEncontrado=1;
						memcpy(map+offsetActualizacion,lineaArchivoBloquesAsignados,strlen(lineaArchivoBloquesAsignados));
						//msync(map, info.st_size, MS_SYNC);
						if(-1==ftruncate(fileno(archivoBloquesAsignados),info.st_size)){
																log_error(logger,"Error al truncar el archivo");

						}
						if (fwrite(map, strlen(map), 1, archivoBloquesAsignados) == 0) {
								log_error(logger,"Error al escribir en el archivo info de restaurante");
						resultado = 0;
						}
						break;
					}
				}
				offsetActualizacion=offsetActualizacion+strlen(lineasArchivo[i]);
				i++;
			}

			if(lineasArchivo[i]==NULL && registroEncontrado==0){ //EL archivo no estaba vacio pero tampoco se encontro el recurso. Se agrega al final

							memcpy(map + offsetFinArchivo, lineaArchivoBloquesAsignados,
									strlen(lineaArchivoBloquesAsignados));

							/*if (msync(map, info.st_size, MS_SYNC) == -1) {
							 log_error(logger, "Error al actualizar archivo mapeado...");
							 resultado=-1;
							 }*/
							if (fwrite(map, strlen(map), 1, archivoBloquesAsignados) == 0) {
								log_error(logger,
										"Error al escribir en el archivo info de restaurante");
								resultado = 0;
							}

							if (munmap(map, info.st_size) == -1) {
								log_error(logger, "Error al liberar memoria mapeada");
								resultado = -1;
								//exit(EXIT_FAILURE);
							}
							//free(map);

					}


			/*if(dictionary_has_key(bloquesAsignados,idPedido)){
				char* listaBloquesActual=dictionary_get(bloquesAsignados,idPedido);
				char** propiedadAActualizar=string_split(lineaArchivoBloquesAsignados," ");
				char*listaBloquesActualizados=propiedadAActualizar[1];
				dictionary_put(bloquesAsignados,idPedido,listaBloquesActualizados);
				msync(map, info.st_size, MS_SYNC);*/


		fclose(archivoBloquesAsignados);

	}
	}

	return resultado;
}

void actualizarArchivoInfoPedido(char* idPedido, int sizeNuevo){

	char** restoPedido=string_n_split(idPedido,2,"-");
	int sizeNombreResto=strlen(restoPedido[0]);
	int sizeNombrePedido=strlen(restoPedido[1]);
	char* nombreResto=malloc(sizeNombreResto+1);
	char* nombrePedido=malloc(sizeNombrePedido+1);


	memcpy(nombreResto,restoPedido[0],sizeNombreResto);
	memcpy(nombrePedido,restoPedido[1],sizeNombrePedido);


	char* pathInfoPedido=malloc(strlen(pathRestaurantes)+sizeNombreResto+1+sizeNombrePedido+5+1);

	int offsetPath=0;
	memcpy(pathInfoPedido,pathRestaurantes,strlen(pathRestaurantes));
	offsetPath+=strlen(pathRestaurantes);
	memcpy(pathInfoPedido+offsetPath,nombreResto,sizeNombreResto);
	offsetPath+=sizeNombreResto;
	memcpy(pathInfoPedido+offsetPath,"/",1);
	offsetPath++;
	memcpy(pathInfoPedido+offsetPath,nombrePedido,sizeNombrePedido);
	offsetPath+=sizeNombrePedido;
	memcpy(pathInfoPedido+offsetPath,".AFIP",strlen(".AFIP"));
	offsetPath+=strlen(".AFIP");
	pathInfoPedido[offsetPath]='\0';

	struct stat info;
	int e=stat(pathInfoPedido,&info);

	if(e==0){
		FILE* archivoInfoPedido=fopen(pathInfoPedido,"rw+");
		char* map = mmap(0, info.st_size, PROT_READ | PROT_WRITE, MAP_SHARED,fileno(archivoInfoPedido), 0);


//		FILE* archivoInfoPedido=fopen(archivoInfoPedido,"r+");

		if (map == MAP_FAILED) {
			close(fileno(archivoInfoPedido));
			perror("Error mapeando el archivo");
			exit(EXIT_FAILURE);

		}
		map[info.st_size]='\0';
		//char** lineasArchivInfo=string_split(map,"\n");
		memcpy(map+5,string_itoa(sizeNuevo),strlen(string_itoa(sizeNuevo)));

		msync(map, info.st_size, MS_SYNC);

		munmap(map, info.st_size);
	}

	free(restoPedido);
	free(pathInfoPedido);
	free(nombreResto);
	free(nombrePedido);

}

int escribirBloques(char*propiedades,uint32_t cantEscritura, int bloqueInicial,char* nombreRecurso,t_list* bloquesAsignadosARecurso,char* tipoRecurso,char* operacion,tInfoBloques* infoBloques,char* idPedido){
	int bloqueActual=0;
	int bloqueSiguiente=0;
	int offsetEscritura=0;
	int i;
	int j;
	int resultado=1;
	int tamanioNuevoTotal=0;
	int bloqueSiguienteNuevo=0;

	log_info(logger,"%d",list_get(bloquesAsignadosARecurso,0));
	uint32_t cantBloquesAEscribir=0;

	char* stringBloquesAsignados=malloc((list_size(bloquesAsignadosARecurso)*sizeof(uint32_t))+(list_size(bloquesAsignadosARecurso)-1)+1);
	//char* stringBloquesAsignados=string_new();
	//char* stringBloquesAsignados=malloc(50*sizeof(char));
	strcpy(stringBloquesAsignados,"");

	//AGREGAR VERIFICACION DE BLOQUES LIBRES EN LAS SIGUINTES CONDICIONES

	if((cantEscritura % infoBloques->tamBloques)==0 && cantEscritura==infoBloques->tamBloques){
		cantBloquesAEscribir= (cantEscritura)/ (infoBloques->tamBloques);
		bloqueActual=bloqueInicial;
		char* pathBloqueActual=malloc(strlen(pathBloques)+strlen(string_itoa(bloqueActual))+4+1);
		memcpy(pathBloqueActual,pathBloques,strlen(pathBloques));
		struct stat infoArchivo;
		int e = stat(pathBloqueActual, &infoArchivo);
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
				resultado=-3;
			}

	}else {
		int sizeStringConPunteros=cantEscritura+ (((cantEscritura) / (infoBloques->tamBloques))*sizeof(uint32_t));
		//int sizeString=strlen()
		//char* stringNuevo=mempcy(propiedades,)

		if(sizeStringConPunteros%infoBloques->tamBloques==0){
			cantBloquesAEscribir= ((sizeStringConPunteros) / (infoBloques->tamBloques));

		}else cantBloquesAEscribir= ((sizeStringConPunteros) / (infoBloques->tamBloques)) +1;



		bloqueActual=bloqueInicial;


		if(strcmp(tipoRecurso,"PEDIDO")==0&& strcmp(operacion,"ACTUALIZAR")==0){
			int bloquesExtra=cantBloquesAEscribir-list_size(bloquesAsignadosARecurso);

			if(bloquesExtra>0){
				for(int i=0;i<bloquesExtra;i++){ //asigno n bloques mas
					bloqueSiguienteNuevo=buscarBloqueLibre(infoBloques);
					list_add(bloquesAsignadosARecurso,bloqueSiguienteNuevo);
					//free(bloqueSiguienteNuevo);
				}
				//En este caso tengo que sacar bloques, en caso que se hayan eliminado elementos del string y se redujera su tamaño
				//saco n elementos de la lista de bloques asignados y los libero para algun otro uso
			}else if (bloquesExtra<0){
				//int bloqueActualAEliminar=atoi(list_get(bloquesAsignadosARecurso,list_size(bloquesAsignadosARecurso)-1));
				for(int i=0;(list_size(bloquesAsignadosARecurso)-bloquesExtra);i++){ //asigno n bloques mas
					int bloqueAEliminar=list_get(bloquesAsignadosARecurso,list_size(bloquesAsignadosARecurso)-1-i);
					 list_remove(bloquesAsignadosARecurso,list_size(bloquesAsignadosARecurso)-1-i); //ELimino desde la ultima posicion
					 bitarray_clean_bit(bitMap,bloqueAEliminar);
					 //
					//list_add(bloquesAsignadosARecurso,bloqueSiguiente);
				}
				//actualizo el diccionario de bloques asignados
				dictionary_put(diccionarioBloquesAsignadosAPedidos,idPedido,bloquesAsignadosARecurso);
			}
		}

		for (i=0; i<cantBloquesAEscribir;i++){
			if(i<cantBloquesAEscribir-1){
				if(string_equals_ignore_case(operacion,"ACTUALIZAR")){
					//if(i<cantBloquesAEscribir-2){
						//bloqueSiguiente=atoi(list_get(bloquesAsignadosARecurso,i+1));
						bloqueSiguiente=list_get(bloquesAsignadosARecurso,i+1);
//						log_warning(logger,"%s",(char*)list_get(bloquesAsignadosARecurso,5));
//						log_warning(logger,"%s",(char*)list_get(bloquesAsignadosARecurso,4));
//						log_warning(logger,"%s",(char*)list_get(bloquesAsignadosARecurso,3));
//						log_warning(logger,"%s",(char*)list_get(bloquesAsignadosARecurso,2));
//						log_warning(logger,"%s",(char*)list_get(bloquesAsignadosARecurso,1));
//						log_warning(logger,"%s",(char*)list_get(bloquesAsignadosARecurso,0));
//
//

					//}

				}else {
					bloqueSiguiente=buscarBloqueLibre(infoBloques);
					list_add(bloquesAsignadosARecurso,bloqueSiguiente);

				}
/*
				if(string_equals_ignore_case(tipoRecurso,"RESTO")){
					dictionary_put(diccionarioBloquesAsignadosARestos,nombreRecurso,bloquesAsignadosARecurso);
				}else if(string_equals_ignore_case(tipoRecurso,"RECETA")){
					dictionary_put(diccionarioBloquesAsignadosARecetas,nombreRecurso,bloquesAsignadosARecurso);

				}else dictionary_put(diccionarioBloquesAsignadosAPedidos,nombreRecurso,bloquesAsignadosARecurso);*/

				char* bytesAEscribir=malloc(infoBloques->tamBloques-sizeof(uint32_t)+1);

				bytesAEscribir=string_substring(propiedades,offsetEscritura,infoBloques->tamBloques-sizeof(uint32_t));


				string_append_with_format(&bytesAEscribir,"%s%d",string_repeat('0',sizeof(uint32_t)-strlen(string_itoa(bloqueSiguiente))),bloqueSiguiente);
								//string_append_with_format(&propiedades,"%d",bloqueSiguiente); //ESCRIBO EL SIZE DE PROPIEDADES+SIZE BLOQUE SIGUIENTE
				//string_append_with_format(&pathBloqueActual,"%d%s",bloqueActual,".bin");
				bytesAEscribir[strlen(bytesAEscribir)]='\0';
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
				if(-1==ftruncate(fileno(archivoBloqueActual),sizeTruncate)){
							log_error(logger,"Error al truncar el archivo");
							resultado=-3;
				}
				tamanioNuevoTotal=tamanioNuevoTotal+strlen(bytesAEscribir);

				if(fwrite(bytesAEscribir,strlen(bytesAEscribir),1,archivoBloqueActual)==0){
												log_error(logger,"Error al escribir en el archivo info de restaurante");
												resultado=0;

												//bitarray_clean_bit(bitMap, i);
												//msync(mapBitArray, fdArchivoBitmap, MS_SYNC);
				}

				offsetEscritura=offsetEscritura+infoBloques->tamBloques-sizeof(uint32_t);
				bloqueActual=bloqueSiguiente;

				free(bytesAEscribir);

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

				FILE* archivoBloqueActual=fopen(pathBloqueActual,"wb+");
				int sizeTruncate=infoBloques->tamBloques;

				if(-1==ftruncate(fileno(archivoBloqueActual),sizeTruncate)){
										log_error(logger,"Error al truncar el archivo");

				}
				tamanioNuevoTotal=tamanioNuevoTotal+strlen(bytesAEscribir);

				if(fwrite(bytesAEscribir,strlen(bytesAEscribir),1,archivoBloqueActual)==0){
					log_error(logger,"Error al escribir en el archivo info de restaurante");
					resultado=-3;

					//bitarray_clean_bit(bitMap, i);
					//msync(mapBitArray, fdArchivoBitmap, MS_SYNC);
				}

				free(bytesAEscribir);
				fclose(archivoBloqueActual);
			}
		}

		//Actualizo el archivo de bloques asignados al recurso (receta,restaurante o pedido)
		log_warning(logger,"%d",list_get(bloquesAsignadosARecurso,0));
		log_warning(logger,"%d",list_get(bloquesAsignadosARecurso,1));
		log_warning(logger,"%d",list_get(bloquesAsignadosARecurso,2));
		log_warning(logger,"%d",list_get(bloquesAsignadosARecurso,3));
		log_warning(logger,"%d",list_get(bloquesAsignadosARecurso,4));
		log_warning(logger,"%d",list_get(bloquesAsignadosARecurso,5));

		if(string_equals_ignore_case(tipoRecurso,"RESTO")){
			dictionary_put(diccionarioBloquesAsignadosARestos,nombreRecurso,bloquesAsignadosARecurso);
			}else if(string_equals_ignore_case(tipoRecurso,"RECETA")){
				dictionary_put(diccionarioBloquesAsignadosARecetas,nombreRecurso,bloquesAsignadosARecurso);

			}else dictionary_put(diccionarioBloquesAsignadosAPedidos,nombreRecurso,bloquesAsignadosARecurso);

		int offset=0;
		for(j=0;j<list_size(bloquesAsignadosARecurso);j++){
			//int bloqueActual=atoi(list_get(bloquesAsignadosARecurso,j));
			//string_append(&stringBloquesAsignados,atoi(list_get(bloquesAsignadosARecurso,j)));

			/*int bloque=0;
			if(strcmp(tipoRecurso,"PEDIDO")==0 && strcmp(operacion,"ACTUALIZAR")==0){
				bloque=atoi(list_get(bloquesAsignadosARecurso,j));
			}else bloque=list_get(bloquesAsignadosARecurso,j);*/
			int bloque=0;
			bloque=list_get(bloquesAsignadosARecurso,j);
			string_append_with_format(&stringBloquesAsignados,"%d",bloque);
			offset=offset+strlen(string_itoa(bloque));
			//memcpy(stringBloquesAsignados+offset,bloqueActual,strlen(bloqueActual));
			if(j<list_size(bloquesAsignadosARecurso)-1){
				string_append(&stringBloquesAsignados,",");
				offset++;
			}
		}
		stringBloquesAsignados[offset]='\0';
		char* lineaArchivoBloquesAsignados;

		char* pathArchivoBloquesAsignadosARestaurantes=malloc(strlen(pathAbsolutoBloquesAsignadosARestos)+1);
		char* pathArchivoBloquesAsignadosAPedidos=malloc(strlen(pathAbsolutoBloquesAsignadosAPedidos)+1);
		char* pathArchivoBloquesAsignadosARecetas=malloc(strlen(pathAbsolutoBloquesAsignadosARecetas)+1);



		if (string_equals_ignore_case(tipoRecurso,"PEDIDO")){
			lineaArchivoBloquesAsignados=malloc(strlen(idPedido)+1+strlen(stringBloquesAsignados)+1);
			sprintf(lineaArchivoBloquesAsignados,"%s%s%s",idPedido," ",stringBloquesAsignados);
		}else {
			lineaArchivoBloquesAsignados=malloc(strlen(nombreRecurso)+1+strlen(stringBloquesAsignados)+1);
			sprintf(lineaArchivoBloquesAsignados,"%s%s%s",nombreRecurso," ",stringBloquesAsignados);
		}

		if(string_equals_ignore_case(tipoRecurso,"RESTO")){
			memcpy(pathArchivoBloquesAsignadosARestaurantes,pathAbsolutoBloquesAsignadosARestos,strlen(pathAbsolutoBloquesAsignadosARestos));
			pathArchivoBloquesAsignadosARestaurantes[strlen(pathAbsolutoBloquesAsignadosARestos)]='\0';
			actualizarArchivosBloques(pathArchivoBloquesAsignadosARestaurantes,lineaArchivoBloquesAsignados,NULL,tipoRecurso,nombreRecurso);
		}else if(string_equals_ignore_case(tipoRecurso,"PEDIDO")){
			memcpy(pathArchivoBloquesAsignadosAPedidos,pathAbsolutoBloquesAsignadosAPedidos,strlen(pathAbsolutoBloquesAsignadosAPedidos));
			pathArchivoBloquesAsignadosAPedidos[strlen(pathAbsolutoBloquesAsignadosAPedidos)]='\0';
			if(string_equals_ignore_case(operacion,"ACTUALIZAR")){
				actualizarArchivoInfoPedido(idPedido,tamanioNuevoTotal);
				actualizarArchivosBloques(pathArchivoBloquesAsignadosAPedidos,lineaArchivoBloquesAsignados,idPedido,tipoRecurso,NULL);

			}else{
				actualizarArchivosBloques(pathArchivoBloquesAsignadosAPedidos,lineaArchivoBloquesAsignados,idPedido,tipoRecurso,NULL);

			}


		}else{
			memcpy(pathArchivoBloquesAsignadosARecetas,pathAbsolutoBloquesAsignadosARecetas,strlen(PATH_BLOQUES_ASIGNADOS_A_RECETAS));
			pathArchivoBloquesAsignadosARecetas[strlen(pathAbsolutoBloquesAsignadosARecetas)]='\0';
			actualizarArchivosBloques(pathArchivoBloquesAsignadosARecetas,lineaArchivoBloquesAsignados,NULL,tipoRecurso,nombreRecurso);

		}


		free(lineaArchivoBloquesAsignados);
		free(pathArchivoBloquesAsignadosARestaurantes);
		free(pathArchivoBloquesAsignadosAPedidos);
		free(pathArchivoBloquesAsignadosARecetas);

	}

	return resultado;

}

int grabarInfoRestaurante(tCreacionRestaurante* restauranteNuevo,char* pathRestoNuevo,tInfoBloques* infoBloques){
	int sizePropiedades=strlen("CANTIDAD_COCINEROS=POSICION=AFINIDAD_COCINEROS=PLATOS=PRECIO_PLATOS=CANTIDAD_HORNOS=")+
			strlen(restauranteNuevo->platos)+strlen(restauranteNuevo->posicion)+
			strlen(restauranteNuevo->preciosPlatos)+strlen(restauranteNuevo->afinidadCocineros)+(sizeof(uint32_t)*2);
	char* propiedades = malloc(sizePropiedades+1);

	int sizePathArchivoInfo=strlen(pathRestoNuevo);
	char* pathArchivoInfo=malloc(strlen(pathRestoNuevo)+sizePathArchivoInfo+1);
	//strcpy(pathArchivoInfo,pathRestoNuevo);+
	//int sizeNombreArchivoInfo=malloc(strlen(NOMBRE_INFO_RESTAURANTE)+1);
	memcpy(pathArchivoInfo,pathRestoNuevo,strlen(pathRestoNuevo));
	memcpy(pathArchivoInfo+strlen(pathRestoNuevo),NOMBRE_INFO_RESTAURANTE,sizePathArchivoInfo);
	//string_append(&pathArchivoInfo,"Info.AFIP");
	pathArchivoInfo[strlen(pathRestoNuevo)+sizePathArchivoInfo]='\0';
	t_list* bloquesAsignadosAResto=list_create();
	int resultadoEscritura=1;

	tPosicionesEnArchivoRestaurante* posEnArchivoRestaurante= malloc(sizeof(tPosicionesEnArchivoRestaurante));

	FILE* archivoRestauranteNuevo = fopen(pathArchivoInfo, "w");


	int lengthPropiedades=sprintf(propiedades,"%s%d%s%s%s%s%s%s%s%s%s%d"
															,"CANTIDAD_COCINEROS=",restauranteNuevo->cantCocineros,
															"POSICION=",restauranteNuevo->posicion,
															"AFINIDAD_COCINEROS=",restauranteNuevo->afinidadCocineros,
															"PLATOS=",restauranteNuevo->platos,
															"PRECIO_PLATOS=",restauranteNuevo->preciosPlatos,
															"CANTIDAD_HORNOS=",restauranteNuevo->cantidadHornos);

	/*
	 * 	int lengthPropiedades=sprintf(propiedades,"%s%d%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%d"
															,"CANTIDAD_COCINEROS=",restauranteNuevo->cantCocineros,
															"POSICION=","[",restauranteNuevo->posicion,"]",
															"AFINIDAD_COCINEROS=","[",restauranteNuevo->afinidadCocineros,"]",
															"PLATOS=","[",restauranteNuevo->platos,"]",
															"PRECIO_PLATOS=","[",restauranteNuevo->preciosPlatos,"]",
															"CANTIDAD_HORNOS=",restauranteNuevo->cantidadHornos);
	 * */

	//Cargo en el diccionario la info de las posiciones de las porpiedades que se crean. Al agrega/modificar propiedades del archivo se debe actualizar este diccionario
	int sizeStringConPunteros=lengthPropiedades+ (((lengthPropiedades) / (infoBloques->tamBloques))*sizeof(uint32_t));

	//int cantBloquesAEscribir= ((sizeStringConPunteros) / (infoBloques->tamBloques)) +1;

	propiedades[sizePropiedades]='\0';
	dictionary_put(diccionarioPosPropiedadesEnArchivo,restauranteNuevo->nombreRestaurante,posEnArchivoRestaurante);

	int bloquelibre=buscarBloqueLibre(infoBloques);
	int sizeInfoPropiedades=strlen("SIZE=INITIAL_BLOCK=")+(sizeof(int)*2)+1;
	char* infoPropiedades = malloc(sizeInfoPropiedades+1);

	int lengthInfoPropiedades= sprintf(infoPropiedades,"%s%d%s%s%d","SIZE=",sizeStringConPunteros,"\n",
														"INITIAL_BLOCK=",bloquelibre);

	infoPropiedades[lengthInfoPropiedades]='\0';


	if(fwrite(infoPropiedades,lengthInfoPropiedades,1,archivoRestauranteNuevo)==0){
		log_error(logger,"Error al escribir en el archivo info de restaurante");
	}

	//ESCRIBO LOS DATOS EN LOS BLOQUES:
	//TODO: ASIGNAR CON ANTICIPACION LOS BLOQUES, Y VALIDAR QUE EN CASO QUE ESTE LLENO EL FS LA OPERACION NO SE PODRA HACER

	list_add(bloquesAsignadosAResto,bloquelibre);

	dictionary_put(diccionarioBloquesAsignadosARestos,restauranteNuevo->nombreRestaurante,bloquesAsignadosAResto);
	resultadoEscritura=escribirBloques(propiedades,lengthPropiedades,bloquelibre,restauranteNuevo->nombreRestaurante,bloquesAsignadosAResto,"RESTO","NUEVO",infoBloques,NULL);


	free(propiedades);
	free(infoPropiedades);
	fclose(archivoRestauranteNuevo);

	return resultadoEscritura;


}


int grabarArchivoRestaurante(tCreacionRestaurante* restauranteNuevo,tInfoBloques* infoBloques){

	int e;
	struct  stat info;
	char* pathRestoNuevo=malloc(60);
	strcpy(pathRestoNuevo,pathRestaurantes);
	string_append_with_format(&pathRestoNuevo,"%s%s",restauranteNuevo->nombreRestaurante,"/"),
	log_info(logger, "Creando directorio de restaursnte nuevo...");
	e = stat(pathRestoNuevo, &info);
	int restoCreado=1;

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
						restoCreado=grabarInfoRestaurante(restauranteNuevo,pathRestoNuevo,infoBloques);

				}
				else {
					log_error(logger,"Se produjo un error accediendo al punto de montaje. [%d - %s]", errno, strerror(errno));
					return 0;
				}
			}
			return restoCreado;

			free(pathRestoNuevo);

}
int grabarInfoReceta(tCreacionReceta* recetaNueva,char* pathRecetaNueva,tInfoBloques* infoBloques){
	char* propiedades = malloc(strlen("PASOS=TIEMPO_PASOS=")+strlen(recetaNueva->pasos)+strlen(recetaNueva->tiemposPasos)+1);
	char* infoPropiedades = malloc(strlen("SIZE=INITIAL_BLOCK=")+sizeof(uint32_t)+sizeof(uint32_t)+1);
	t_list* bloquesAsignadosAReceta=list_create();
	int resutado=0;


	FILE* archivoRecetaNueva = fopen(pathRecetaNueva, "w");
	int lengthPropiedades=sprintf(propiedades,"%s%s%s%s"
														,"PASOS=",recetaNueva->pasos,
														"TIEMPO_PASOS=",recetaNueva->tiemposPasos);
	propiedades[strlen(propiedades)]='\0';

	int bloquelibre=buscarBloqueLibre(infoBloques);

	int sizeStringConPunteros=lengthPropiedades+ (((lengthPropiedades) / (infoBloques->tamBloques))*sizeof(uint32_t));

	//int cantBloquesAEscribir= ((sizeStringConPunteros) / (infoBloques->tamBloques)) +1;

	int lengthInfoPropiedades= sprintf(infoPropiedades,"%s%d%s%s%d","SIZE=",sizeStringConPunteros,"\n",
															"INITIAL_BLOCK=",bloquelibre);

	infoPropiedades[strlen(infoPropiedades)]='\0';

	if(fwrite(infoPropiedades,lengthInfoPropiedades,1,archivoRecetaNueva)==0){
			log_error(logger,"Error al escribir en el archivo info de receta");
	}

		//ESCRIBO LOS DATOS EN LOS BLOQUES

	list_add(bloquesAsignadosAReceta,bloquelibre);

	dictionary_put(diccionarioBloquesAsignadosARecetas,recetaNueva->nombreReceta,bloquesAsignadosAReceta);

	resutado=escribirBloques(propiedades, lengthPropiedades, bloquelibre,recetaNueva->nombreReceta,bloquesAsignadosAReceta,"RECETA","NUEVO",infoBloques,NULL);

	free(propiedades);
	free(infoPropiedades);
	fclose(archivoRecetaNueva);

return resutado;
}
int grabarArchivoReceta(tCreacionReceta* recetaNueva,tInfoBloques*infoBloques){

	int e;
	struct  stat info;
	int sizePathReceta=strlen(pathRecetas)+strlen(recetaNueva->nombreReceta);
	char* pathRecetaNueva=malloc(sizePathReceta+1);
	strcpy(pathRecetaNueva,pathRecetas);
	string_append_with_format(&pathRecetaNueva,"%s%s",recetaNueva->nombreReceta,".AFIP"),
	log_info(logger, "Creando directorio de receta nuevo...");
	e = stat(pathRecetaNueva, &info);

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

			e = grabarInfoReceta(recetaNueva, pathRecetaNueva,infoBloques);
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

	free(pathRecetaNueva);
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
	t_list* listaParametros=list_create();

	strcpy(pathInfoResto,pathRestaurantes);
	string_append_with_format(&pathInfoResto,"%s%s%s",nombreResto,"/","Info.AFIP");
	stat(pathInfoResto, &info);
	FILE* archResto=fopen(pathInfoResto,"r+");
	char* map=malloc(info.st_size+1);

	map=mmap(0,info.st_size,PROT_READ | PROT_WRITE, MAP_SHARED, fileno(archResto), 0);

	if(map==MAP_FAILED){
		close(fileno(archResto));
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

	if (munmap(map,info.st_size) == -1) {
					log_error(logger,"Error al liberar memoria mapeada");
					exit(EXIT_FAILURE);

	}

	mapearParametrosDeLista(listaParametros,infoResto);
	free(map);
	free(pathInfoResto);

return infoResto;
}

int grabarArchivoPedido(tCreacionPedido* pedidoNuevo,char* pathPedido,char* nombrePedido,char* idPedido){
	uint32_t sizePropiedadesPedido=strlen("ESTADO_PEDIDO=LISTA_PLATOS=CANTIDAD_PLATOS=CANTIDAD_LISTA=PRECIO_TOTAL=")+
									 strlen(pedidoNuevo->estadoPedido)+strlen(pedidoNuevo->listaPlatos)+strlen(pedidoNuevo->cantidadPlatos)+
									 strlen(pedidoNuevo->cantidadLista)+sizeof(uint32_t);
	//uint32_t sizeInfoPropiedades=strlen("SIZE=\nINITIAL_BLOCK=")+(sizeof(uint32_t))*2;
	char* propiedades = malloc(sizePropiedadesPedido+1);
	t_list* bloquesAsignadosAPedido = list_create();
	int resultadoOperacion=0;

	FILE* archivoPedidoNuevo = fopen(pathPedido, "w");
	int lengthPropiedades = sprintf(propiedades, "%s%s%s%s%s%s%s%s%s%d", "ESTADO_PEDIDO=", pedidoNuevo->estadoPedido, "LISTA_PLATOS=",
			pedidoNuevo->listaPlatos,"CANTIDAD_PLATOS=",pedidoNuevo->cantidadPlatos,"CANTIDAD_LISTA=",pedidoNuevo->cantidadLista,
			"PRECIO_TOTAL=",pedidoNuevo->precioTotal);

	propiedades[sizePropiedadesPedido]='\0';

	int bloquelibre = buscarBloqueLibre(infoBloques);
	int sizeStringConPunteros=lengthPropiedades+ (((lengthPropiedades) / (infoBloques->tamBloques))*sizeof(uint32_t));

	/*int lengthInfoPropiedades = sprintf(infoPropiedades, "%s%d%s%s%d", "SIZE=",
			lengthPropiedades, "\n", "INITIAL_BLOCK=", bloquelibre);*/
	int lengthInfoPropiedades=strlen("SIZE=")+strlen("INITIAL_BLOCK=")+(sizeof(int)*2)+1;
	char* infoPropiedades = malloc(lengthInfoPropiedades+1);

	/*memcpy(infoPropiedades,"SIZE=",SIZE);
	memcpy(infoPropiedades+5,string_itoa(lengthPropiedades),strlen(string_itoa(lengthPropiedades)));
	//infoPropiedades[strlen(infoPropiedades)+5+sizeof(uint32_t)]='\n';
	memcpy(infoPropiedades+5+strlen(string_itoa(lengthPropiedades)),"\n",1);
	memcpy(infoPropiedades+strlen(string_itoa(lengthPropiedades))+1,"INITIAL_BLOCK",INITIAL_BLOCK);
	memcpy(infoPropiedades+strlen(string_itoa(lengthPropiedades))+13,string_itoa(bloquelibre),strlen(string_itoa(bloquelibre)));*/

	int sizeInfoPropiedades=sprintf(infoPropiedades,"%s%d%s%s%d","SIZE=",sizeStringConPunteros,"\n",
															"INITIAL_BLOCK=",bloquelibre);

	infoPropiedades[sizeInfoPropiedades]='\0';

	if (fwrite(infoPropiedades, sizeInfoPropiedades, 1, archivoPedidoNuevo)== 0) {
		log_error(logger, "Error al escribir en el archivo info de receta");
		resultadoOperacion=0;
	}

	//ESCRIBO LOS DATOS EN LOS BLOQUES
	//TODO: ASIGNAR ANTICIPADAMENTE LOS BLOQUES, CONTEMPLANDO EL CASO QUE EL FS ESTE LLENO

	list_add(bloquesAsignadosAPedido, bloquelibre);

	dictionary_put(diccionarioBloquesAsignadosAPedidos,nombrePedido, bloquesAsignadosAPedido);

	resultadoOperacion=escribirBloques(propiedades, lengthPropiedades, bloquelibre,nombrePedido, bloquesAsignadosAPedido,"PEDIDO","NUEVO",infoBloques,idPedido);

	free(propiedades);
	free(infoPropiedades);
	fclose(archivoPedidoNuevo);

	return resultadoOperacion;

}

#endif /* FILESYSTEM_C_ */
