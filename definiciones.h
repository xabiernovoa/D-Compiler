#ifndef DEFINICIONES_H
#define DEFINICIONES_H

// Palabras reservadas que aparecen en regression.d
#define CAST 300
#define DOUBLE 301
#define FOREACH 302
#define IMPORT 303
#define INT 304
#define RETURN 305
#define VOID 306
#define WHILE 307

// Identificadores y literales
#define ID 308              // Identificadores (variables, funciones)
#define CTE_ENTERA 309      // Constantes enteras: 0, 1, 2, 0b1, 8_000
#define CTE_REAL 310        // Constantes reales: 6.5, 1.0e+03, 40e-1
#define CTE_CADENA 311      // Cadenas: "texto"

// Operadores compuestos (más de un carácter)
#define SUMA_ASIG 312       // +=
#define INCR 313            // ++
#define IGUAL 314           // ==

// Fin de fichero
#define EOF_TOKEN 315       // Fin de archivo

// Los caracteres simples usan su código ASCII directamente:


#endif // DEFINICIONES_H