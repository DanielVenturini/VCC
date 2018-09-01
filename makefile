all: main

main: varredura arvore
	gcc main.c lexical/varredura.o tree.o -o vcc
	make clean
	make print

varredura:
	gcc -c lexical/varredura.c -o lexical/varredura.o

arvore:
	gcc -c tree.c -o tree.o

parse: arvore
	gcc syntactic/parse.c tree.o -o syntactic/parse
	./syntactic/parse

teste:
	./vcc test.tpp

clean:
	rm lexical/varredura.o
	rm tree.o
	rm syntactic/parse.o

print:
	echo "USE: make teste"