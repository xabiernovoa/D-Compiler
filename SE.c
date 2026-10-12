#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "errores.h"
#include "SE.h"

static FILE *archivo = NULL;

// Doble buffer con doble centinela
static char buffer[TAM_BUFFER * 2 + 2];
static int inicio = 0;      // Puntero de inicio
static int delantero = 0;   // Puntero delantero
static int bufferActivo = 0; // 0 = primera mitad, 1 = segunda mitad
static int finArchivo = 0;   // Flag para indicar fin de archivo

// Función auxiliar para recargar buffer
static void recargarBuffer(int mitad) {
    if (archivo == NULL) return;
    
    int offset = mitad * TAM_BUFFER;
    int leidos = fread(&buffer[offset], 1, TAM_BUFFER, archivo);
    
    // Siempre colocar centinela al final de lo leído
    buffer[offset + leidos] = EOF;
    
    if (leidos < TAM_BUFFER) {
        finArchivo = 1;
    }
}


int iniciarSE(const char *nombreArchivo){
    archivo = fopen(nombreArchivo, "r");
    if (archivo == NULL) {
        errorSistema("No se pudo abrir el archivo");
        exit(1);  
    }
    
    // Inicializar buffers
    inicio = 0;
    delantero = 0;
    bufferActivo = 0;
    finArchivo = 0;
    
    // Cargar ambos buffers inicialmente
    recargarBuffer(0);
    recargarBuffer(1);
    
    return 0;
}

int siguienteCaracter(void){
    char caracter = buffer[delantero];
    
    // Si llegamos al centinela
    if (caracter == EOF) {
        // Verificar si es fin de archivo real
        if (delantero == TAM_BUFFER - 1) {
            // Fin de primer buffer, recargar
            recargarBuffer(0);
            delantero++;
            bufferActivo = 1;
        } else if (delantero == (TAM_BUFFER * 2) - 1) {
            // Fin de segundo buffer, recargar y volver al inicio
            recargarBuffer(1);
            delantero = 0;
            bufferActivo = 0;
        } else {
            // Fin de archivo real
            return EOF;
        }
        caracter = buffer[delantero];
        if (caracter == EOF) {
            return EOF;
        }
    }
    
    delantero++;
    
    return caracter;
}


int devolverCaracter(int C){
    if (delantero > 0) {
        delantero--;
    } else {
        delantero = (TAM_BUFFER * 2) - 1;
    }
    
    return 0;
}

void cerrarSE(void){
    if (archivo != NULL) {
        fclose(archivo);
        archivo = NULL;
    }
}


