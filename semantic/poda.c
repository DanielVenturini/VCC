#include "poda.h"













TreeNode *get_atribuicao(TreeNode *atribuicao) {

}

TreeNode *get_expressao(TreeNode *expressao) {
	TreeNode *filho = expressao->filhos[0];

	switch(filho->bnfval) {

		case EXPRESSAO_LOGICA:
			return expressao->filhos[0]->filhos[0]->filhos[0]->filhos[0]->filhos[0]->filhos[0]->filhos[0]->filhos[0];
			break;

		case B_ATRIBUICAO:
			return get_atribuicao(filho);
			break;
	}
}

void get_lista_variaveis(TreeNode *lista_variaveis) {
	unsigned char posVar = 0;
	unsigned char posFilho = 0;

	do {

		TreeNode *var = lista_variaveis->filhos[posFilho];
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

void get_inicializacao_variaveis(TreeNode *inicializacao_variaveis) {

	TreeNode *atribuicao = inicializacao_variaveis->filhos[0];

	remove_filho(inicializacao_variaveis);					// remove o atribuicao

	TreeNode *var = atribuicao->filhos[0];
	insere_filho(inicializacao_variaveis, var->filhos[0]);	// adiciona o nó 'var' que vai ser atribuido
	// coloca a expressao como segundo filho para atribuir
	insere_filho(inicializacao_variaveis, get_expressao(atribuicao->filhos[2]));
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
	if(lista_parametros->filhos[0]->bnfval == VAZIO)		// se a lista for vázia
		return;

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

	get_lista_parametros(declaracao_funcao->filhos[pos+1]);		// simplifica a lista_parametros
}

void get_declaracao(TreeNode *declaracao) {

	TreeNode *filho = declaracao->filhos[0];

	switch(filho->bnfval) {

		case DECLARACAO_VARIAVEIS:
			get_declaracao_variaveis(filho);
			break;

		case INICIALIZACAO_VARIAVEIS:
			get_inicializacao_variaveis(filho);
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