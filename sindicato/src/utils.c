/*
 * utils.c
 *
 *  Created on: 29 oct. 2020
 *      Author: utnso
 */
#include "sindicato.h"

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
