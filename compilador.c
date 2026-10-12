#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include "TS.h"
#include "errores.h"
#include "SE.h"
#include "lexico.h"
#include "definiciones.h"
#include "sintactico.h"

// Función para liberar recursos
void liberarRecursos(void) {
    destruirTS();
    cerrarSE();
}

// Manejador de señal para Ctrl+C
void manejadorCtrlC(int signal) {
    printf("\n\nCompilación interrumpida (Ctrl + C)\n");
    liberarRecursos();
    exit(0);
}



static void inicializarCompilador(char *nombreArchivo) {
    // Inicializar tabla de símbolos
    iniciarTS();

    // Inicializar sistema de entrada
    iniciarSE(nombreArchivo);
       
}



int main(int argc, char *argv[]){
    // Configurar manejador de Ctrl+C
    signal(SIGINT, manejadorCtrlC);
    
    // Verificar argumentos
    if (argc < 2) {
        printf("Uso: %s <archivo_fuente>\n", argv[0]);
        return 1;
    }
    
    // Inicializar compilador
    inicializarCompilador(argv[1]);

    // Tabla de símbolos inicial (solo palabras reservadas) 
    printf("\n");
    printf("Tabla de símbolos: \n");
    imprimirTS();

    //Secuencia de componentes léxicos 
    printf("\n");
    printf("Secuencia de componentes léxicos: \n");


    analisisSintactico();


    printf("\n");
    printf("Tabla de símbolos: \n");
    imprimirTS();

    // Liberar recursos al finalizar normalmente
    liberarRecursos();
    return 0;
}