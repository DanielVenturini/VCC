#include "tabsimb.h"


// dado o nó VAR e o tipo, retorna um Identificador
Identificador *cria_identificador(TreeNode *no, TokenType tipo, char funcao) {

	Identificador *id = (Identificador *) malloc(sizeof(Identificador));

	id->tipo = tipo;
	id->nome = (char *) no->token->val;
	id->erro = 0;
	id->funcao = funcao;
	id->iniciada = 0;
	id->utilizada = 0;
	id->proximo = NULL;

	return id;
}


TabSimb *criaTabSim(TabSimb *escopoSuperior/*, char *nome*/) {

	TabSimb *tabsimb = (TabSimb *) malloc(sizeof(TabSimb));
	tabsimb->escopoSuperior = (struct TabSimb *) escopoSuperior;

	tabsimb->declarados = NULL;
	//tabsimb->nome = nome;

	return tabsimb;
}


// procura e retorna o id dentro do escopo.
// se o escopoProcura for 0, procurar apenas no escopo local
// se o escopoProcura for 1, procurar nos escopos superiores
Identificador *contem(TabSimb *escopo, char *id, char escopoProcura) {
	if(!escopo)		// se não houver nenhuma variável no escopo
		return NULL;

	Identificador *identificador = escopo->declarados[0];
	while(identificador) {
		if(id == identificador->nome) {					// se achou o identificador na tabela de símbolos
			return identificador;
		}

		identificador = (Identificador *) identificador->proximo;		// avança para o próximo identificador
	}

	if(escopoProcura) {		// se é para procurar somente no local e não foi encontrado
		return NULL;
	} else {
		return contem((TabSimb *) escopo->escopoSuperior, id, 1);	// procura no escopo superior
	}
}


void inserir(TabSimb *tabsimb, char *id) {

}