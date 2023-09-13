CC = gcc
CFLAGS = -Wall -pedantic -std=c99
LFLAGS = -lm
SOURCES = $(wildcard *.c)
EXECUTABLES = $(SOURCES:.c=)

all: $(EXECUTABLES)
	./app ./test/*
# BORRAR LINEA DE ARRIBA ANTES DE ENTREGAR

%: %.c
	$(CC) $< $(CFLAGS) $(LFLAGS) -o $@

clean:
	rm -f $(EXECUTABLES) output.txt

.PHONY: clean all

