CC      ?= gcc
CFLAGS  ?= -Wall -Wextra -Werror -O2 -std=c99
LDFLAGS ?=

TARGET  = aggregate6
SRC     = src/main.c src/aggregate.c src/trie.c
OBJ     = $(SRC:.c=.o)
DEP     = $(SRC:.c=.d)

PREFIX  ?= /usr/local

.PHONY: all clean install test

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -MMD -MP -c -o $@ $<

-include $(DEP)

clean:
	rm -f $(TARGET) $(OBJ) $(DEP)

install: $(TARGET)
	install -d $(DESTDIR)$(PREFIX)/bin
	install -m 755 $(TARGET) $(DESTDIR)$(PREFIX)/bin/

test: $(TARGET)
	sh tests/test-basic.sh ./$(TARGET)
