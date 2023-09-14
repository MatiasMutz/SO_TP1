CC = gcc
CFLAGS = -Wall -pedantic -fsanitize=address -g
LFLAGS = -lm
SOURCES = $(wildcard *.c) $(wildcard lib/*.c)
EXECUTABLES = $(SOURCES:.c=)

all: $(EXECUTABLES)
	./app ./test/*
# BORRAR LINEA DE ARRIBA ANTES DE ENTREGAR

%: %.c
	$(CC) $< $(CFLAGS) $(LFLAGS) -o $@

clean:
	rm -f $(EXECUTABLES) *.txt

.PHONY: clean all

