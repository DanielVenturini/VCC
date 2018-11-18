#include "semantico.h"

char *filename;

Identificador *procura(TabSimb *, TreeNode *, TokenType);
/**********************************************\
|             OPERAÇÕES AUXILIARES             |
\**********************************************/
// função para adaptação da função procura
Identificador *procura_somente_id(TabSimb *escopoLocal, char *id, TokenRecord *token, TokenType tipoAnterior) {
	TreeNode *no = novo_node(token, ID);
	no->token->val = (void *) id;

	return procura(escopoLocal, no, tipoAnterior);
}

// função responsável por procurar e inserir um nó na tabela de símbolos
Identificador *insere(TabSimb *escopoLocal, TreeNode *var, TokenType tipoAnterior) {

	char *id = (char *) var->token->val;
	char funcao = tipoAnterior == CHAMADA_FUNCAO ? 1 : tipoAnterior == DECLARACAO_FUNCAO ? 1 : 0;	// recupera se este é uma variavel ou uma declaracao_funcao

	// deve procurar apenas no escopo local
	Identificador *identificador = contem(escopoLocal, id, 0, funcao);
	if(identificador && !identificador->erro && !identificador->funcao) {
		erro(filename, var->token, "Redeclaração de variável no mesmo escopo.", 0, 1);
		// insere do mesmo geito e marca como erro
		insere_escopo(escopoLocal, var, funcao, filename)->erro = 1;
		return NULL;
	} else {	// se não tem, então insere

		return insere_escopo(escopoLocal, var, funcao, filename);
	}
}

// função responsável por procurar um nó na tabela
Identificador *procura(TabSimb *escopoLocal, TreeNode *var, TokenType tipoAnterior) {

	char *id = (char *) var->token->val;
	// recupera se este é uma variavel ou uma funcao
	char funcao = tipoAnterior == CHAMADA_FUNCAO ? 1 : tipoAnterior == DECLARACAO_FUNCAO ? 1 : 0;

	// procura em todos os escopos passados
	// se não tiver
	Identificador *identificador = contem(escopoLocal, id, 1, funcao);
	if(!identificador) {

		erro(filename, var->token, "Variável não declarada neste e nem nos escopos superiores.", 0, 1);
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
			erro(filename, no->token, "Variável não inicializada.", 0, 1);
			return var->tipo;
		} else if (atribuicao != B_ATRIBUICAO) {
			var->utilizada = 1;			// marca como sendo utilizada
		}

		return var->tipo;
	} else {						// se não for uma variável
		return no->tipoExpressao;	// apenas retorna o tipo
	}
}

/**********************************************\
|             OPERAÇÕES SEMÂNTICAS             |
\**********************************************/
// verifica se a variável - se for uma VAR - na tabela de símbolos realmente tem índice
// se tiver, verifica se o índice que está tentando acessar realmente está no limite
void verificaIndice(TabSimb *escopoLocal, TreeNode *st) {
	if(st->bnfval != VAR)			// se não for variável
		return;

	Identificador *id = procura(escopoLocal, st, -1);	// não precisa saber o tipoAnterior
	if(!id->indices && !st->filhos[0])					// não tem índice e não está sendo acessada com índices
		return;

	if(id->erro)					// se já teve um erro, não processa
		return;

	// se tem índices e não está sendo especificado: id[10] id = 20 -> id[pos] = 20
	if(id->indices && !st->filhos[0]) {
		erro(filename, st->token, "Utilizando variável sem especificar a dimensão.", 0, 1);
		id->erro = 1;
		return;
	}

	if(!id->indices) {				// se não houver índice
		if(!id->erro)				// se não houver erro, printa a mensagem; senão, suprime
			erro(filename, st->token, "Variável não foi declarada com índices.", 0, 1);

		id->erro = 1;
		return;
	}

	// se chegou até aqui, então tem índice
	// então percorre todos os índices do VAR e verifica se confere com o da tabela de símbolos
	char i;
	for(i = 0; st->filhos[i] && id->indices[i] != -1; i ++) {

		// verifica se o índice declarado é inteiro
		if(st->filhos[i]->tipoExpressao != INTEIRO) {
			erro(filename, st->filhos[i]->token, "Índice não inteiro.", 0, 1);
			id->erro = 1;
			return;
		}

		// se não é um número, e é uma VAR, então verifica se contém na tabela de símbolos
		if(st->filhos[i]->bnfval == VAR) {
			Identificador *var = procura(escopoLocal, st->filhos[i], VAR);
			if(var->erro)
				return;

			if(!var->iniciada) {
				erro(filename, st->filhos[i]->token, "Variável não inicializada.", 0, 1);
				return;
			}

			continue;
		}

		// não é um var, nem número, mas é inteiro, então ignora
		if(st->filhos[i]->bnfval != NUMERO) {
			continue;
		}

		int *indiceAcessado = (int *) st->filhos[i]->token->val;
		if(*indiceAcessado < 0) {
			erro(filename, st->filhos[i]->token, "Índice inválido.", 0, 1);
			id->erro = 1;
			return;
		}

		// verifica se o indice acessado é maior que o tamanho máximo
		int indiceDeclarado = id->indices[i];
		if(*indiceAcessado >= indiceDeclarado) {
			erro(filename, st->filhos[i]->token, "Índice maior que o declarado.", 0, 1);
			id->erro = 1;
			return;
		}
	}

	// se ainda tiver st->filhos[i] então
	// está tentando acessar uma dimensão que não existe
	if(st->filhos[i]) {
		erro(filename, st->token, "Variável não possúi todas estas dimensões.", 0, 1);
		id->erro = 1;
		return;
	}
}


// atribui o tipo da variável na VAR
void operacaoVar(TabSimb *escopoLocal, TreeNode *st, EBNFType tipoAnterior, char *nomeFuncao) {

	Identificador *id;
	TokenType tipo = -1;

	// se for um desses tipos, então adiciona na tabela de símbolos
	if(tipoAnterior == LISTA_VARIAVEIS || tipoAnterior == DECLARACAO_FUNCAO || tipoAnterior == PARAMETRO) {
		// insere a VAR
		id = insere(escopoLocal, st, tipoAnterior);

		// para evitar printar duas vezes que uma função não está sendo utilizada
		// no seu escopo local, marca que ela está sendo utilizada
		if(!strcmp((char *) st->token->val, nomeFuncao) && tipoAnterior == DECLARACAO_FUNCAO) {
			// eu acho q é declarada e não utilizada
			id->utilizada = 1;
		}

		tipo = id->tipo;

	} else {	// então procura na tabela de símbolos, pois está utilizando normalmente
		id = procura(escopoLocal, st, tipoAnterior);

		if(!id->erro) {
			verificaIndice(escopoLocal, st);	// verifica o índice - se houver - da variável que está recebendo
			id->utilizada = 1;
		}

		tipo = id->tipo;
	}

	st->tipoExpressao = tipo;
}


// processa as operações de atribuição, soma e multiplicacao
void operacoesTernarias(TabSimb *escopoLocal, TreeNode *st, EBNFType tipoAnterior, char *nomeFuncao) {

	TokenType tipo1 = getTipoNo(escopoLocal, st->filhos[0], tipoAnterior, st->bnfval);
	TokenType tipo2 = getTipoNo(escopoLocal, st->filhos[1], tipoAnterior, 0);	// não precisa dizer qual é a operação

	// o índice já foi verificado

	if(tipo1 == tipo2) {					// são os mesmos tipos

		st->tipoExpressao = tipo1;			// então atribui a operação
		if(st->filhos[0]->bnfval == NUMERO)	// se o número estiver na esquerda, então simplesmente retorna
			return;

		Identificador *id = procura(escopoLocal, st->filhos[0], tipoAnterior);
		id->utilizada = 1;

		if(st->bnfval == B_ATRIBUICAO)
			id->iniciada = 1;

		return;

	// se os dois filhos não for número
	}

	st->tipoExpressao = INTEIRO;			// faz casting para inteiro

	if(st->filhos[0]->bnfval != NUMERO && st->filhos[1]->bnfval != NUMERO){

		Identificador *id1 = procura(escopoLocal, st->filhos[0], tipoAnterior);
		id1->utilizada = 1;

		Identificador *id2 = procura(escopoLocal, st->filhos[1], tipoAnterior);
		id2->utilizada = 1;
		if(id1->erro || id2->erro)
			return;

		erro(filename, st->token, "Operação com tipos diferentes.", 0, 0);

	// um dos dois é NUMERO
	} else {
		erro(filename, st->token, "Operação com tipos diferentes.", 0, 0);
	}
}

// processa as operações de retorno
void operacaoRetorna(TabSimb *escopoLocal, TreeNode *st, EBNFType tipoAnterior, char *nomeFuncao) {

	if(tipoAnterior == DECLARACAO_FUNCAO)
		return;

	// recupera da tabela de símbolos a função
	TokenType tipo = st->filhos[0]->tipoExpressao;
	Identificador *funcao = procura_somente_id(escopoLocal, nomeFuncao, st->filhos[0]->token, CHAMADA_FUNCAO);

	funcao->retornou = 1;
	if(funcao->tipo == tipo)
		return;

	erro(filename, st->filhos[0]->token, "Função retornando valor com tipo errado.", 0, 1);
}

// verifica os tipos das operações
void verificaOperacao(TabSimb *escopoLocal, TreeNode *st, EBNFType tipoAnterior, char *nomeFuncao) {

	switch(st->bnfval) {

		case VAR:
			// apenas atribui o tipoExpressao a variavel
			operacaoVar(escopoLocal, st, tipoAnterior, nomeFuncao);
			break;

		case B_ATRIBUICAO:
		case OPERADOR_SOMA:
		case OPERADOR_MULTIPLICACAO:
			operacoesTernarias(escopoLocal, st, tipoAnterior, nomeFuncao);
			break;

		case B_RETORNA:
			operacaoRetorna(escopoLocal, st, tipoAnterior, nomeFuncao);
			break;

		case CHAMADA_FUNCAO:
			break;
	}
}


// tipoAnterior é quem é o pai deste nó st
// se o nó for um VAR e  o tipoAnterior for lista_variaveis, declaracao_funcao, parametro
// nomeFuncao é o nome da funçao que está, para saber o tipo de retorno
void recursivo(TabSimb *escopoLocal, TreeNode *st, EBNFType tipoAnterior, char *nomeFuncao) {
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
				recursivo(escopoLocal, var, filho->bnfval, nomeFuncao);	// adiciona o ID no escopo global
				nomeFuncao = (char *) var->token->val;
			}

			// cria um novo escopo
			escopoInferior = criaTabSim(escopoLocal);
			recursivo(escopoInferior, filho, st->bnfval, nomeFuncao);	// utiliza o novo escopo
		} else {
			recursivo(escopoLocal, filho, st->bnfval, nomeFuncao);		// utiliza o escopo atual
		}
	}

	// se não for um VAR, então é uma operação
	verificaOperacao(escopoLocal, st, tipoAnterior, nomeFuncao);
}


/**********************************************\
|           OPERAÇÕES PÓS-SEMANTICAS           |
\**********************************************/
// verifica se contém variável declarada não usada
void verificaNaoUtilizadas(TabSimb *local) {

	Identificador *id = local->declarados;

	for(; id;) {
		// se a VAR não está sendo utilizada
		// mas tem que ter sido declarada implicitamente
		// também não pode ser a função principal
		if(!id->utilizada && id->declarada && !id->erro && !(!strcmp(id->nome, "principal") && id->funcao)) {
			erro(filename, id->token, "Variável declarada e não utilizada.", 0, 0);
		}

		id = (Identificador *) id->proximo;
	}

	TabSimb *escoposInferior = (TabSimb *) local->escoposInferior;
	while(escoposInferior) {				// procura nos escopos inferiores
		verificaNaoUtilizadas(escoposInferior);
		escoposInferior = (TabSimb *) escoposInferior->proximo;
	}
}


// verifica se a função principal foi declarada
// verifica direto na árvore
void verificaPrincipal(TreeNode *st) {
	unsigned char i;
	for(i = 0; st->filhos[i]; i ++) {
		if(st->filhos[i]->bnfval != DECLARACAO_FUNCAO){
			continue;
		}

		TreeNode *declaracao_funcao = st->filhos[i];	// recupera a declaração de função
		TreeNode *var = declaracao_funcao->filhos[1]->bnfval == VAR ? declaracao_funcao->filhos[1] : declaracao_funcao->filhos[0];

		char *nomeFuncao = (char *) var->token->val;
		if(!strcmp(nomeFuncao, "principal")) {		// se achou a função princpal
			return;									// retorna sem gerar erro
		}
	}

	erro(filename, NULL, "Função principal() não declarada.", 0, 1);
}


// verifica quais foram as funções que não retornaram valor e se deveriam retornar
void verificaRetornos(TabSimb *escopo) {

	// a informação de retorno das funções só está no escopo mais interno, ou seja, o da própria função
	TabSimb *escoposInferior = (TabSimb *) escopo->escoposInferior;
	while(escoposInferior) {				// procura nos escopos inferiores

		// para cada um dos escopos inteirnos
		Identificador *id = escoposInferior->declarados;
		for(; id;) {
			// se é uma função
			// se o tipo é não vazio
			// se não está retornando
			if(id->funcao && id->tipo && !id->retornou) {
				erro(filename, id->token, "Função deveria retornar algum valor, mas retorna vazio.", 0, 1);
			}

			id = (Identificador *) id->proximo;
		}

		escoposInferior = (TabSimb *) escoposInferior->proximo;
	}
}

TabSimb *constroiTabSimb(TreeNode *st, char *nomeArquivo) {

	filename = nomeArquivo;
	TabSimb *programa = criaTabSim(NULL/*, "global\0"*/);	// escopo superior, que no caso não existe
	TabSimb *global = criaTabSim(programa);					// escopo global

	recursivo(global, st, -1, "global");

	/*printf("FIM DA ANÁLISE. AGORA É O PÓS-SEMANTICA\n");*/
	verificaNaoUtilizadas(global);
	verificaPrincipal(st);
	verificaRetornos(global);

	return global;
}