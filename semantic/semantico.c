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
void insere(TabSimb *escopoSuperior, TabSimb *escopoLocal, TreeNode *var, TokenType tipoAnterior) {

	char *id = (char *) var->token->tokenval;
	char funcao = tipoAnterior == DECLARACAO_FUNCAO ? 1 : 0;	// recupera se este é uma variavel ou uma declaracao_funcao

	// deve procurar apenas no escopo local
	if(!contem(escopoLocal, id, 0, funcao)) {
		//printf("ERRO: %s já existe neste escopo.\n", id);
		return;
	} else {	// se não tem, então insere

		if(!escopoLocal)	// se é a primeira variável do escopo
			escopoLocal = criaTabSim(escopoSuperior);

		//printf("INSERINDO: %s.\n", id);
		insere_escopo(escopoLocal, id, var->tipoExpressao, funcao);
	}
}


void procura(TabSimb *escopoSuperior, TabSimb *escopoLocal, TreeNode *filho) {

}

// tipoAnterior é quem é o pai deste nó st
// se o nó for um VAR e  o tipoAnterior for lista_variaveis, declaracao_funcao, parametro
void recursivo(TabSimb *escopoSuperior, TreeNode *st, EBNFType tipoAnterior) {
	if(!st)
		return;

	unsigned char i;
	TabSimb *escopoLocal = NULL;			// somente se precisar alocar
	for(i = 0; st->filhos[i]; i ++) {		// cada uma das declaracoes globais
		TreeNode *filho = st->filhos[i];
		recursivo(escopoSuperior, filho, st->bnfval);
	}

	if(st->bnfval == VAR) {

		// se for um desses tipos, então adiciona na tabela de símbolos
		if(tipoAnterior == LISTA_VARIAVEIS || tipoAnterior == DECLARACAO_FUNCAO || tipoAnterior == PARAMETRO) {
		   	printf("-");
			getTipso(st);
			insere(escopoSuperior, escopoLocal, st, tipoAnterior);
		} else {	// então procura na tabela de símbolos
			getTipso(st);
			procura(escopoSuperior, escopoLocal, st);
		}
	}
}


TabSimb *constroiTabSimb(TreeNode *st) {

	TabSimb *global = criaTabSim(NULL/*, "global\0"*/);		// NULL é o escopo superior, que no caso não existe
	recursivo(global, st, -1);

	return global;
}