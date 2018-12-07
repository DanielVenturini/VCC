all: main											# compila todos os arquivos
install: main clean move xdot codeinstall highlight	# instala o vcc

# configurações para compilar geracao.c
CC = clang-3.5
CXX = clang++-3.5
LLVMCFG = llvm-config-3.5
OPT = opt-3.5
CFLAGS = -Wall `$(LLVMCFG) --cflags`
LDFLAGS = -Wall `$(LLVMCFG) --libs core --system-libs` `$(LLVMCFG) --ldflags`

# regras do make
# gera todos os arquivos .o e depois linka tudo
main: varredura parse semantic geracao	# parse já inclui a árvore
	gcc main.c lexical/varredura.o tree/tree.o syntactic/parse.o desacerto.o tree/stack.o semantic/poda.o semantic/tabsimb.o semantic/semantico.o codegenerator/geracao.o -o vcc
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

geracao:
	gcc -c codegenerator/geracao.c -o codegenerator/geracao.o $(CFLAGS)

clean:		# apaga os arquivos objetos de compilação
	rm lexical/varredura.o -f
	rm tree/tree.o -f
	rm syntactic/parse.o -f
	rm desacerto.o -f
	rm tree/pilha.o -f
	rm semantic/poda.o -f
	rm semantic/tabsimb.o -f
	rm semantic/semantico.o -f
	rm codegenerator/geracao.o -f

xdot:		# instala o xdot
	apt-get install xdot --allow-unauthenticated -y

codeinstall:	# instala llvm e clang, todos na versão 3.5; biblioteca necessária para o -ledit
	apt-get install clang-3.5 --allow-unauthenticated -y
	sudo apt-get install libedit-dev --allow-unauthenticated -y

move:		# move o executavel para a pasta /bin/
	mv vcc /usr/bin/

highlight:
	cd highlightSyntax/ && make