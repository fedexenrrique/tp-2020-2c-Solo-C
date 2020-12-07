/*
 * utils.c
 *
 *  Created on: 29 oct. 2020
 *      Author: utnso
 */
#include "utils.h"
#define NOMBRE_INFO_RESTAURANTE "Info.AFIP"

char *strremove(char *str, const char *sub) {
    size_t len = strlen(sub);
    if (len > 0) {
        char *p = str;
        while ((p = strstr(p, sub)) != NULL) {
            memmove(p, p + len, strlen(p + len) + 1);
        }
    }
    return str;
}


//char* buscarSubstring(char* string, char* subString){
//	uint32_t tamSubString=strlen(subString);
//	char* p=string;
//	if(tamSubString>0){
//
//	}
//
//}

int contarPedidosEnDirectorio(char* path){
	int contador = 0;
	DIR * directorio;
	struct dirent * entradaDirectorio;

	directorio = opendir(path); /* There should be error handling after this */
	while ((entradaDirectorio = readdir(directorio)) != NULL) {
	    if (entradaDirectorio->d_type == DT_REG) { /* If the entry is a regular file */
	         contador++;
	    }
	}
	closedir(directorio);
	return contador;
}

char* removerBloqueSiguienteDeString(char* cadena,char* numBloque){
	int lenCadena=strlen(cadena);
	int lenNumBloque=strlen(numBloque);
	cadena=string_substring(cadena,0,lenCadena-lenNumBloque);

	return cadena;
}

int buscarPosicionCaracter(char* cadena, char caracter){
	int lenCadena=strlen(cadena);
	int i=0;
	char* aux=cadena;
	int posCaracter=0;

	for(i=0;i<lenCadena;i++){
		if(aux[i]==caracter) posCaracter=i;
	}

	return posCaracter;

}

