#ifndef TS_H
#define TS_H

#include <stdio.h>
#include <stdlib.h>

// Estructura opaca para el nodo del árbol (no expuesta al usuario)
typedef struct TSnodo TSnodo;

/* ============= FUNCIONES DE LA TABLA DE SÍMBOLOS ============= */

/**
 * Inicializa la tabla de símbolos
 * Establece el nivel inicial en 0 y el árbol vacío
 */
void iniciarTS(void);

/**
 * Inserta un nuevo símbolo en la tabla de símbolos
 * @param lexema: cadena que representa el identificador
 * @param identificador: identificador/atributo del símbolo
 * @return 0 si se insertó correctamente, -1 si hubo error (duplicado en mismo nivel)
 */
int insertarTS(const char *lexema, int identificador);

/**
 * Busca un símbolo en la tabla de símbolos
 * @param lexema: cadena a buscar
 * @return puntero al nodo si se encuentra, NULL si no existe
 */
TSnodo* buscarTS(const char *lexema);

/**
 * Devuelve el identificador/código almacenado en un nodo de la TS
 * @param nodo: puntero al nodo devuelto por buscarTS
 * @return identificador del símbolo (ej. IMPORT, ID, INT...)
 */
int obtenerIdentificadorTS(TSnodo *nodo);

/**
 * Incrementa el nivel de bloque actual (inicio de nuevo bloque/ámbito)
 */
void nuevoBloqueTS(void);

/**
 * Finaliza el bloque actual
 * Elimina todos los símbolos del nivel actual y decrementa el nivel
 */
void finBloqueTS(void);

/**
 * Imprime la estructura completa de la tabla de símbolos
 * Muestra el árbol con sus listas de símbolos por nivel
 */
void imprimirTS(void);

/**
 * Libera toda la memoria de la tabla de símbolos
 */
void destruirTS(void);

#endif