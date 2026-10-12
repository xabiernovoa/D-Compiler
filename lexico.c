#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lexico.h"
#include "SE.h"
#include "TS.h"
#include "errores.h"
#include "definiciones.h"

#define MAX_LEXEMA_LEN LONGITUD_MAX_LEXEMA

// Seguimiento de posición 
static int lineaActual = 1;
static int columnaActual = 0;

int obtenerLinea(void)  { return lineaActual; }
int obtenerColumna(void){ return columnaActual; }

// Leer cada caracter
static int leerCaracter(void) {
    int c = siguienteCaracter();
    if (c == '\n') {
        lineaActual++;
        columnaActual = 0;
    } else if (c != EOF) {
        columnaActual++;
    }
    return c;
}

// Retroceder caracter
static void retrocederCaracter(int c) {
    devolverCaracter(c);
    if (c == '\n') {
        lineaActual--;
        columnaActual = 0;
    } else if (columnaActual > 0) {
        columnaActual--;
    }
}

static int agregarCaracterLexema(Token *token, int *indice, int c) {
    if (*indice < MAX_LEXEMA_LEN) {
        token->lexema[(*indice)++] = (char)c;
        return 1;
    }
    return 0;
}

// Función auxiliar para saltar espacios en blanco
static void saltarBlancos(void) {
    int c;
    while ((c = leerCaracter()) != EOF) {
        if (c != ' ' && c != '\t' && c != '\n' && c != '\r') {
            retrocederCaracter(c);
            break;
        }
    }
}

// Función auxiliar para manejar comentarios
static int procesarComentario(void) {
    int c = leerCaracter();

    // Comentario de línea //
    if (c == '/') {
        while ((c = leerCaracter()) != EOF && c != '\n');
        return 1;
    }
    // Comentario de bloque /* */
    else if (c == '*') {
        int anterior = 0;
        while ((c = leerCaracter()) != EOF) {
            if (anterior == '*' && c == '/') {
                return 1;
            }
            anterior = c;
        }
        errorLexico(obtenerLinea(), obtenerColumna(), "Comentario /* sin cerrar");
        return 1;
    }
    // Comentario anidado /+ +/ — usa contador de profundidad
    else if (c == '+') {
        int profundidad = 1;
        int lineaApertura = obtenerLinea();

        int anterior = 0;
        while ((c = leerCaracter()) != EOF) {
            if (anterior == '/' && c == '+') {
                profundidad++;
                anterior = 0;
            } else if (anterior == '+' && c == '/') {
                profundidad--;
                anterior = 0;
                if (profundidad == 0) {
                    return 1;
                }
            } else {
                anterior = c;
            }
        }

        errorLexico(lineaApertura, 0, "Comentario /+ sin cerrar");
        return 1;
    }

    // No es comentario, devolver el carácter
    retrocederCaracter(c);
    return 0;
}

// Autómata para identificadores y palabras reservadas
static int procesarIdentificador(Token *token, int primerCaracter) {
    int i = 0;
    int c = primerCaracter;
    int excedido = 0;
    
    if (!agregarCaracterLexema(token, &i, c)) {
        excedido = 1;
    }
    
    while ((c = leerCaracter()) != EOF) {
        if (isalnum(c) || c == '_') {
            if (!agregarCaracterLexema(token, &i, c)) {
                excedido = 1;
            }
        } else {
            retrocederCaracter(c);
            break;
        }
    }
    
    token->lexema[i] = '\0';

    if (excedido) {
        errorLexico(token->linea, token->columna, "Lexema demasiado largo");
    }
    
    // Consultar la tabla de símbolos: devuelve código de palabra reservada o ID
    TSnodo *nodo = buscarTS(token->lexema);
    if (nodo == NULL) {
        // Nuevo identificador: insertar en la TS como ID
        insertarTS(token->lexema, ID);
        token->codigo = ID;
    } else {
        // Ya existe: puede ser palabra reservada (IMPORT, INT...) o ID
        token->codigo = obtenerIdentificadorTS(nodo);
    }
    
    return token->codigo;
}

// Autómata para números (enteros, binarios, reales, exponenciales)
static int procesarNumero(Token *token, int primerCaracter) {
    int i = 0;
    int c = primerCaracter;
    int esReal = 0;
    int excedido = 0;
    
    if (!agregarCaracterLexema(token, &i, c)) {
        excedido = 1;
    }
    
    // Verificar si es binario (0b)
    if (c == '0') {
        c = leerCaracter();
        if (c == 'b') {
            if (!agregarCaracterLexema(token, &i, c)) {
                excedido = 1;
            }
            // Leer dígitos binarios
            while ((c = leerCaracter()) != EOF) {
                if (c == '0' || c == '1') {
                    if (!agregarCaracterLexema(token, &i, c)) {
                        excedido = 1;
                    }
                } else if (c == '_') {
                    // Ignorar separadores
                    continue;
                } else {
                    retrocederCaracter(c);
                    break;
                }
            }
            token->lexema[i] = '\0';

            if (excedido) {
                errorLexico(token->linea, token->columna, "Lexema demasiado largo");
            }

            token->codigo = CTE_ENTERA;
            // Convertir binario a entero
            token->atributo.valorEntero = (int)strtol(token->lexema + 2, NULL, 2);
            return CTE_ENTERA;
        } else {
            retrocederCaracter(c);
        }
    }
    
    // Leer dígitos enteros
    while ((c = leerCaracter()) != EOF) {
        if (isdigit(c)) {
            if (!agregarCaracterLexema(token, &i, c)) {
                excedido = 1;
            }
        } else if (c == '_') {
            // Ignorar separadores (para números como 8_000)
            continue;
        } else if (c == '.') {
            // Parte decimal
            esReal = 1;
            if (!agregarCaracterLexema(token, &i, c)) {
                excedido = 1;
            }
            
            // Leer dígitos decimales
            while ((c = leerCaracter()) != EOF) {
                if (isdigit(c)) {
                    if (!agregarCaracterLexema(token, &i, c)) {
                        excedido = 1;
                    }
                } else if (c == '_') {
                    // Ignorar separadores (para números como 3.14_15)
                    continue;
                } else {
                    retrocederCaracter(c);
                    break;
                }
            }
            break;
        } else {
            retrocederCaracter(c);
            break;
        }
    }
    
    // Verificar exponencial (e o E)
    c = leerCaracter();
    if (c == 'e' || c == 'E') {
        esReal = 1;
        if (!agregarCaracterLexema(token, &i, c)) {
            excedido = 1;
        }
        
        c = leerCaracter();
        if (c == '+' || c == '-') {
            if (!agregarCaracterLexema(token, &i, c)) {
                excedido = 1;
            }
            c = leerCaracter();
        }
        
        // Leer dígitos del exponente
        while (isdigit(c)) {
            if (!agregarCaracterLexema(token, &i, c)) {
                excedido = 1;
            }
            c = leerCaracter();
        }
        retrocederCaracter(c);
    } else {
        retrocederCaracter(c);
    }
    
    token->lexema[i] = '\0';

    if (excedido) {
        errorLexico(token->linea, token->columna, "Lexema demasiado largo");
    }
    
    if (esReal) {
        token->codigo = CTE_REAL;
        token->atributo.valorReal = atof(token->lexema);
        return CTE_REAL;
    } else {
        token->codigo = CTE_ENTERA;
        token->atributo.valorEntero = atoi(token->lexema);
        return CTE_ENTERA;
    }
}

// Autómata para cadenas
static int procesarCadena(Token *token) {
    int i = 0;
    int c;
    int excedido = 0;
    
    if (!agregarCaracterLexema(token, &i, '"')) {
        excedido = 1;
    }
    
    while ((c = leerCaracter()) != EOF) {
        if (c == '"') {
            if (!agregarCaracterLexema(token, &i, c)) {
                excedido = 1;
            }
            token->lexema[i] = '\0';

            if (excedido) {
                errorLexico(token->linea, token->columna, "Lexema demasiado largo");
            }

            token->codigo = CTE_CADENA;
            return CTE_CADENA;
        } else if (c == '\\') {
            // Manejar secuencias de escape
            if (!agregarCaracterLexema(token, &i, c)) {
                excedido = 1;
            }
            c = leerCaracter();
            if (c != EOF) {
                if (!agregarCaracterLexema(token, &i, c)) {
                    excedido = 1;
                }
            }
        } else if (c == '\n') {
            errorLexico(obtenerLinea(), obtenerColumna(), "Cadena sin cerrar");
            token->lexema[i] = '\0';
            token->codigo = -1;
            // No devolver el \n, ya fue consumido
            return -1;
        } else {
            if (!agregarCaracterLexema(token, &i, c)) {
                excedido = 1;
            }
        }
    }
    
    errorLexico(obtenerLinea(), obtenerColumna(), "Cadena sin cerrar al final del archivo");
    token->lexema[i] = '\0';
    token->codigo = -1;
    return -1;
}

// Función principal del analizador léxico
int siguienteToken(Token *token) {
    int c;
    
    while (1) {
        saltarBlancos();
        
        token->linea = obtenerLinea();
        token->columna = obtenerColumna();
        memset(token->lexema, 0, sizeof(token->lexema));
        
        c = leerCaracter();
        
        if (c == EOF) {
            token->codigo = EOF_TOKEN;
            strcpy(token->lexema, "EOF");
            return EOF_TOKEN;
        }
        
        // Comentarios - verificar primero
        if (c == '/') {
            
            if (procesarComentario()) {
                continue; // Saltar comentario y buscar siguiente token
            }
            // Si no es comentario, el carácter fue devuelto, continuar procesando
        }
        
        // Identificadores y palabras reservadas
        if (isalpha(c) || c == '_') {
            return procesarIdentificador(token, c);
        }
        
        // Números
        if (isdigit(c)) {
            return procesarNumero(token, c);
        }
        
        // Cadenas
        if (c == '"') {
            int resultado = procesarCadena(token);
            if (resultado == -1) {
                // Hubo un error en la cadena, ya se reportó y la línea fue consumida
                // Intentar con el siguiente token
                continue;
            }
            return resultado;
        }
        
        // Operadores compuestos y caracteres simples
        switch (c) {
            case '+':
                c = leerCaracter();
                if (c == '+') {
                    strcpy(token->lexema, "++");
                    token->codigo = INCR;
                    return INCR;
                } else if (c == '=') {
                    strcpy(token->lexema, "+=");
                    token->codigo = SUMA_ASIG;
                    return SUMA_ASIG;
                } else {
                    retrocederCaracter(c);
                    token->lexema[0] = '+';
                    token->lexema[1] = '\0';
                    token->codigo = '+';
                    return '+';
                }
            
            case '=':
                c = leerCaracter();
                if (c == '=') {
                    strcpy(token->lexema, "==");
                    token->codigo = IGUAL;
                    return IGUAL;
                } else {
                    retrocederCaracter(c);
                    token->lexema[0] = '=';
                    token->lexema[1] = '\0';
                    token->codigo = '=';
                    return '=';
                }
            
            // Caracteres simples
            case '-': case '*': case '/':
            case '<': case '>':
            case '(': case ')':
            case '[': case ']':
            case '{': case '}':
            case ';': case ',': case '.':
                token->lexema[0] = c;
                token->lexema[1] = '\0';
                token->codigo = c;
                return c;
            
            default:
                sprintf(token->lexema, "%c", c);
                errorLexico(token->linea, token->columna, "Carácter no reconocido");
                continue; // Intentar siguiente token
        }
    }
    
    return EOF_TOKEN;
}