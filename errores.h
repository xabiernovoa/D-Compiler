#ifndef ERRORES_H
#define ERRORES_H
#include <stdio.h>


void errorSistema(const char *mensaje);


void errorLexico(int linea, int columna, const char *mensaje);


void errorSintactico(int linea, int columna, const char *mensaje);


void errorSemantico(int linea, int columna, const char *mensaje);

#endif