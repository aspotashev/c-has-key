CC=gcc
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=buffer.c trie.c main.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=has_key

all: $(SOURCES) $(EXECUTABLE)

clean:
	rm -f *.o has_key

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@
