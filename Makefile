TARGET = bfc

SRCDIR=src
OBJDIR=obj
BINDIR=bin

CC=gcc

CFLAGS = -std=c99 -Wall -g -I $(SRCDIR)
LFLAGS = -Wall

SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SOURCES))

$(BINDIR)/$(TARGET): $(OBJECTS)
	$(CC) $(LFLAGS) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

PHONY: clean
clean:
	rm bin/$(TARGET)
	rm $(OBJECTS)
