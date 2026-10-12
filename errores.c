#include <stdio.h>
#include <stdlib.h>
#include "errores.h"

void errorSistema(const char *mensaje){
    fprintf(stderr, "Error: %s\n", mensaje);
    exit(1);
}


void errorLexico(int linea, int columna, const char *mensaje){
    fprintf(stderr, "Error léxico en línea %d, columna %d: %s\n", linea, columna, mensaje);
}


void errorSintactico(int linea, int columna, const char *mensaje){
    fprintf(stderr, "Error sintáctico en línea %d, columna %d: %s\n", linea, columna, mensaje);
}


void errorSemantico(int linea, int columna, const char *mensaje){
    fprintf(stderr, "Error semántico en línea %d, columna %d: %s\n", linea, columna, mensaje);
}