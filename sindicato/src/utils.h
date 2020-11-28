/*
 * utils.h
 *
 *  Created on: 30 oct. 2020
 *      Author: utnso
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <stdio.h>
#include <dirent.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <setjmp.h>
#include <dirent.h>

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <math.h>
#include <pthread.h>
#include <signal.h>
#include "commons/bitarray.h"
#include "commons/collections/list.h"

#include "serializar.h"

char *strremove(char *str, const char *sub) ;
char* buscarSubstring(char* string, char* subString);
int contarPedidosEnDirectorio(char* path);
char* removerBloqueSiguienteDeString(char* cadena,char* numBloque);

#endif /* UTILS_H_ */
