#include <stdio.h>
#include <stdlib.h>
#include "errores.h"
#include "lexico.h"
#include "definiciones.h"

void analisisSintactico(){
    // Procesar tokens hasta fin de fichero
    Token token;
    

    while ((siguienteToken(&token)) != EOF_TOKEN) {
        printf("  <%s, %d>\n", token.lexema, token.codigo);
    }

    // Imprimir el token EOF
    printf("  <%s, %d>\n", token.lexema, token.codigo);

    // Continuar con el análisis sintáctico en el futuro
}

