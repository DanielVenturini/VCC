#include "semantico.h"

void getTipso(TreeNode *node){
	char tipo = node->tipoExpressao;
	switch (node->bnfval) {
		case VAR:
			printf("VAR ");
			break;
		case DECLARACAO_FUNCAO:
			printf("DECLARACAO_FUNCAO ");
			break;
		default:
			printf("node->%d ", node->bnfval);
	}

	printf("%s ", (char *) node->token->val);

	switch(tipo) {
		case 0:
			printf("VOID");
			break;
		case FLUTUANTE:
			printf("FLUTUANTE");
			break;
		case INTEIRO:
			printf("INTEIRO");
			break;
		default:
			printf("tipo %d", tipo);
	}

	printf("\n");
}


// esta função existe apenas para alocar o escopo local caso precise
void insere(TabSimb *escopoSuperior, TabSimb *escopoLocal, TreeNode *var) {

	// deve procurar apenas no escopo local
	if(!contem(escopoLocal, (char *) var->token->tokenval, 0)) {
		// printa mensagem de erro
		return;
	} else {	// se não tem, então insere

		if(!escopoLocal)
			escopoLocal = criaTabSim(escopoSuperior);
	}
}


void procura(TabSimb *escopoSuperior, TabSimb *escopoLocal, TreeNode *filho) {

}

// tipoAnterior é quem é o pai deste nó st
// se o nó for um VAR e for uma lista_variaveis, declaracao_funcao, parametro
void recursivo(TabSimb *escopoSuperior, TreeNode *st, EBNFType tipoAnterior) {
	if(!st)
		return;

	unsigned char i;
	TabSimb *escopoLocal = NULL;			// somente se precisar alocar
	for(i = 0; st->filhos[i]; i ++) {		// cada uma das declaracoes globais

		TreeNode *filho = st->filhos[i];
		recursivo(escopoSuperior, filho, filho->bnfval);

		if(filho->bnfval == VAR || filho->bnfval == DECLARACAO_FUNCAO) {	// é uma variável ou uma função

			// se for um desses tipos, então adiciona na tabela de símbolos
			if(tipoAnterior == LISTA_VARIAVEIS || tipoAnterior == DECLARACAO_FUNCAO || tipoAnterior == PARAMETRO) {
				getTipso(filho);
				//insere(escopoSuperior, escopoLocal, filho);
			} else {	// então procura na tabela de símbolos
				procura(escopoSuperior, escopoLocal, filho);
			}
		}
	}
}


TabSimb *constroiTabSimb(TreeNode *st) {

	TabSimb *global = criaTabSim(NULL/*, "global\0"*/);		// NULL é o escopo superior, que no caso não existe
	recursivo(global, st, -1);

	return global;
}