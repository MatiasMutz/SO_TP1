CC=gcc
CFLAGS =-g -Wall -Wextra -pedantic -fsanitize=address
CLIBS = -lrt -lpthread -lm
SOURCES = $(wildcard lib/*.c)
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLES = app slave view
INCLUDES = $(wildcard *.h) $(wildcard lib/*.h)

all: $(EXECUTABLES)
	make clean_objects

%.o: %.c $(SOURCES) $(INCLUDES)
	@$(CC) $(CFLAGS) -c $< -o $@

$(EXECUTABLES): %: %.o $(OBJECTS)
	@$(CC) $(CFLAGS) $< $(OBJECTS) -o $@ $(CLIBS)

.PHONY: clean all
clean:
	@rm -rf $(EXECUTABLES) *.txt $(OBJECTS) *.o

clean_objects:
	@rm -rf *.o lib/*.o

