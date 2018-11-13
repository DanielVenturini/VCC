#include "semantico.h"

char *filename;

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


// função responsável por procurar e inserir um nó na tabela de símbolos
void insere(TabSimb *escopoLocal, TreeNode *var, TokenType tipoAnterior) {

	char *id = (char *) var->token->val;
	char funcao = tipoAnterior == DECLARACAO_FUNCAO ? 1 : 0;	// recupera se este é uma variavel ou uma declaracao_funcao

	// deve procurar apenas no escopo local
	if(contem(escopoLocal, id, 0, funcao)) {
		erro(filename, var->token, "Redeclaração de variável no mesmo escopo.", 0);
	} else {	// se não tem, então insere

		insere_escopo(escopoLocal, id, var->tipoExpressao, funcao);
	}
}

// função responsável por procurar um nó na tabela
void procura(TabSimb *escopoLocal, TreeNode *var, TokenType tipoAnterior) {

	char *id = (char *) var->token->val;
	char funcao = tipoAnterior == CHAMADA_FUNCAO ? 1 : 0;		// recupera se este é uma variavel ou uma declaracao_funcao

	// procura em todos os escopos passados
	// se não tiver
	Identificador *identificador = contem(escopoLocal, id, 1, funcao);
	if(!identificador) {
		erro(filename, var->token, "Variável não declarada neste e nem nos escopos superiores.", 0);
		insere_escopo(escopoLocal, id, var->tipoExpressao, funcao);	// insere na tabela, para suprimir os erros que 
	}
}


// tipoAnterior é quem é o pai deste nó st
// se o nó for um VAR e  o tipoAnterior for lista_variaveis, declaracao_funcao, parametro
void recursivo(TabSimb *escopoLocal, TreeNode *st, EBNFType tipoAnterior) {
	if(!st)
		return;

	unsigned char i;
	for(i = 0; st->filhos[i]; i ++) {		// cada uma das declaracoes globais
		TreeNode *filho = st->filhos[i];

		TabSimb *escopoInferior;
		// se começar um novo bloco
		if(filho->bnfval == B_SE || filho->bnfval == B_REPITA || filho->bnfval == DECLARACAO_FUNCAO) {

			if(filho->bnfval == DECLARACAO_FUNCAO) {		// o nome da função é no escopo global, mas o resto é local
				TreeNode *var = filho->filhos[1]->bnfval == VAR ? filho->filhos[1] : filho->filhos[0];
				recursivo(escopoLocal, var, filho->bnfval);	// adiciona o ID no escopo global
			}

			// cria um novo escopo
			escopoInferior = criaTabSim(escopoLocal);
			recursivo(escopoInferior, filho, st->bnfval);	// utiliza o novo escopo
		} else {
			recursivo(escopoLocal, filho, st->bnfval);		// utiliza o escopo atual
		}
	}

	if(st->bnfval == VAR) {

		// se for um desses tipos, então adiciona na tabela de símbolos
		if(tipoAnterior == LISTA_VARIAVEIS || tipoAnterior == DECLARACAO_FUNCAO || tipoAnterior == PARAMETRO) {
			//getTipso(st);
			insere(escopoLocal, st, tipoAnterior);
		} else {	// então procura na tabela de símbolos
			//getTipso(st);
			procura(escopoLocal, st, tipoAnterior);
		}
	}
}


TabSimb *constroiTabSimb(TreeNode *st, char *nomeArquivo) {

	filename = nomeArquivo;
	TabSimb *programa = criaTabSim(NULL/*, "global\0"*/);	// escopo superior, que no caso não existe
	TabSimb *global = criaTabSim(programa);					// escopo global

	recursivo(global, st, -1);

	return global;
}