#include "tabsimb.h"

unsigned char numEscopo = 0;

// cria uma lista de indices. O tamanho da lista é referente a quantidade das dimensões
// cada posição da lista é do tamanho daquela dimensão
// se um tamanho for negativo, então retorna 1 e marca na variável que esta contém erro
char getIndice(TreeNode *var, Identificador *id, char *nomeArquivo) {
	if(!var->filhos[0])					// se não houver índice
		return 0;

	char i;

	for(i = 0; var->filhos[i]; i ++);	// contando cada filho

	id->indices = (short int *) malloc((i+1)*sizeof(short int));
	id->indices[i] = -1;				// na última posição insere o -1 para saber o final dos índices

	char i2;
	char temErro = 0;	// se contém algum erro na quantidade
	for(i2 = 0; i2 < i; i2 ++) {
		id->indices[i2] = *((int *) var->filhos[i2]->token->val);

		if(id->indices[i2] < 1 || var->filhos[i2]->token->tokenval == NUM_F) {
			temErro = 1;
			erro(nomeArquivo, var->filhos[i2]->token, "Índice inválido'.", 0, 1);
		}
	}

	return temErro;	// se não houve erro, será retornado 0
}

// dado o nó VAR e o tipo, retorna um Identificador
Identificador *cria_identificador(TreeNode *var, char funcao, char *nomeArquivo, TokenRecord *token) {

	Identificador *id = (Identificador *) malloc(sizeof(Identificador));

	id->tipo = var->tipoExpressao;
	id->nome = (char *) var->token->val;
	id->funcao = funcao;
	id->iniciada = 0;
	id->utilizada = 0;
	id->declarada = 1; // mas pode ser alterada se o identificador for criado apenas para informação
	id->retornou = 0;
	id->indices = NULL;		// pode ser alterado na função getIndice
	id->proximo = NULL;
	id->token = token;

	id->erro = getIndice(var, id, nomeArquivo);
	return id;
}


// insere um escopo filho na lista de escopos inferiores
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
		char *retornou = id->retornou ? "RET" : "N-RET";
		char *indice = id->indices ? "IND" : "N-IND";

		tab(qtdTab+2);
		printf("|%s: %s;%s;%s;%s;%s;%s;%s|\n", id->nome, funcao, tipo, erro, iniciada, utilizada, retornou, indice);
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
// se a VAR tiver erro, RETORNA O TIPO DA PRIMEIRA VAR DECLARADA, OU NULL
Identificador *contem(TabSimb *escopo, char *id, char escopoProcura, char funcao) {
	if(!escopo)		// se não houver nenhuma variável no escopo
		return NULL;

	Identificador *identificador = escopo->declarados;
	Identificador *identificadorErrado = NULL;
	while(identificador) {

		// se achou o identificador na tabela de símbolos
		if(!strcmp(id, identificador->nome) && funcao == identificador->funcao) {
			if(!identificador->declarada) {	// se a VAR não foi declarada
				identificadorErrado = identificador;
				goto proximo;				// continua procurando para retornar a primera VAR declarada
			}

			return identificador;
		}

		proximo:
		identificador = (Identificador *) identificador->proximo;		// avança para o próximo identificador
	}

	if(escopoProcura && !identificadorErrado) {	// se é para procurar somente no local e não foi encontrado
		return contem((TabSimb *) escopo->escopoSuperior, id, 1, funcao);// procura no escopo superior
	} else {
		return identificadorErrado;
	}
}

// insere uma variável no escopo local
// se esta já estiver no escopo local, então gera um erro
// funcao diz respeito se este VAR é uma função
Identificador *insere_escopo(TabSimb *tabsimb, TreeNode *var, char funcao, char *nomeArquivo) {

	Identificador *novo_id = cria_identificador(var, funcao, nomeArquivo, var->token);	// cria um identificador
	novo_id->proximo = (struct Identificador *) tabsimb->declarados;		// aponta para o proximo
	tabsimb->declarados = novo_id;											// insere na lista

	return novo_id;
}