# Makefile para el compilador

# Compilador y flags
CC = gcc
CFLAGS = -g -Wall

# Archivos objeto
OBJS = compilador.o TS.o errores.o lexico.o sintactico.o SE.o

# Ejecutable
TARGET = compilador

# Regla principal
all: $(TARGET)

# Regla para crear el ejecutable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Reglas para compilar archivos objeto
main.o: compilador.c TS.h errores.h
	$(CC) $(CFLAGS) -c main.c

TS.o: TS.c TS.h definiciones.h
	$(CC) $(CFLAGS) -c TS.c

errores.o: errores.c errores.h
	$(CC) $(CFLAGS) -c errores.c

lexico.o: lexico.c lexico.h definiciones.h
	$(CC) $(CFLAGS) -c lexico.c

sintactico.o: sintactico.c definiciones.h
	$(CC) $(CFLAGS) -c sintactico.c

SE.o: SE.c SE.h
	$(CC) $(CFLAGS) -c SE.c

# Regla para limpiar archivos generados
clean:
	rm -f $(OBJS) $(TARGET)

# Regla para ejecutar el programa
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
