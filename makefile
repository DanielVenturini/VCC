all: main						# compila todos os arquivos
install: main clean move xdot	# instala o vcc

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

clean:		# apaga os arquivos objetos de compilação
	rm lexical/varredura.o -f
	rm tree.o -f
	rm syntactic/parse.o -f
	rm desacerto.o -f

xdot:		# instala o xdot
	apt-get install xdot --allow-unauthenticated

move:		# move o executavel para a pasta /bin/
	mv vcc /usr/bin/