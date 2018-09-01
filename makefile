all: main

main: varredura parse	# parse já inclui a árvore
	gcc main.c lexical/varredura.o tree.o syntactic/parse.o -o vcc
	#make clean

varredura:
	gcc -c lexical/varredura.c -o lexical/varredura.o

arvore:
	gcc -c tree.c -o tree.o

parse: arvore
	gcc -c syntactic/parse.c -o syntactic/parse.o

teste:
	./vcc test.tpp

clean:
	rm lexical/varredura.o
	rm tree.o
	rm syntactic/parse.o

print:
	echo "USE: make teste"