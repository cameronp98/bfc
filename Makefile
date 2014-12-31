objects=bfc.o program.o util.o

bfc: $(objects)
	gcc -o bfc $(objects)

.PHONY: clean
clean:
	rm bfc $(objects)
