all: main								# compila todos os arquivos
install: main clean move xdot highlight	# instala o vcc

main: varredura parse semantic	# parse já inclui a árvore
	gcc main.c lexical/varredura.o tree/tree.o syntactic/parse.o desacerto.o tree/stack.o semantic/poda.o semantic/tabsimb.o semantic/semantico.o -o vcc
	make clean

varredura:
	gcc -c lexical/varredura.c -o lexical/varredura.o

arvore: pilha
	gcc -c tree/tree.c -o tree/tree.o -Wformat=0	# a árvore contém conversões de tipos que não devem ser mostradas

parse: arvore desacerto
	gcc -c syntactic/parse.c -o syntactic/parse.o

desacerto:
	gcc -c desacerto.c -o desacerto.o

pilha:	# a pilha é usado somente na árvore
	gcc -c tree/stack.c -o tree/stack.o

tabsimb:
	gcc -c semantic/tabsimb.c -o semantic/tabsimb.o

semantic: poda tabsimb
	gcc -c semantic/semantico.c -o semantic/semantico.o

poda:
	gcc -c semantic/poda.c -o semantic/poda.o

clean:		# apaga os arquivos objetos de compilação
	rm lexical/varredura.o -f
	rm tree/tree.o -f
	rm syntactic/parse.o -f
	rm desacerto.o -f
	rm tree/pilha.o -f
	rm semantic/poda.o -f
	rm semantic/tabsimb.o -f
	rm semantic/semantico.o -f

xdot:		# instala o xdot
	apt-get install xdot --allow-unauthenticated -y

move:		# move o executavel para a pasta /bin/
	mv vcc /usr/bin/

highlight:
	cd highlightSyntax/ && make