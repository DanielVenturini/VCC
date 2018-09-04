#include "tree.h"

void printFile(TokenRecord *token, FILE *treedot, char identacao, char seta, char final);

void nullPosicoes(TreeNode *node, char inicial, char final){
	char i;
	for(i = inicial; i < final; i ++)
		node->filhos[i] = NULL;
}

TreeNode *novo_node(TokenRecord *token){
	// aloca os devidos ponteiros
	TreeNode *node = (TreeNode *) malloc(sizeof(TreeNode));
	node->filhos = (TreeNode **) malloc(NUM_INICIAL_FILHOS*sizeof(TreeNode*));

	// seta como NULL, pois, inicialmente, não terão nada
	node->token = token;
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

	printFile(pai->token, treedot, 1, 0, 1);	// token, identacao, seta, final - este último parametro como 1 para colocar o ';\n'

	char i;
	for(i = 0; pai->filhos[i]; i ++){			// adiciona 'pai -> filho' para cada filho
		printFile(pai->token, treedot, 1, 1, 0);
		printFile(pai->filhos[i]->token, treedot, 0, 0, 1);
	}

	for(i = 0; pai->filhos[i]; i ++)			// chama para cada filho também
		getArvoreRecursiva(pai->filhos[i], treedot);
}

void printArvore(TreeNode *raiz){
	FILE *treedot = fopen("tree.dot", "w");	// abrindo o arquivo
	fprintf(treedot, "digraph G {\n");		// imprime cabeçalho

	getArvoreRecursiva(raiz, treedot);		// recursivamente vai adicionando no arquivo

	fprintf(treedot, "}");					// finaliza o digrafo com o '}'
	fclose(treedot);						// fecha o arquivo
	system("xdot tree.dot");				// printa a arvore com o xdot - graphviz
}

void printFile(TokenRecord *token, FILE *treedot, char identacao, char seta, char final) {
	printToken(token, 0, 0);
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