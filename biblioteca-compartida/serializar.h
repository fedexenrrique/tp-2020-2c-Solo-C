#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

int serializar(void* buffer, const char* format, ...);
int deserializar(void* buffer, const char* format, ...);
