#include "tabsimb.h"

unsigned char numEscopo = 0;

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

void insereListaEscopo(TabSimb *escopoSuperior, TabSimb *escopoInferior) {
	if(!escopoSuperior)
		return;

	TabSimb *escopo = (TabSimb *) escopoSuperior->escoposInferior;	// recupera o primeiro escopo
	if(!escopo) {										// se não houver escopos inferiores
		escopoSuperior->escoposInferior = (struct TabSimb *) escopoInferior;
		return;
	}

	while(escopo->proximo) {							// enquanto tiver escopos inferiores
		escopo = (TabSimb *) escopo->proximo;			// avança
	}

	escopo->proximo = (struct TabSimb *) escopoInferior;// atribui como próximo no escopo inferior
}


TabSimb *criaTabSim(TabSimb *escopoSuperior) {

	TabSimb *tabsimb = (TabSimb *) malloc(sizeof(TabSimb));
	tabsimb->escopoSuperior = (struct TabSimb *) escopoSuperior;

	tabsimb->escoposInferior = NULL;	// sem lista de escopos inferiores
	tabsimb->declarados = NULL;			// se lista das variáveis declaradas
	tabsimb->proximo = NULL;			// não tem próximo no seu escopo irmão
	tabsimb->numEscopo = numEscopo ++;

	insereListaEscopo(escopoSuperior, tabsimb);

	return tabsimb;
}


// printa a quantidade de tab
void tab(char qtd) {
	char i;
	for(i = 0; i < qtd; i ++)
		printf(" ");
}


// printa a tabela de símbolos
void printEscopo(TabSimb *escopo, char qtdTab) {

	Identificador *id = escopo->declarados;
	TabSimb *escopoSuperior = (TabSimb *) escopo->escopoSuperior;

	tab(qtdTab);
	printf("|LOC|PAI|\n");
	tab(qtdTab);
	printf("| %d | %d | =>\n", escopo->numEscopo, escopoSuperior->numEscopo);
	for(; id;) {

		char *tipo = id->tipo == INTEIRO ? "INT" : (id->tipo == FLUTUANTE ? "FLT" : "VOID");
		char *funcao = id->funcao ? "FUNC" : "VAR";
		char *erro = id->erro ? "ERR" : "N-ERR";
		char *iniciada = id->iniciada ? "INIC" : "N-INIC";
		char *utilizada = id->utilizada ? "UTLZ" : "N-UTLZ";

		tab(qtdTab+2);
		printf("|%s: %s;%s;%s;%s;%s|\n", id->nome, funcao, tipo, erro, iniciada, utilizada);
		id = (Identificador *) id->proximo;
	}

	printf("\n");
	TabSimb *escoposInferior = (TabSimb *) escopo->escoposInferior;
	while(escoposInferior) {
		printEscopo(escoposInferior, qtdTab+2);
		escoposInferior = (TabSimb *) escoposInferior->proximo;
	}
}

// procura e retorna o id dentro do escopo.
// se o escopoProcura for 0, procurar apenas no escopo local
// se o escopoProcura for 1, procurar nos escopos superiores
Identificador *contem(TabSimb *escopo, char *id, char escopoProcura, char funcao) {
	if(!escopo)		// se não houver nenhuma variável no escopo
		return NULL;

	Identificador *identificador = escopo->declarados;
	while(identificador) {

		// se achou o identificador na tabela de símbolos
		if(!strcmp(id, identificador->nome) && funcao == identificador->funcao) {
			return identificador;
		}

		identificador = (Identificador *) identificador->proximo;		// avança para o próximo identificador
	}

	if(escopoProcura) {		// se é para procurar somente no local e não foi encontrado
		return contem((TabSimb *) escopo->escopoSuperior, id, 1, funcao);// procura no escopo superior
	} else {
		return NULL;
	}
}

// insere uma variável no escopo local
// se esta já estiver no escopo local, então gera um erro
// funcao diz respeito se este VAR é uma função
Identificador *insere_escopo(TabSimb *tabsimb, char *id, TokenType tipo, char funcao) {

	Identificador *novo_id = cria_identificador(id, tipo, funcao);	// cria um identificador
	novo_id->proximo = (struct Identificador *) tabsimb->declarados;// aponta para o proximo
	tabsimb->declarados = novo_id;									// insere na lista

	return novo_id;
}