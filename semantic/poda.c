#include "poda.h"

void get_declaracao_variaveis(TreeNode *declaracao_variaveis);
TreeNode *get_expressao(TreeNode *expressao);
void get_corpo(TreeNode *corpo);
void get_indice(TreeNode *var);

void get_lista_argumentos(TreeNode *lista_argumentos) {

	if(lista_argumentos->filhos[0]->bnfval == VAZIO) {		// se a lista for vázia
		lista_argumentos->filhos[0] = NULL;					// remove o nó VAZIO
		return;
	}

	unsigned char posVar = 0;
	unsigned char posFilho = 0;

	do {

		TreeNode *argumento = lista_argumentos->filhos[posFilho];
		lista_argumentos->filhos[posVar] = get_expressao(argumento);
		posVar ++;

		posFilho += 2;

	} while(lista_argumentos->filhos[posFilho]);				// enquanto tiver o próximo filho

	for(; posFilho > posVar+1; posFilho --) {				// remove todos nós a frente do último var
		remove_filho(lista_argumentos);
	}
}

TreeNode *get_chamada_funcao(TreeNode *chamada_funcao) {

	chamada_funcao->filhos[1] = chamada_funcao->filhos[2];	// troca o '(' pela lista_argumentos

	remove_filho(chamada_funcao);	// remove o filho ')'
	remove_filho(chamada_funcao);	// remove o filho 'lista_argumentos', visto que este já foi para a esquerda

	get_lista_argumentos(chamada_funcao->filhos[1]);		// simplifica o lista_argumentos
	return chamada_funcao;
}

TreeNode *get_fator(TreeNode *fator) {

	switch(fator->filhos[0]->bnfval) {

		case VAR:
			1;
			TreeNode *var = fator->filhos[0];
			get_indice(var);				// simplifica os índices
			var = var->filhos[0];			// remove o nó var e troca pelo ID
			var->bnfval = VAR;
			return var;

		case CHAMADA_FUNCAO:
			return get_chamada_funcao(fator->filhos[0]);

		case NUMERO:
			1;		// apenas porque não pode ter declaração de variável como primeiro comando no case
			TreeNode *numero = fator->filhos[0]->filhos[0];
			numero->bnfval = NUMERO;
			return numero;

		default:	// caso onde o fator leva à '(' expressao ')'
			return get_expressao(fator->filhos[1]);
	}
}

TreeNode *get_expressao_unaria(TreeNode *exp_unaria) {

	if(!exp_unaria->filhos[1]) {			// se não tiver filho na posição 1, então é fator
		return get_fator(exp_unaria->filhos[0]);
	}

	TreeNode *operador = exp_unaria->filhos[0]->filhos[0];
	operador->bnfval = exp_unaria->filhos[0]->bnfval;

	exp_unaria->filhos[0] = operador;
	exp_unaria->filhos[1] = get_fator(exp_unaria->filhos[1]);
	return exp_unaria;
}

TreeNode *get_expressao_multiplicativa(TreeNode *exp_multiplicativa) {

	if(!exp_multiplicativa->filhos[1]) {			// se não for um operador_multiplicacao, então é uma expressao_unaria
		return get_expressao_unaria(exp_multiplicativa->filhos[0]);
	}

	unsigned char i = 0;
	TreeNode *op_multiplicacao;
	TreeNode *left = exp_multiplicativa->filhos[i];

	i ++;
	do {
		op_multiplicacao = exp_multiplicativa->filhos[i];		// trocando o operador_multiplicacao pelo '*'
		op_multiplicacao = op_multiplicacao->filhos[0];
		op_multiplicacao->bnfval = OPERADOR_MULTIPLICACAO;

		if(left->bnfval == OPERADOR_MULTIPLICACAO)		// se já é um operador_multiplicacao
			insere_filho(op_multiplicacao, left);
		else
			insere_filho(op_multiplicacao, get_expressao_unaria(left));

		insere_filho(op_multiplicacao, get_expressao_unaria(exp_multiplicativa->filhos[++ i]));;

		left = op_multiplicacao;
	} while (exp_multiplicativa->filhos[++ i]);

	return op_multiplicacao;
}

TreeNode *get_expressao_aditiva(TreeNode *exp_aditiva) {

	if(!exp_aditiva->filhos[1]) {			// se não for um operador_soma, então é uma expressao_multiplicativa
		return get_expressao_multiplicativa(exp_aditiva->filhos[0]);
	}

	unsigned char i = 0;
	TreeNode *op_soma;
	TreeNode *left = exp_aditiva->filhos[i];

	i ++;
	do {
		op_soma = exp_aditiva->filhos[i];		// trocando o operador_soma pelo '+'
		op_soma = op_soma->filhos[0];
		op_soma->bnfval = OPERADOR_SOMA;

		if(left->bnfval == OPERADOR_SOMA)		// se já é um operador_relacional
			insere_filho(op_soma, left);
		else
			insere_filho(op_soma, get_expressao_multiplicativa(left));

		insere_filho(op_soma, get_expressao_multiplicativa(exp_aditiva->filhos[++ i]));;

		left = op_soma;
	} while (exp_aditiva->filhos[++ i]);

	return op_soma;
}

TreeNode *get_expressao_simples(TreeNode *exp_simples) {

	if(!exp_simples->filhos[1]) {			// se não for um operador_relacional, então é uma expressao_aditiva
		return get_expressao_aditiva(exp_simples->filhos[0]);
	}

	unsigned char i = 0;
	TreeNode *op_relacional;
	TreeNode *left = exp_simples->filhos[i];

	i ++;
	do {
		op_relacional = exp_simples->filhos[i];
		op_relacional = op_relacional->filhos[0];
		op_relacional->bnfval = OPERADOR_RELACIONAL;

		if(left->bnfval == OPERADOR_RELACIONAL)		// se já é um operador_relacional
			insere_filho(op_relacional, left);
		else
			insere_filho(op_relacional, get_expressao_aditiva(left));

		insere_filho(op_relacional, get_expressao_aditiva(exp_simples->filhos[++ i]));;

		left = op_relacional;
	} while (exp_simples->filhos[++ i]);

	return op_relacional;
}

TreeNode *get_expressao_logica(TreeNode *exp_logico) {

	if(!exp_logico->filhos[1]) {	// se não for um operador_logico, então é uma expressao_simples
		return get_expressao_simples(exp_logico->filhos[0]);
	}

	unsigned char i = 0;
	TreeNode *op_logico;
	TreeNode *left = exp_logico->filhos[i];

	i ++;
	do {

		op_logico = exp_logico->filhos[i];				// recupera o nó OU ou E
		op_logico = op_logico->filhos[0];				// troca o nó OPERADOR_LOGICO pelo token || ou &&
		op_logico->bnfval = OPERADOR_LOGICO;			// adiciona o tipo do operador

		// como foi alterado a regra da expressao_logica, tem que executar uma chamada recursiva para os filhos
		if(left->bnfval == EXPRESSAO_LOGICA)			// se for EXPRESSAO_LOGICA, então tem um E na esquerda
			insere_filho(op_logico, get_expressao_logica(left));// adiciona a EXPRESSAO_SIMPLES como filho do token
		else
			insere_filho(op_logico, get_expressao_simples(left));

		i ++;
		if(exp_logico->filhos[i]->bnfval == EXPRESSAO_LOGICA)
			insere_filho(op_logico, get_expressao_logica(exp_logico->filhos[i]));
		else
			insere_filho(op_logico, get_expressao_simples(exp_logico->filhos[i]));

		left = op_logico;
	} while (exp_logico->filhos[++ i]);

	return op_logico;
}

TreeNode *get_atribuicao(TreeNode *atribuicao) {

	TreeNode *atr = atribuicao->filhos[1];		// recuperando o token atribuição
	atr->bnfval = B_ATRIBUICAO;

	get_indice(atribuicao->filhos[0]);			// simplifica o var

	insere_filho(atr, atribuicao->filhos[0]->filhos[0]);	// adiciona o ID como primeiro filho
	insere_filho(atr, get_expressao(atribuicao->filhos[2]));// adiciona a expressao como segundo filho

	return atr;
}

TreeNode *get_expressao(TreeNode *expressao) {

	TreeNode *filho = expressao->filhos[0];

	switch(filho->bnfval) {

		case EXPRESSAO_LOGICA:
			return get_expressao_logica(filho);

		case B_ATRIBUICAO:
			return get_atribuicao(filho);
	}
}

void get_retorna(TreeNode *retorna) {

	retorna->filhos[0] = retorna->filhos[2];
	retorna->filhos[0] = get_expressao(retorna->filhos[0]);

	remove_filho(retorna);	// remove o ')'
	remove_filho(retorna);	// remove expressao
	remove_filho(retorna);	// remove o '('
}

void get_escreva(TreeNode *escreva) {

	escreva->filhos[0] = escreva->filhos[2];
	escreva->filhos[0] = get_expressao(escreva->filhos[0]);

	remove_filho(escreva);	// remove o ')'
	remove_filho(escreva);	// remove expressao
	remove_filho(escreva);	// remove '('
}

void get_se(TreeNode *se) {

	se->filhos[0] = se->filhos[1];	// remove o nó SE e substitui pelo expressao
	se->filhos[1] = se->filhos[3];	// remove o nó expressao e substitui pelo corpo

	se->filhos[0] = get_expressao(se->filhos[0]);	// simplifica a expressao
	get_corpo(se->filhos[1]);		// simplifica o corpo

	unsigned char comeco;			// começa a apagar os filhos dessa posição
	unsigned char fim;				// apaga os filhos até esta posição

	TokenRecord *token = se->filhos[4]->token;	// será o token FIM ou SENAO

	if(token->tokenval == FIM) {		// nao tem o corpo do SENAO
		comeco = 2;
		fim = 5;
	} else {
		se->filhos[2] = se->filhos[5];	// coloca o corpo do SENÃO
		get_corpo(se->filhos[2]);		// simplifica o corpo
		comeco = 3;
		fim = 7;
	}

	for(; comeco < fim; comeco ++) {	// remove os filhos a frente
		remove_filho(se);
	}
}

void get_repita(TreeNode *repita) {

	repita->filhos[0] = repita->filhos[1];	// remove o repita e substitui pelo corpo
	repita->filhos[1] = repita->filhos[3];	// remove o corpo e adiciona o expressao

	repita->filhos[1] = get_expressao(repita->filhos[1]);	// simplifica a expressao
	get_corpo(repita->filhos[0]);							// simplifica o corpo

	remove_filho(repita);	// remove o ate
	remove_filho(repita);	// remove expressao

	printArvoreT(repita, "oi");
}

void get_leia(TreeNode *leia) {

	leia->filhos[0] = leia->filhos[2];				// substitui o LEIA pelo var
	get_indice(leia->filhos[0]);					// se houver índice, organiza
	leia->filhos[0] = leia->filhos[0]->filhos[0];	// remove o nó intermediário VAR

	remove_filho(leia);		// remove o filho ')'
	remove_filho(leia);		// remove o filho 'var'
	remove_filho(leia);		// remove o filho '('
}

TreeNode *get_acao(TreeNode *acao) {

	acao = acao->filhos[0];		// remove o nó acao

	switch(acao->bnfval) {

		case EXPRESSAO:
			return get_expressao(acao);

		case DECLARACAO_VARIAVEIS:
			get_declaracao_variaveis(acao);
			break;

		case B_SE:
			get_se(acao);
			break;

		case B_REPITA:
			get_repita(acao);
			break;

		case B_LEIA:
			get_leia(acao);
			break;

		case B_ESCREVA:
			get_escreva(acao);
			break;

		case B_RETORNA:
			get_retorna(acao);
			break;
	}

	return acao;
}

void get_corpo(TreeNode *corpo) {

	if(corpo->filhos[0]->bnfval == VAZIO) {
		corpo->filhos[0] = NULL;
		return;
	}

	unsigned short int i = 0;
	while(corpo->filhos[i]) {					// enquanto houver acoes

		TreeNode *acao = corpo->filhos[i];
		corpo->filhos[i] = get_acao(acao);		// simplifica acao

		i ++;
	}
}

// remove o índice do var e adiciona a expressão como filho do ID
void get_indice(TreeNode *var) {

	if(!var->filhos[1])			// se não tiver índice
		return;

	TreeNode *id = var->filhos[0];		// recupera o id
	TreeNode *indice = var->filhos[1];	// recupera o nó indice
	unsigned char i = 0;

	while(indice->filhos[i]) {
		insere_filho(id, get_expressao(indice->filhos[i+1]));
		i += 3;							// pula o ']' e '['
	}
}

void get_lista_variaveis(TreeNode *lista_variaveis) {

	unsigned char posVar = 0;
	unsigned char posFilho = 0;

	do {

		TreeNode *var = lista_variaveis->filhos[posFilho];
		get_indice(var);									// se houver índice, simplifica
		lista_variaveis->filhos[posVar] = var->filhos[0];	// removendo o nó 'var' e pegando o 'id'
		posVar ++;

		posFilho += 2;

	} while(lista_variaveis->filhos[posFilho]);				// enquanto tiver o próximo filho

	for(; posFilho > posVar+1; posFilho --) {				// remove todos nós a frente do último var
		remove_filho(lista_variaveis);
	}
}

void get_declaracao_variaveis(TreeNode *declaracao_variaveis) {

	TreeNode *tipo = declaracao_variaveis->filhos[0];	// recuperando o nó tipo
	declaracao_variaveis->filhos[0] = tipo->filhos[0];	// remove o nó intermediário TIPO

	TreeNode *lista_variaveis = declaracao_variaveis->filhos[2];	// recuperando nó lista_variaveis
	declaracao_variaveis->filhos[1] = lista_variaveis;	// colocando a lista_variaveis no lugar de ':', assim remove o nó ':''

	get_lista_variaveis(lista_variaveis);				// simplifica a lista de variaveis

	remove_filho(declaracao_variaveis);					// apenas coloca NULL no último filho
}

void get_parametro(TreeNode *parametro) {

	parametro->filhos[0] = parametro->filhos[0]->filhos[0];	// remove o nó TIPO e adiciona o INTEIRO|FLUTUANTE

	unsigned char i = 1;
	// se tiver vários indices, tem qu deslocar para a esquerda
	do {
		parametro->filhos[i] = parametro->filhos[i+1];			// remove o ':' e adiciona o próximo
		i ++;
	} while(parametro->filhos[i+1]);

	remove_filho(parametro);
}

void get_lista_parametros(TreeNode *lista_parametros) {

	if(lista_parametros->filhos[0]->bnfval == VAZIO) {		// se a lista for vázia
		lista_parametros->filhos[0] = NULL;					// remove o nó VAZIO
		return;
	}

	unsigned char posVar = 0;
	unsigned char posFilho = 0;

	do {

		TreeNode *parametro = lista_parametros->filhos[posFilho];
		get_parametro(parametro);							// simplifica o parâmetro
		lista_parametros->filhos[posVar] = parametro;		// removendo o nó ',' e pegando o 'parametro'
		posVar ++;

		posFilho += 2;

	} while(lista_parametros->filhos[posFilho]);				// enquanto tiver o próximo filho

	for(; posFilho > posVar+1; posFilho --) {				// remove todos nós a frente do último var
		remove_filho(lista_parametros);
	}
}

void get_declaracao_funcao(TreeNode *declaracao_funcao) {

	unsigned char pos = 0;
	TreeNode *tipo = declaracao_funcao->filhos[0];
	// se a declaração de função não tiver tipo, então ignora
	if(tipo->bnfval == TIPO) {
		declaracao_funcao->filhos[0] = tipo->filhos[0];		// remove o nó tipo e adiciona o INTEIRO | FLUTUANTE
		pos = 1;
	} else {
		pos = 0;
	}

	TreeNode *cabecalho = declaracao_funcao->filhos[pos];
	declaracao_funcao->filhos[pos] = cabecalho->filhos[0];		// remove o nó cabecalho e adiciona o nó ID
	declaracao_funcao->filhos[pos+1] = cabecalho->filhos[2];	// insere a lista_parametros
	declaracao_funcao->filhos[pos+2] = cabecalho->filhos[4];	// insere o corpo

	get_corpo(cabecalho->filhos[4]);							// simplifica o corpo

	get_lista_parametros(declaracao_funcao->filhos[pos+1]);		// simplifica a lista_parametros
}

void get_declaracao(TreeNode *declaracao) {

	TreeNode *filho = declaracao->filhos[0];

	switch(filho->bnfval) {

		case DECLARACAO_VARIAVEIS:
			get_declaracao_variaveis(filho);
			break;

		case INICIALIZACAO_VARIAVEIS:
			1;
			TreeNode *atribuicao = get_atribuicao(filho->filhos[0]);
			declaracao->filhos[0] = atribuicao;
			break;

		case DECLARACAO_FUNCAO:
			get_declaracao_funcao(filho);
			break;
	}
}

// sempre considerando que a árvore está
// totalmente de acordo com a EBNF
TreeNode *podar(TreeNode *ast) {

	TreeNode *lista_declaracao = ast->filhos[0];
	remove_filho(ast);			// remove a lista_declaracao

	unsigned char i;
	for(i = 0; lista_declaracao->filhos[i]; i ++) {
		TreeNode *declaracao = lista_declaracao->filhos[i];

		get_declaracao(declaracao);
		lista_declaracao->filhos[i] = declaracao->filhos[0];	// removendo o nó declaracao
		insere_filho(ast, lista_declaracao->filhos[i]);
	}

	return ast;
}