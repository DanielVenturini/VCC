#include "parse.h"
#include "../lexical/varredura.h"

// apenas declarando para não ter problemas de função a cima que não reconhece as abaixos
TreeNode *expressao_multiplicativa();
TreeNode *inicializacao_variaveis();
TreeNode *operador_multiplicacao();
TreeNode *declaracao_variaveis();
TreeNode *operador_relacional();
TreeNode *expressao_aditiva();
TreeNode *expressao_simples();
TreeNode *expressao_logica();
TreeNode *declaracao_funcao();
TreeNode *lista_parametros();
TreeNode *expressao_unaria();
TreeNode *lista_argumentos();
TreeNode *lista_variaveis();
TreeNode *operador_logico();
TreeNode *chamada_funcao();
TreeNode *operador_soma();
TreeNode *declaracao();
TreeNode *atribuicao();
TreeNode *cabecalho();
TreeNode *parametro();
TreeNode *expressao();
TreeNode *escreva();
TreeNode *retorna();
TreeNode *repita();
TreeNode *indice();
TreeNode *numero();
TreeNode *corpo();
TreeNode *fator();
TreeNode *leia();
TreeNode *tipo();
TreeNode *acao();
TreeNode *var();
TreeNode *se();

// esta função recupera o próximo token
// porém não substitui pelo token atual, apenas recupera e retorna
TokenRecord *verProximo() {
	if(tokenProximo)			// se o próximo já estiver algo
		return tokenProximo;	// retorna o token que é atualmente o próximo

	tokenProximo = getToken();	// recupera o próximo token
	return tokenProximo;		// retorna o token próximo
}

// esta função retorna token a token
// retorna o tokenAtual
// se o próximo existir, troca em tokenAtual
TokenRecord *proximo() {

	if(tokenProximo) {				// se já tiver um token de próximo
		tokenAtual = tokenProximo;	// troca os tokens
		tokenProximo = NULL;
	} else {
		tokenAtual = getToken();	// recupera o próximo token
	}

	return tokenAtual;
}

// retorna o tokenAtual
TokenRecord *atual(){
	return tokenAtual;
}

// lista_argumentos "," expressao | expressao | vazio
TreeNode *lista_argumentos() {

}

// ID "(" lista_argumentos ")"
TreeNode *chamada_funcao() {


}
// NUM_INTEIRO | NUM_PONTO_FLUTUANTE | NUM_NOTACAO_CIENTIFICA
TreeNode *numero() {
	switch(tokenAtual->tokenval){
		case NUM_I:
		case NUM_F:
			return novo_node(tokenAtual);

		default:
			printf("Err no número.\n");
	}

	return NULL;
}

// "(" expressao ")" | var | chamada_funcao | numero
TreeNode *fator() {
	switch(tokenAtual->tokenval){
		case ABRE_PARENTESES:								// "(" expressao ")"
			0;
			TreeNode *exprss = expressao();					// recupera a expressão

			if(tokenAtual->tokenval == FECHA_PARENTESES){	// verifica o fecha parenteses
				return exprss;
			} else {
				printf("Eeeeeeerou.\n");
			}

			break;

		case ID:
			tokenAtual = getToken();
			if(tokenAtual->tokenval == ABRE_PARENTESES)
				return chamada_funcao();
	}

}

// "*" | "/"
TreeNode *operador_multiplicacao() {
	switch(tokenAtual->tokenval){
		case MULTIPLICACAO:
		case DIVISAO:
			return novo_node(tokenAtual);

		default:
			printf("Erro no operador multiplicacao.\n");
			break;
	}

	return NULL;
}

// "&&" | "||"
TreeNode *operador_logico() {
	switch(tokenAtual->tokenval){
		case OU_LOGICO:
		case E_LOGICO:
			return novo_node(tokenAtual);

		default:
			printf("Err no operador lógico.\n");
	}

	return NULL;
}

// "+" | "-"
TreeNode *operador_soma() {

	switch(tokenAtual->tokenval){
		case SOMA:
		case SUBTRACAO:
			return novo_node(tokenAtual);

		default:
			printf("Err no operador lógico.\n");
	}

	return NULL;
}

// "<" | ">" | "=" | "<>" "<=" | ">="
TreeNode *operador_relacional() {

	//tokenAtual = getToken();// lê o próximo token

	switch(tokenAtual->tokenval) {

		case MENOR:
		case MAIOR:
		case IGUALDADE:
		case DIFERENTE:
		case MENOR_IGUAL:
		case MAIOR_IGUAL:
			return novo_node(tokenAtual);

		default:			// marca não reconhecida
			printf("Syntax error at line %d: %s.\n", tokenAtual->numline, (char *)tokenAtual->val);
	}

}

// fator | operador_soma fator | oeprador_negacao fator
TreeNode *expressao_unaria() {

	switch(tokenAtual->tokenval){
		case ABRE_PARENTESES:
			return fator();
	}

	return NULL;
}

// expressao_unaria | expressao_multiplicativa operador_multiplicacao expressao_unaria
TreeNode *expressao_multiplicativa() {

	switch(tokenAtual->tokenval){
		case ABRE_PARENTESES:
			return expressao_unaria();
	}

	return NULL;
}

// expressao_multiplicativa | expressao_aditiva operador_soma expressao_multiplicativa
TreeNode *expressao_aditiva() {

	switch(tokenAtual->tokenval){
		case ABRE_PARENTESES:
			return expressao_multiplicativa();
	}

	return NULL;
}

// expressao_aditiva | expressao_simples operador_relacional expressao_aditiva
TreeNode *expressao_simples() {

	switch(tokenAtual->tokenval){
		case ABRE_PARENTESES:
			return expressao_aditiva();
	}

	return NULL;
}

// expressao_simples | expressao_logica operador_logico expressao_simples
TreeNode *expressao_logica() {

}

// expressao_logica | atribuicao
TreeNode *expressao() {

}

// RETORNA "(" expressao ")"
TreeNode *retorna() {

}

// ESCREVA "(" expressao ")"
TreeNode *escreva() {


}

// LEIA "(" var ")"
TreeNode *leia() {

}

// var ":=" expressao
TreeNode *atribuicao() {

	//TreeNode *var = var();		// recupera o id
	tokenAtual = getToken();

}

// REPITA corpo ATE expressao
TreeNode *repita() {


}

// SE expressao ENTAO corpo FIM | SE expressao ENTAO corpo SENAO corpo FIM
TreeNode *se() {

}

// expressao | declaracao_variaveis | se | repita | leia | escreva | retorna | erro
TreeNode *acao() {

}

// corpo acao | vazio
TreeNode *corpo() {


}

// tipo ":" ID | parametro "[" "]"
TreeNode *parametro() {

}

// lista_parametros "," parametro | parametro | vazio
TreeNode *lista_parametros() {


}

// ID "(" lista_parametros ")" corpo FIM
TreeNode *cabecalho() {


}


// tipo cabecalho | cabecalho
TreeNode *declaracao_funcao() {


}

// INTEIRO | FLUTUANTE
TreeNode *tipo() {
	switch(tokenAtual->tokenval) {
		case INTEIRO:
		case FLUTUANTE:
			return novo_node(tokenAtual);

		default:
			printf("Err no tipo\n");
	}

	return NULL;
}

// indice "["  expressao "]" | "[" expressao "]"
TreeNode *indice() {


}

// D | ID indice
TreeNode *var() {
	switch(tokenAtual->tokenval){
		case ID:
		
			break;

		default:
			printf("Err no var\n");
	}

	return NULL;
}

// lista_variaveis "," var | var
TreeNode *lista_variaveis() {


}

// atribuicao
TreeNode *inicializacao_variaveis() {

}

// tipo ":" lista_variaveis
TreeNode *declaracao_variaveis() {

}

// declaracao_variaveis | inicializacao_variaveis | declaracao_funcao
TreeNode *declaracao() {

}

// lista_declaracao declaracao | declaracao
TreeNode *lista_declaracao() {
	TreeNode *raiz = novo_node((TokenRecord *) malloc(sizeof(TokenRecord)));

	while(tokenAtual->tokenval != EOFU){
		TreeNode *filho;
		switch(tokenAtual->tokenval){
			case SE:
				printf("chegou no se\n");
				insere_filho(raiz, se());
				break;
		}

		insere_filho(raiz, filho);
	}
}

TreeNode *parse() {
	tokenProximo = NULL;
	tokenAtual = getToken();			// atualiza o token
	printToken(tokenAtual, 0, 0);

	TreeNode *raiz = lista_declaracao();// recupera o programa

	return raiz;
}