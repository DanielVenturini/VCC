#include "parse.h"
#include "../lexical/varredura.h"

// apenas declarando para não ter problemas de função a cima que não reconhece as abaixos
TreeNode *expressao_multiplicativa();
TreeNode *inicializacao_variaveis();
TreeNode *operador_multiplicacao();
TreeNode *declaracao_variaveis();
TreeNode *operador_relacional();
TreeNode *lista_declaracoes();
TreeNode *expressao_aditiva();
TreeNode *expressao_simples();
TreeNode *declaracao_funcao();
TreeNode *lista_parametros();
TreeNode *expressao_unaria();
TreeNode *expressao_logica();
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
TreeNode *indice();
TreeNode *repita();
TreeNode *numero();
TreeNode *corpo();
TreeNode *fator();
TreeNode *leia();
TreeNode *tipo();
TreeNode *acao();
TreeNode *var();
TreeNode *se();

// apenas cria um nó que será o nó programa
TreeNode *criaPrograma() {
	return novo_node(NULL, PROGRAMA);
}

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
			return novo_node(atual(), NUMERO);

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
			return novo_node(atual(), OPERADOR_MULTIPLICACAO);

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
			return novo_node(atual(), OPERADOR_LOGICO);

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
			return novo_node(atual(), OPERADOR_SOMA);

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
			return novo_node(atual(), OPERADOR_RELACIONAL);

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
	return novo_node(proximo(), -1);
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

// indice "["  expressao "]" | "[" expressao "]"
TreeNode *indice() {

	TreeNode *indice = novo_node(NULL, INDICE);

	while(TRUE){

		if(verProximo()->tokenval != ABRE_COLCHETES){
			return NULL;
		}

		TreeNode *abre = novo_node(proximo(), -1);
		TreeNode *exp = expressao();

		// se não encontrar o FECHA_COLCHETES ou a expressão for NULL
		if(verProximo()->tokenval != FECHA_COLCHETES || !expressao){
			return NULL;
		}

		TreeNode *fecha = novo_node(proximo(), -1);
		insere_filho(indice, abre);
		insere_filho(indice, exp);
		insere_filho(indice, fecha);

		if(verProximo()->tokenval != ABRE_COLCHETES){
			return indice;
		}
	}
}

// ID | ID indice
TreeNode *var() {

	if (atual()->tokenval != ID){
		printf("Err VAR.\n");
		return NULL;
	}

	TreeNode *var = novo_node(NULL, VAR);		// cria o nó ID
	insere_filho(var, novo_node(atual(), -1));	// no filho da VAR coloca o id

	if (verProximo()->tokenval == ABRE_COLCHETES)
		insere_filho(var, indice());

	return var;
}

// lista_variaveis "," var | var
TreeNode *lista_variaveis() {

	TreeNode *lista_variaveis = novo_node(NULL, LISTA_VARIAVEIS);

	proximo();								// avança para o ID
	insere_filho(lista_variaveis, var());	// recupera a primeira var

	while(TRUE) {								// não tem mais nada na gramática para lista_variaveis
		if(proximo()->tokenval != VIRGULA)		// se não houver uma vírgula, para
			break;

		insere_filho(lista_variaveis, novo_node(atual(), -1));	// adiciona a vírgula
		proximo();												// avanca para o ID
		insere_filho(lista_variaveis, var());					// recupera a primeira var
	}

	return lista_variaveis;
}

// atribuicao
TreeNode *inicializacao_variaveis() {

}

// INTEIRO | FLUTUANTE
TreeNode *tipo() {

	TokenType tipoToken = atual()->tokenval;

	if(tipoToken != FLUTUANTE && tipoToken != INTEIRO){
		printf("Err TIPO.\n");
		return NULL;
	}

	TreeNode *tipo = novo_node(NULL, TIPO);
	insere_filho(tipo, novo_node(atual(), -1));

	return tipo;
}

// tipo ":" lista_variaveis
TreeNode *declaracao_variaveis() {

	TreeNode *declaracao_variaveis = novo_node(NULL, DECLARACAO_VARIAVEIS);	// recupera o token que trouxe até aqui INTEIRO | FLUTUANTE
	insere_filho(declaracao_variaveis, tipo());			// insere o tipo como filho de declaracao_variaveis

	if(proximo()->tokenval != DOIS_PONTOS) {	// esta verificação provavelmente é redundante
		printf("Err declaracao_variaveis.\n");
		return NULL;
	}

	insere_filho(declaracao_variaveis, novo_node(atual(), -1));	// insere o ":" como filho de declaracao_variaveis
	insere_filho(declaracao_variaveis, lista_variaveis());

	return declaracao_variaveis;
}

// declaracao_variaveis | inicializacao_variaveis | declaracao_funcao
TreeNode *declaracao() {

    TreeNode *declaracao = novo_node(NULL, DECLARACAO);

    switch(atual()->tokenval){

    	// declaracao -> declaracao_variaveis | declaracao_funcao
    	// declaracao_variaveis -> tipo ":" lista_variaveis
    	// declaracao_funcao -> tipo cabecalho | cabecalho
    	// pode ser tanto uma declaracao_variaveis ou declaracao_funcao
    	// ambas começam com TIPO, mas declaracao_funcao pode começar com um cabecalho também
    	case INTEIRO:
    	case FLUTUANTE:
    		// em declaracao_variaveis o próximo token será ":"
    		// já em declaracao_funcao será um IDENTIFICADOR
    		if(verProximo()->tokenval == DOIS_PONTOS){
    			insere_filho(declaracao, declaracao_variaveis());	// se o token próximo é ":", então é declaração_variaveis
    		} else if (verProximo()->tokenval == ID){
    			insere_filho(declaracao, declaracao_funcao());		// em declaracao_funcao, a regra que começa com um tipo, é seguida de um ID
    		}

    		break;

    	// declaracao_funcao -> tipo cabecalho | cabecalho
    	// cabecalho -> ID ...
    	// se não começar com inteiro mas começar com um ID, então é uma declaração_função
    	case ID:
    		// make something
    		break;

    	default:
    		// wrong token
    		break;
    }

    return declaracao;
}

// lista_declaracao declaracao | declaracao
TreeNode *lista_declaracoes() {

	// cria um novo nó sem token
	TreeNode *lista_declaracao = novo_node(NULL, LISTA_DECLARACOES);

	// enquanto estiver tokens para consumir
	while(atual()->tokenval != EOFU){
		// recupera uma declaracao
		insere_filho(lista_declaracao, declaracao());
	}

	return lista_declaracao;
}

TreeNode *parse() {
	tokenProximo = NULL;

	TreeNode *programa = criaPrograma();
	proximo();									// avança para o primeiro token
	insere_filho(programa, lista_declaracoes());// recupera o programa

	return programa;
}