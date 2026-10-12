#ifndef LEXICO_H
#define LEXICO_H

#include <stdio.h>
#include <stdlib.h>

#define LONGITUD_MAX_LEXEMA 255

// Estructura para representar un token
typedef struct {
    int codigo;           // Código del token (definido en definiciones.h)
    int linea;            // Línea donde aparece
    int columna;          // Columna donde aparece
    char lexema[LONGITUD_MAX_LEXEMA + 1]; // Lexema del token
    union {
        int valorEntero;
        double valorReal;
    } atributo;
} Token;

// Función principal del analizador léxico
int siguienteToken(Token *token);

#endif // LEXICO_H