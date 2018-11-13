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
		// insere do mesmo geito e marca como erro
		insere_escopo(escopoLocal, var, funcao, filename)->erro = 1;
	} else {	// se não tem, então insere

		insere_escopo(escopoLocal, var, funcao, filename);
	}
}

// função responsável por procurar um nó na tabela
Identificador *procura(TabSimb *escopoLocal, TreeNode *var, TokenType tipoAnterior) {

	char *id = (char *) var->token->val;
	char funcao = tipoAnterior == CHAMADA_FUNCAO ? 1 : 0;	// recupera se este é uma variavel ou uma declaracao_funcao

	// procura em todos os escopos passados
	// se não tiver
	Identificador *identificador = contem(escopoLocal, id, 1, funcao);
	if(!identificador) {
		erro(filename, var->token, "Variável não declarada neste e nem nos escopos superiores.", 0);
		// insere na tabela, para suprimir os erros que este irão causar
		identificador = insere_escopo(escopoLocal, var, funcao, filename);
		identificador->declarada = 0;	// marca que não existe
		identificador->erro = 1;
	}

	return identificador;
}


// retorna o tipo da expressão
// se for uma VAR e conter erro, RETORNA O TIPO DA PRIMERIA VAR DECLARADA
// aqui já é feita as verificações de inicialização e de uso
TokenType getTipoNo(TabSimb *escopoLocal, TreeNode *no, EBNFType tipoAnterior, EBNFType atribuicao) {
	if(!no)
		return -1;

	// se for chamada de função
	// substitui pelo nome da chamada
	if(no->bnfval == CHAMADA_FUNCAO) {
		tipoAnterior = CHAMADA_FUNCAO;
		no = no->filhos[0];
	}

	if(no->bnfval == VAR) {

		// verificar o tipo da operação atual
		// verificar se já é a inicialização
		// atribuir o uso
		Identificador *var = procura(escopoLocal, no, tipoAnterior);	// recupera a variável

		if(!var || var->erro) {		// se a var não foi encontrada
			return no->tipoExpressao;
		}

		no->tipoExpressao = var->tipo;	// atribui o tipo

		// se não é atribuição, tem que verificar se a VAR foi iniciada
		// se não foi iniciada, e não tiver erro, então gera o erro
		if (atribuicao != B_ATRIBUICAO && !var->iniciada && !var->erro && !var->funcao) {
			erro(filename, no->token, "Variável não inicializada.", 0);
			return var->tipo;
		} else if (atribuicao != B_ATRIBUICAO) {
			var->utilizada = 1;			// marca como sendo utilizada
		}

		return var->tipo;
	} else {						// se não for uma variável
		return no->tipoExpressao;	// apenas retorna o tipo
	}
}


// verifica os tipos das operações
void verificaOperacao(TabSimb *escopoLocal, TreeNode *st, EBNFType tipoAnterior) {

	switch(st->bnfval) {

		case B_ATRIBUICAO:
		case OPERADOR_SOMA:
		case OPERADOR_MULTIPLICACAO:
			1;
			// se for atribuicao, será nesta primeira variável
			TokenType tipo1 = getTipoNo(escopoLocal, st->filhos[0], tipoAnterior, st->bnfval);
			TokenType tipo2 = getTipoNo(escopoLocal, st->filhos[1], tipoAnterior, 0);	// não precisa dizer qual é a operação

			if(tipo1 == tipo2) {			// são os mesmos tipos
				st->tipoExpressao = tipo1;	// então atribui a operação
				procura(escopoLocal, st->filhos[0], tipoAnterior)->iniciada = 1;	// marca como iniciada
			} else {						// fazer cast e gerar um warning
				procura(escopoLocal, st->filhos[0], tipoAnterior)->iniciada = 1;	// marca como iniciada
				erro(filename, st->token, "Atribuição com tipos diferentes.", 0);
			}

			// marca que foi inicializada se for uma atribuição
			break;
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
		// se for um B_SE, tem que separar um bloco para o corpo do SE e outro para o corpo do SENÃO
		// se o i == 0, não pode entrar, pois está no corpo da expressao
		if((st->bnfval == B_SE && i != 0) || filho->bnfval == B_REPITA || filho->bnfval == DECLARACAO_FUNCAO) {

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

	// aqui a tabela de símbolos é construido
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

	// se não for um VAR, então é uma operação
	verificaOperacao(escopoLocal, st, tipoAnterior);
}


TabSimb *constroiTabSimb(TreeNode *st, char *nomeArquivo) {

	filename = nomeArquivo;
	TabSimb *programa = criaTabSim(NULL/*, "global\0"*/);	// escopo superior, que no caso não existe
	TabSimb *global = criaTabSim(programa);					// escopo global

	recursivo(global, st, -1);

	return global;
}