TARGET = bfc

SRCDIR=src
OBJDIR=obj

CC=gcc

CFLAGS = -O -std=c99 -Wall
LFLAGS = -Wall

SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SOURCES))

$(TARGET): $(OBJECTS)
	$(CC) $(LFLAGS) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

PHONY: clean install uninstall

clean:
	rm $(TARGET)
	rm $(OBJECTS)

install:
	install -m 775 $(TARGET) /usr/local/bin

uninstall:
	rm -rf /usr/loal/bin/$(TARGET)
