#include "tabsimb.h"


// dado o nó VAR e o tipo, retorna um Identificador
Identificador *cria_identificador(char *nome, TokenType tipo, char funcao) {

	Identificador *id = (Identificador *) malloc(sizeof(Identificador));

	id->tipo = tipo;
	id->nome = nome;
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


void printEscopo(Identificador);
// procura e retorna o id dentro do escopo.
// se o escopoProcura for 0, procurar apenas no escopo local
// se o escopoProcura for 1, procurar nos escopos superiores
Identificador *contem(TabSimb *escopo, char *id, char escopoProcura, char funcao) {
	if(!escopo)		// se não houver nenhuma variável no escopo
		return NULL;

	Identificador *identificador = escopo->declarados;
	while(identificador) {
		if(*id == *identificador->nome) {					// se achou o identificador na tabela de símbolos
			return identificador;
		}

		identificador = (Identificador *) identificador->proximo;			// avança para o próximo identificador
	}

	if(!escopoProcura) {		// se é para procurar somente no local e não foi encontrado
		return NULL;
	} else {
		return contem((TabSimb *) escopo->escopoSuperior, id, 1, funcao);	// procura no escopo superior
	}
}

// insere uma variável no escopo local
// se esta já estiver no escopo local, então gera um erro
// funcao diz respeito se este VAR é uma função
void insere_escopo(TabSimb *tabsimb, char *id, TokenType tipo, char funcao) {

	Identificador *novo_id = cria_identificador(id, tipo, funcao);	// cria um identificador
	novo_id->proximo = (struct Identificador *) tabsimb->declarados;// aponta para o proximo
	tabsimb->declarados = novo_id;									// insere na lista

}