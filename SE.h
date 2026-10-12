#ifndef SE_H
#define SE_H

#include <stdio.h>
#include <stdlib.h>

#define TAM_BUFFER 1024

int iniciarSE(const char *nombreArchivo);
int siguienteCaracter(void);
int devolverCaracter(int C);
void cerrarSE(void);


#endif