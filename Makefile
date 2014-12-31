objects=src/bfc.o src/program.o src/util.o

bfc: $(objects)
	gcc -o bfc $(objects)

.PHONY: clean
clean:
	rm bfc $(objects)
