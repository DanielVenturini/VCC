#include "tree.h"

TreeNode *novo_node(char letra){
	// aloca os devidos ponteiros
	TreeNode *node = (TreeNode *) malloc(sizeof(TreeNode));
	node->filhos = (TreeNode **) malloc(NUM_INICIAL_FILHOS*sizeof(TreeNode*));

	// seta como NULL, pois, inicialmente, não terão nada
	node->token = NULL;
	node->filhos[0] = NULL;
	node->letra = letra;
	node->max_filhos = NUM_INICIAL_FILHOS;

	return node;
}

// realoca a quantidade de filhos que este pai tem
void realocaFilhos(TreeNode *node) {
	char novo_tamanho = node->max_filhos + NUM_INICIAL_FILHOS;
	node->max_filhos = novo_tamanho;

	node->filhos = (TreeNode **) realloc(node->filhos, novo_tamanho*sizeof(TreeNode *));
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

	fprintf(treedot, "    %c;\n", pai->letra);	//se tiver somente o pai, ele tem que aparecer

	char i;
	for(i = 0; pai->filhos[i]; i ++)	// adiciona 'pai -> filho' para cada filho
		fprintf(treedot, "    %c -> %c;\n", pai->letra, pai->filhos[i]->letra);

	for(i = 0; pai->filhos[i]; i ++)	// chama para cada filho também
		getArvoreRecursiva(pai->filhos[i], treedot);
}

void printArvore(TreeNode *raiz){
	FILE *treedot = fopen("tree.dot", "w");	// abrindo o arquivo
	fprintf(treedot, "digraph G {\n");		// imprime cabeçalho

	getArvoreRecursiva(raiz, treedot);		// recursivamente vai adicionando no arquivo

	fprintf(treedot, "}");
	fclose(treedot);						// fecha o arquivo
	system("xdot tree.dot");				// printa a arvore com o xdot - graphviz
}