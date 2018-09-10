all: main

main: varredura parse desacerto	# parse já inclui a árvore
	gcc main.c lexical/varredura.o tree.o syntactic/parse.o desacerto.o -o vcc
	#make clean

varredura:
	gcc -c lexical/varredura.c -o lexical/varredura.o

arvore:
	gcc -c tree.c -o tree.o -Wformat=0	# a árvore contém conversões de tipos que não devem ser mostradas

parse: arvore
	gcc -c syntactic/parse.c -o syntactic/parse.o

desacerto:
	gcc -c desacerto.c -o desacerto.o

teste:
	./vcc test.tpp

clean:
	rm lexical/varredura.o
	rm tree.o
	rm syntactic/parse.o
	rm desacerto.o

print:
	echo "USE: make teste"

parse2: main
	./vcc testeparse.tpp