all: main

main: varredura
	gcc main.c lexical/varredura.o -o vcc -lpthread
	make clean

varredura:
	gcc -c lexical/varredura.c -o lexical/varredura.o

teste:
	./vcc test.tpp

clean:
	rm lexical/varredura.o

