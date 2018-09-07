#include "tree.h"

void printFile1(TreeNode *node, FILE *treedot, char identacao, char seta, char final);
void printFile2(TokenRecord *token, FILE *treedot, char identacao, char seta, char final);

void nullPosicoes(TreeNode *node, char inicial, char final){
	char i;
	for(i = inicial; i < final; i ++)
		node->filhos[i] = NULL;
}

TreeNode *novo_node(TokenRecord *token, BNFType bnfval){
    // aloca os devidos ponteiros
    TreeNode *node = (TreeNode *) malloc(sizeof(TreeNode));
    node->filhos = (TreeNode **) malloc(NUM_INICIAL_FILHOS*sizeof(TreeNode*));

    // seta como NULL, pois, inicialmente, não terão nada
    node->token = token;
    node->bnfval = bnfval;
    node->max_filhos = NUM_INICIAL_FILHOS;

    nullPosicoes(node, 0, node->max_filhos);

    return node;
}

// realoca a quantidade de filhos que este pai tem
void realocaFilhos(TreeNode *node) {
	char novo_tamanho = node->max_filhos + NUM_INICIAL_FILHOS;
	node->max_filhos = novo_tamanho;

	node->filhos = (TreeNode **) realloc(node->filhos, novo_tamanho*sizeof(TreeNode *));

	nullPosicoes(node, node->max_filhos-NUM_INICIAL_FILHOS, node->max_filhos);
}

void insere_filho(TreeNode *pai, TreeNode *filho){
	if(!pai || !filho)
		return;

	unsigned char i = 0;

	while (pai->filhos[i]) {		// enquanto tiver filhos já alocado

		i ++;

		if(pai->max_filhos == i)	// se o pai não tiver mais espaço para filhos
			realocaFilhos(pai);		// realoca o número de filhos
	}

	pai->filhos[i] = filho;		// insere o filho
}

void getArvoreRecursiva(TreeNode *pai, FILE *treedot) {
	if(!pai)
		return;

	printFile1(pai, treedot, 1, 0, 1);	// token, identacao, seta, final - este último parametro como 1 para colocar o ';\n'

	char i;
	for(i = 0; pai->filhos[i]; i ++){			// adiciona 'pai -> filho' para cada filho
		printFile1(pai, treedot, 1, 1, 0);
		printFile1(pai->filhos[i], treedot, 0, 0, 1);
	}

	for(i = 0; pai->filhos[i]; i ++)			// chama para cada filho também
		getArvoreRecursiva(pai->filhos[i], treedot);
}

void printArvore(TreeNode *raiz){
    if(!raiz)
        return;

	FILE *treedot = fopen("tree.dot", "w");	// abrindo o arquivo
	fprintf(treedot, "digraph G {\n");		// imprime cabeçalho

	getArvoreRecursiva(raiz, treedot);		// recursivamente vai adicionando no arquivo

	fprintf(treedot, "}");					// finaliza o digrafo com o '}'
	fclose(treedot);						// fecha o arquivo
	system("xdot tree.dot");				// printa a arvore com o xdot - graphviz
}

// printa no arquivos os nós intermediários
void printFile1(TreeNode *node, FILE *treedot, char identacao, char seta, char final){
    if(node->token) {      // se tiver um token, então é uma folha
        printFile2(node->token, treedot, identacao, seta, final);
        return;
    }

    if(identacao)           // apenas para identar
        fprintf(treedot, "    ");

    if (node->bnfval == PROGRAMA)
        fprintf(treedot, "\"PROGRAMA\"");
    else if (node->bnfval == LISTA_DECLARACOES)
        fprintf(treedot, "\"LISTA_DECLARACOES\"");
    else if (node->bnfval == DECLARACAO)
        fprintf(treedot, "\"DECLARACAO\"");
    else if (node->bnfval == DECLARACAO_VARIAVEIS)
        fprintf(treedot, "\"DECLARACAO_VARIAVEIS\"");
    else if (node->bnfval == INICIALIZACAO_VARIAVEIS)
        fprintf(treedot, "\"INICIALIZACAO_VARIAVEIS\"");
    else if (node->bnfval == LISTA_VARIAVEIS)
        fprintf(treedot, "\"LISTA_VARAIVEIS\"");
    else if (node->bnfval == VAR)
        fprintf(treedot, "\"VAR\"");
    else if (node->bnfval == INDICE)
        fprintf(treedot, "\"INDICE\"");
    else if (node->bnfval == TIPO)
        fprintf(treedot, "\"TIPO\"");
    else if (node->bnfval == DECLARACAO_FUNCAO)
        fprintf(treedot, "\"DECLARACAO_FUNCAO\"");
    else if (node->bnfval == CABECALHO)
        fprintf(treedot, "\"CABECALHO\"");
    else if (node->bnfval == LISTA_PARAMETROS)
        fprintf(treedot, "\"LISTA_PARAMETROS\"");
    else if (node->bnfval == PARAMETRO)
        fprintf(treedot, "\"PARAMETRO\"");
    else if (node->bnfval == CORPO)
        fprintf(treedot, "\"CORPO\"");
    else if (node->bnfval == ACAO)
        fprintf(treedot, "\"ACAO\"");
    else if (node->bnfval == B_SE)
        fprintf(treedot, "\"SE\"");
    else if (node->bnfval == B_REPITA)
        fprintf(treedot, "\"REPITA\"");
    else if (node->bnfval == B_LEIA)
        fprintf(treedot, "\"LEIA\"");
    else if (node->bnfval == B_ESCREVA)
        fprintf(treedot, "\"ESCREVA\"");
    else if (node->bnfval == B_RETORNA)
        fprintf(treedot, "\"RETORNA\"");
    else if (node->bnfval == EXPRESSAO)
        fprintf(treedot, "\"EXPRESSAO\"");
    else if (node->bnfval == EXPRESSAO_LOGICA)
        fprintf(treedot, "\"EXPRESSAO_LOGICA\"");
    else if (node->bnfval == EXPRESSAO_SIMPLES)
        fprintf(treedot, "\"EXPRESSAO_SIMPLES\"");
    else if (node->bnfval == EXPRESSAO_ADITIVA)
        fprintf(treedot, "\"EXPRESSAO_ADITIVA\"");
    else if (node->bnfval == EXPRESSAO_MULTIPLICATIVA)
        fprintf(treedot, "\"EXPRESSAO_MULTIPLICATIVA\"");
    else if (node->bnfval == EXPRESSAO_UNARIA)
        fprintf(treedot, "\"EXPRESSAO_UNARIA\"");
    else if (node->bnfval == OPERADOR_RELACIONAL)
        fprintf(treedot, "\"OPERADOR_RELACIONAL\"");
    else if (node->bnfval == OPERADOR_SOMA)
        fprintf(treedot, "\"OPERADOR_SOMA\"");
    else if (node->bnfval == OPERADOR_LOGICO)
        fprintf(treedot, "\"OPERADOR_LOGICO\"");
    else if (node->bnfval == OPERADOR_MULTIPLICACAO)
        fprintf(treedot, "\"OPERADOR_MULTIPLICACAO\"");
    else if (node->bnfval == FATOR)
        fprintf(treedot, "\"FATOR\"");
    else if (node->bnfval == NUMERO)
        fprintf(treedot, "\"NUMERO\"");
    else if (node->bnfval == CHAMADA_FUNCAO)
        fprintf(treedot, "\"CHAMADA_FUNCAO\"");
    else if (node->bnfval == LISTA_ARGUMENTOS)
        fprintf(treedot, "\"LISTA_ARGUMENTOS\"");

    if(seta)        // antes do filho
        fprintf(treedot, " -> ");

    if(final)       // apenas para finalizar com o ';\n'
        fprintf(treedot, ";\n");
}

// printa no arquivos as folhas
void printFile2(TokenRecord *token, FILE *treedot, char identacao, char seta, char final) {
	if(identacao)			// apenas para identar
		fprintf(treedot, "    ");

	if (token->tokenval == ID)
        fprintf(treedot, "\"ID-%s, %d:%d\"", (char *) token->val, token->numline, token->numcaracter);
    else if (token->tokenval == NUM_I)
        fprintf(treedot, "\"NUM-%d, %d:%d\"", *(int *) token->val, token->numline, token->numcaracter);
    else if (token->tokenval == NUM_F)
        fprintf(treedot, "\"NUM-%f, %d:%d\"", *((float *) token->val), token->numline, token->numcaracter);
    else if (token->tokenval == ATE)
        fprintf(treedot, "\"ATÉ, %d-%d", token->numline, token->numcaracter);
    else if (token->tokenval == ENTAO)
        fprintf(treedot, "\"ENTÃO, %d:%d\"", token->numline, token->numcaracter);
    else if (token->tokenval == ESCREVA)
        fprintf(treedot, "\"ESCREVA, %d:%d\"", token->numline, token->numcaracter);
    else if (token->tokenval == FIM)
        fprintf(treedot, "\"FIM, %d:%d\"", token->numline, token->numcaracter);
    else if (token->tokenval == FLUTUANTE)
        fprintf(treedot, "\"FLUTUANTE, %d:%d\"", token->numline, token->numcaracter);
    else if (token->tokenval == INTEIRO)
        fprintf(treedot, "\"INTEIRO, %d:%d\"", token->numline, token->numcaracter);
    else if (token->tokenval == LEIA)
        fprintf(treedot, "\"LEIA, %d:%d\"", token->numline, token->numcaracter);
    else if (token->tokenval == REPITA)
        fprintf(treedot, "\"REPITA, %d:%d\"", token->numline, token->numcaracter);
    else if (token->tokenval == RETORNA)
        fprintf(treedot, "\"RETORNA, %d:%d\"", token->numline, token->numcaracter);
    else if (token->tokenval == SE)
        fprintf(treedot, "\"SE, %d:%d\"", token->numline, token->numcaracter);
    else if (token->tokenval == SENAO)
        fprintf(treedot, "\"SENÃO, %d:%d\"", token->numline, token->numcaracter);
    else if (token->tokenval == SOMA)
        fprintf(treedot, "\"+ , %d:%d\"", token->numline, token->numcaracter);
    else if (token->tokenval == SUBTRACAO)
        fprintf(treedot, "\"- , %d:%d\"", token->numline, token->numcaracter);
    else if (token->tokenval == MULTIPLICACAO)
        fprintf(treedot, "\"* , %d:%d\"", token->numline, token->numcaracter);
    else if (token->tokenval == DIVISAO)
        fprintf(treedot, "\"/ , %d:%d\"", token->numline, token->numcaracter);
    else if (token->tokenval == MAIOR)
        fprintf(treedot, "\"> , %d:%d\"", token->numline, token->numcaracter);
    else if (token->tokenval == MENOR)
        fprintf(treedot, "\"< , %d:%d\"", token->numline, token->numcaracter);
    else if (token->tokenval == MAIOR_IGUAL)
        fprintf(treedot, "\">= , %d:%d\"", token->numline, token->numcaracter);
    else if (token->tokenval == MENOR_IGUAL)
        fprintf(treedot, "\"<= , %d:%d\"", token->numline, token->numcaracter);
    else if (token->tokenval == IGUALDADE)
        fprintf(treedot, "\"= , %d:%d\"", token->numline, token->numcaracter);
    else if (token->tokenval == ABRE_COLCHETES)
        fprintf(treedot, "\"[ , %d:%d\"", token->numline, token->numcaracter);
    else if (token->tokenval == FECHA_COLCHETES)
        fprintf(treedot, "\"] , %d:%d\"", token->numline, token->numcaracter);
    else if (token->tokenval == ABRE_PARENTESES)
        fprintf(treedot, "\"( , %d:%d\"", token->numline, token->numcaracter);
    else if (token->tokenval == FECHA_PARENTESES)
        fprintf(treedot, "\") , %d:%d\"", token->numline, token->numcaracter);
    else if (token->tokenval == VIRGULA)
        fprintf(treedot, "\", , %d:%d\"", token->numline, token->numcaracter);
    else if (token->tokenval == DOIS_PONTOS)
        fprintf(treedot, "\": , %d:%d\"", token->numline, token->numcaracter);
    else if (token->tokenval == ATRIBUICAO)
        fprintf(treedot, "\":= , %d:%d\"", token->numline, token->numcaracter);
    else if (token->tokenval == E_LOGICO)
        fprintf(treedot, "\"&& , %d:%d\"", token->numline, token->numcaracter);
    else if (token->tokenval == NEGACAO)
        fprintf(treedot, "\"! , %d:%d\"", token->numline, token->numcaracter);
    else if (token->tokenval == OU_LOGICO)
        fprintf(treedot, "\"|| , %d:%d\"", token->numline, token->numcaracter);
    else if (token->tokenval == DIFERENTE)
        fprintf(treedot, "\"<> , %d:%d\"", token->numline, token->numcaracter);
    else if (token->tokenval == NAO_IDENTIFICADO)
        fprintf(treedot, "\"NÃO_IDENTIFICADO-%s, %d:%d\"", (char *) token->val, token->numline, token->numcaracter);

    if(seta)		// antes do filho
    	fprintf(treedot, " -> ");

    if(final)		// apenas para finalizar com o ';\n'
    	fprintf(treedot, ";\n");
}