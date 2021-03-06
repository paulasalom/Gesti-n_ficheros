CC=gcc
CFLAGS=-c -g -Wall -std=gnu99 #-std=c99
LDFLAGS=-pthread

SOURCES=bloques.c mi_mkfs.c ficheros_basico.c leer_sf.c ficheros.c escribir.c leer.c permitir.c truncar.c #todos los .c
LIBRARIES=bloques.o ficheros_basico.o ficheros.o#todos los .o de la biblioteca del SF
INCLUDES=bloques.h ficheros_basico.h ficheros.h #todos los .h
PROGRAMS=mi_mkfs leer_sf escribir leer permitir truncar #todos los main
OBJS=$(SOURCES:.c=.o)

all: $(OBJS) $(PROGRAMS)

$(PROGRAMS): $(LIBRARIES) $(INCLUDES)
	$(CC) $(LDFLAGS) $(LIBRARIES) $@.o -o $@

%.o: %.c $(INCLUDES)
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	rm -rf *.o *~ $(PROGRAMS)