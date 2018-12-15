all: main											# compila todos os arquivos

install: xdot codeinstall main clean move highlight	# instala o vcc


# configurações para compilar geracao.c
CC = clang-3.5
CXX = clang++-3.5
LLVMCFG = llvm-config-3.5
OPT = opt-3.5
CFLAGS = -Wall `$(LLVMCFG) --cflags`
LDFLAGS = -Wall `$(LLVMCFG) --libs --system-libs` `$(LLVMCFG) --ldflags`

# regras do make
# gera todos os arquivos .o e depois linka tudo
main: varredura parse semantic geracao # parse já inclui a árvore
	gcc -c main.c -o main.o
	$(CXX) -g main.o lexical/varredura.o tree/tree.o syntactic/parse.o desacerto.o tree/stack.o semantic/poda.o semantic/tabsimb.o semantic/semantico.o codegenerator/geracao.o -o vcc $(LDFLAGS)
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
	$(CC) -c codegenerator/geracao.c -o codegenerator/geracao.o $(CFLAGS)

depurar:
	gdb -q --args ./vcc exemplos/gencode-001.tpp -c

codigo:
	./vcc exemplos/teste.tpp -c 2> exemplos/teste.ll
	llc-3.5 exemplos/teste.ll -o exemplos/teste.s
	gcc exemplos/teste.s -o exemplos/teste.exe
	./exemplos/teste.exe
	echo $?

clean:		# apaga os arquivos objetos de compilação
	rm main.o -f
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
	apt-get install llvm-3.5 --allow-unauthenticated -y
	apt-get install libedit-dev --allow-unauthenticated -y
	#apt-get install libcxxtools-dev --allow-unauthenticated -y
	#apt-get install libcxxtools9v5 --allow-unauthenticated -y
	cp /usr/lib/llvm-3.5/include/llvm-c/ /usr/include/ -r
	cp /usr/lib/llvm-3.5/include/llvm/ /usr/include/ -r

gdbinstall:		# depurador para C
	apt-get install gdb --allow-unauthenticated -y

move:		# move o executavel para a pasta /bin/
	mv vcc /usr/bin/

highlight:
	cd highlightSyntax/ && make