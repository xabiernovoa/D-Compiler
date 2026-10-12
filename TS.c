#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "TS.h"
#include "errores.h"
#include "definiciones.h"

/* Tabla de símbolos. Implementada mediante un árbol binario ordenado
   y lista enlazada para los casos en los que se repiten en diferentes niveles.
*/

// Nodo de la lista encadenada para símbolos repetidos en diferentes niveles
typedef struct ListaNodo {
    int nivel;
    int identificador;  // Identificador/atributo del símbolo
    struct ListaNodo *siguiente;
} ListaNodo;

// Nodo del árbol binario de búsqueda
typedef struct TSnodo {
    char *lexema;       // Cadena del identificador
    int nivel;
    int identificador;  // Identificador/atributo del símbolo
    ListaNodo *lista;   // Lista de símbolos con mismo lexema en diferentes niveles
    struct TSnodo *izq;
    struct TSnodo *der;
} TSnodo;

// Variables globales
static TSnodo *raiz = NULL;
static int nivelActual = 0;

/* ============= DECLARACIONES DE FUNCIONES ============= */

static void imprimirRecursivo(TSnodo *nodo);

/* ============= FUNCIONES AUXILIARES ============= */

// Crea un nuevo nodo de lista
ListaNodo* crearNodoLista(int nivel, int identificador) {
    ListaNodo *nuevo = (ListaNodo*)malloc(sizeof(ListaNodo));
    if (nuevo == NULL) {
        fprintf(stderr, "Error: No se pudo asignar memoria para nodo de lista\n");
        exit(1);
    }
    nuevo->nivel = nivel;
    nuevo->identificador = identificador;
    nuevo->siguiente = NULL;
    return nuevo;
}

// Crea un nuevo nodo del árbol
TSnodo* crearNodoArbol(const char *lexema, int nivel, int identificador) {
    TSnodo *nuevo = (TSnodo*)malloc(sizeof(TSnodo));
    if (nuevo == NULL) {
        fprintf(stderr, "Error: No se pudo asignar memoria para nodo del árbol\n");
        exit(1);
    }
    nuevo->lexema = (char*)malloc(strlen(lexema) + 1);
    if (nuevo->lexema == NULL) {
        fprintf(stderr, "Error: No se pudo asignar memoria para lexema\n");
        free(nuevo);
        exit(1);
    }
    strcpy(nuevo->lexema, lexema);
    nuevo->nivel = nivel;
    nuevo->identificador = identificador;
    nuevo->lista = NULL;
    nuevo->izq = NULL;
    nuevo->der = NULL;
    return nuevo;
}

// Busca si existe un símbolo con el mismo lexema y nivel en la lista
int buscarEnLista(ListaNodo *lista, int nivel) {
    ListaNodo *actual = lista;
    while (actual != NULL) {
        if (actual->nivel == nivel) {
            return 1; // Encontrado
        }
        actual = actual->siguiente;
    }
    return 0; // No encontrado
}

// Inserta en la lista encadenada del nodo
void insertarEnLista(TSnodo *nodo, int nivel, int identificador) {
    ListaNodo *nuevo = crearNodoLista(nivel, identificador);
    nuevo->siguiente = nodo->lista;
    nodo->lista = nuevo;
}

/* ============= FUNCIONES PRINCIPALES ============= */

// Inicializa la tabla de símbolos
void iniciarTS(void) {
    raiz = NULL;
    nivelActual = 0;
    
    // Cargar palabras reservadas del lenguaje D
    insertarTS("cast", CAST);
    insertarTS("double", DOUBLE);
    insertarTS("foreach", FOREACH);
    insertarTS("import", IMPORT);
    insertarTS("int", INT);
    insertarTS("return", RETURN);
    insertarTS("void", VOID);
    insertarTS("while", WHILE);
}

// Función auxiliar recursiva para insertar en el árbol
TSnodo* insertarRecursivo(TSnodo *nodo, const char *lexema, int nivel, int identificador, int *error) {
    // Caso base: posición encontrada
    if (nodo == NULL) {
        return crearNodoArbol(lexema, nivel, identificador);
    }
    
    int comparacion = strcmp(lexema, nodo->lexema);
    
    if (comparacion < 0) {
        // Insertar en subárbol izquierdo
        nodo->izq = insertarRecursivo(nodo->izq, lexema, nivel, identificador, error);
    } 
    else if (comparacion > 0) {
        // Insertar en subárbol derecho
        nodo->der = insertarRecursivo(nodo->der, lexema, nivel, identificador, error);
    } 
    else {
        // El lexema ya existe
        if (nodo->nivel == nivel) {
            // Error: símbolo duplicado en el mismo nivel
            *error = 1;
            fprintf(stderr, "Error: El símbolo '%s' ya existe en el nivel %d\n", lexema, nivel);
            return nodo;
        } 
        else if (buscarEnLista(nodo->lista, nivel)) {
            // Error: ya existe en la lista con el mismo nivel
            *error = 1;
            fprintf(stderr, "Error: El símbolo '%s' ya existe en el nivel %d (en lista)\n", lexema, nivel);
            return nodo;
        }
        else {
            // Añadir a la lista encadenada
            insertarEnLista(nodo, nivel, identificador);
        }
    }
    
    return nodo;
}

// Insertar un nuevo símbolo en la tabla
int insertarTS(const char *lexema, int identificador) {
    int error = 0;
    raiz = insertarRecursivo(raiz, lexema, nivelActual, identificador, &error);
    return error ? -1 : 0;
}

// Función auxiliar recursiva para buscar en el árbol
TSnodo* buscarRecursivo(TSnodo *nodo, const char *lexema) {
    if (nodo == NULL) {
        return NULL;
    }
    
    int comparacion = strcmp(lexema, nodo->lexema);
    
    if (comparacion < 0) {
        return buscarRecursivo(nodo->izq, lexema);
    } 
    else if (comparacion > 0) {
        return buscarRecursivo(nodo->der, lexema);
    } 
    else {
        return nodo; // Encontrado
    }
}

// Buscar un símbolo en la tabla
TSnodo* buscarTS(const char *lexema) {
    return buscarRecursivo(raiz, lexema);
}

// Devuelve el identificador almacenado en un nodo de la TS
int obtenerIdentificadorTS(TSnodo *nodo) {
    if (nodo == NULL) return -1;
    return nodo->identificador;
}

// Incrementar el nivel (nuevo bloque)
void nuevoBloqueTS(void) {
    nivelActual++;
}

// Libera la lista de un nodo eliminando solo los elementos del nivel especificado
void eliminarNivel(TSnodo *nodo, int nivel) {
    if (nodo == NULL) return;
    
    ListaNodo *actual = nodo->lista;
    ListaNodo *anterior = NULL;
    
    while (actual != NULL) {
        if (actual->nivel == nivel) {
            // Eliminar este nodo de la lista
            if (anterior == NULL) {
                nodo->lista = actual->siguiente;
            } else {
                anterior->siguiente = actual->siguiente;
            }
            ListaNodo *temp = actual;
            actual = actual->siguiente;
            free(temp);
        } else {
            anterior = actual;
            actual = actual->siguiente;
        }
    }
}

// Encuentra el nodo mínimo (más a la izquierda) del subárbol
TSnodo* encontrarMinimo(TSnodo *nodo) {
    while (nodo->izq != NULL) {
        nodo = nodo->izq;
    }
    return nodo;
}

// Función auxiliar recursiva para eliminar nodos del árbol en un nivel específico
TSnodo* eliminarNodosNivelRecursivo(TSnodo *nodo, int nivel) {
    if (nodo == NULL) {
        return NULL;
    }
    
    // Recorrer el árbol recursivamente
    nodo->izq = eliminarNodosNivelRecursivo(nodo->izq, nivel);
    nodo->der = eliminarNodosNivelRecursivo(nodo->der, nivel);
    
    // Verificar si el nodo actual debe ser eliminado
    if (nodo->nivel == nivel && nodo->lista == NULL) {
        // Este nodo principal está en el nivel a eliminar y no tiene lista
        
        // Caso 1: Nodo sin hijos
        if (nodo->izq == NULL && nodo->der == NULL) {
            free(nodo->lexema);
            free(nodo);
            return NULL;
        }
        // Caso 2: Nodo con un hijo
        else if (nodo->izq == NULL) {
            TSnodo *temp = nodo->der;
            free(nodo->lexema);
            free(nodo);
            return temp;
        }
        else if (nodo->der == NULL) {
            TSnodo *temp = nodo->izq;
            free(nodo->lexema);
            free(nodo);
            return temp;
        }
        // Caso 3: Nodo con dos hijos
        else {
            TSnodo *sucesor = encontrarMinimo(nodo->der);
            free(nodo->lexema);
            nodo->lexema = (char*)malloc(strlen(sucesor->lexema) + 1);
            strcpy(nodo->lexema, sucesor->lexema);
            nodo->nivel = sucesor->nivel;
            nodo->identificador = sucesor->identificador;
            nodo->lista = sucesor->lista;
            nodo->der = eliminarNodosNivelRecursivo(nodo->der, nivel);
        }
    }
    else if (nodo->nivel == nivel && nodo->lista != NULL) {
        // Promover el primer elemento de la lista al nodo principal
        ListaNodo *primero = nodo->lista;
        nodo->nivel = primero->nivel;
        nodo->identificador = primero->identificador;
        nodo->lista = primero->siguiente;
        free(primero);
    }
    else {
        // Eliminar de la lista si existe en ese nivel
        eliminarNivel(nodo, nivel);
    }
    
    return nodo;
}

// Finalizar bloque (decrementar nivel y eliminar símbolos locales)
void finBloqueTS(void) {
    if (nivelActual <= 0) {
        fprintf(stderr, "Error: No hay bloques activos para finalizar\n");
        return;
    }
    
    raiz = eliminarNodosNivelRecursivo(raiz, nivelActual);
    nivelActual--;
}

// Contador para numerar los símbolos
static int contadorSimbolo = 0;

// Función auxiliar para imprimir el árbol en orden (in-orden)
static void imprimirRecursivo(TSnodo *nodo) {
    if (nodo == NULL) {
        return;
    }

    // Recorrer subárbol izquierdo (menores alfabéticamente)
    imprimirRecursivo(nodo->izq);

    // Imprimir nodo actual
    contadorSimbolo++;
    printf("  %-6d  %-20s\n",
           nodo->identificador, nodo->lexema);

    // Si hay lista, imprimir también esos símbolos
    if (nodo->lista != NULL) {
        ListaNodo *actual = nodo->lista;
        while (actual != NULL) {
            contadorSimbolo++;
            printf("  %-6d  %-20s\n",
                   actual->identificador, nodo->lexema);
            actual = actual->siguiente;
        }
    }

    // Recorrer subárbol derecho (mayores alfabéticamente)
    imprimirRecursivo(nodo->der);
}

// Imprimir la tabla de símbolos
void imprimirTS(void) {
    contadorSimbolo = 0;

    printf("  %-6s  %-20s\n", "ID", "LEXEMA");
    printf("  ------  --------------------\n");

    if (raiz == NULL) {
        printf("  (Tabla vacía)\n");
    } else {
        imprimirRecursivo(raiz);
    }

    printf("\n  Total de entradas: %d\n", contadorSimbolo);
}

// Liberar toda la memoria de la tabla
void liberarLista(ListaNodo *lista) {
    while (lista != NULL) {
        ListaNodo *temp = lista;
        lista = lista->siguiente;
        free(temp);
    }
}

void liberarArbolRecursivo(TSnodo *nodo) {
    if (nodo == NULL) return;
    
    liberarArbolRecursivo(nodo->izq);
    liberarArbolRecursivo(nodo->der);
    liberarLista(nodo->lista);
    free(nodo->lexema);
    free(nodo);
}

void destruirTS(void) {
    liberarArbolRecursivo(raiz);
    raiz = NULL;
    nivelActual = 0;
}

