#include "desacerto.h"

FILE *leitor;	// ponteiro para o arquivo atual com o erro

// esta função apenas abre o arquivo
char abreArquivo(char *nomeArquivo) {
	leitor = fopen(nomeArquivo, "r");

	if(leitor)
		return 1;
	else
		return 0;
}

// esta função avança no arquivo até a linha determinada
void avancaLinha(unsigned short int numline){

	unsigned short int lineAtual = 1;

	while(lineAtual < numline-1){		// lê até uma linha antes, para depois imprimir as duas linhas que possivelmente terão o erro
		while(getc(leitor) != '\n');	// enquanto não chegar no fim da linha
		lineAtual ++;
	}
}

// esta função printa as informações sobre o erro/warning:
/* arquivo.tpp:linha:coluna: erro: esperado ‘TOKENTYPE’, ‘TOKENTYPE’ ou ‘TOKENTYPE’ antes de ‘TOKENTYPE‘
*  LINHA DO ARQUIVO COM ERRO */
void printInformacoes(char *nomeArquivo, TokenRecord *token, char *msgErro, char erro){
	// \e[38=cor_fonte; 5=sei_la; 255=corm
	printf("\e[38;5;184m");		// cor branco
	printf("%s:", nomeArquivo);
	if(token) {		// se for passado o token
		printf("%d:", token->numline);
		printf("%d:", token->numcaracter);
	}

	if(erro) {
		printf("\e[38;5;196m");		// cor vermelho
		printf(" Erro: ");
	} else {
		printf("\e[38;5;123m");		// cor vermelho
		printf(" Aviso: ");
	}

	printf("%s\n", msgErro);
}

void printLinhaAnterior(unsigned short int numline){

	if(numline == 1)	// não tem linha anterior
		return;

	char c;
	while((c = getc(leitor)) != '\n'){	// imprime a linha
		if(c == '	') {
			printf("    ");				// se for tab -|, printa direto os quatro espaços
		} else
			printf("%c", c);
	}

	printf("\n");
}

// esta função printa a linha especifica do erro
// e marca abaixo a coluna do token que gerou o erro
void printLine(unsigned short int numline, unsigned char posLine) {

	char c;
	char qtdTab = 1;

	printf("\e[38;5;255m");				// cor branco
	printLinhaAnterior(numline);
	c = getc(leitor);

	while(c != '\n' && c != EOF){	// imprime a linha
		if(c == '	') {
			qtdTab ++;
			printf("    ");				// se for tab -|, printa direto os quatro espaços
		} else
			printf("%c", c);

		c = getc(leitor);
	}

	printf("\n");
	unsigned char i;
	for(i = 1; i < qtdTab; i ++)		// printa as tabs
		printf("    ");

	for(; i < posLine; i ++)
		printf(" ");					// imprime nada até chegar na posição

	printf("\e[38;5;046m");				// cor verde
	printf("^\n");
}

// sair diz se encerra a execução do vcc
void erro(char *nomeArquivo, TokenRecord *token, char *msgErro, char sair, char erro) {

	if(!abreArquivo(nomeArquivo)) {
		printf("Erro: arquivo %s com erro não existe.\n", nomeArquivo);
		return;
	}

	printInformacoes(nomeArquivo, token, msgErro, erro);
	if(!token)
		return;

	avancaLinha(token->numline);
	printLine(token->numline, token->numcaracter);

	if(sair)
		exit(0);
}