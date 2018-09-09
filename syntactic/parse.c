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

// esta função verifica se o proximo token é do tipo TokenType tipo
// se for, retorna TRUE
// senão, retorna FALSE
// se for especificado, será avançado o token
// isto evita ficar verificando com verProximo() e depois usar o proximo()
char verificaEAvanca(TokenType tipo, char avancar) {

	if(verProximo()->tokenval != tipo){	// verifica se é o tipo correto
		return FALSE;
	}

	if(avancar)
		proximo();						// se for, atualiza

	return TRUE;
}

// apenas cria um nó que será o nó programa
TreeNode *criaPrograma() {
	proximo();									// avança para o primeiro token
	return novo_node(NULL, PROGRAMA);
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

	TreeNode *expressao = novo_node(NULL, EXPRESSAO);

	// se o próximo for ":="
	if(verificaEAvanca(ATRIBUICAO, FALSE))
		insere_filho(expressao, atribuicao());
	else {
		//insere_filho(expressao, expressao_logica());
		insere_filho(expressao, novo_node(atual(), -1));
		proximo();
	}

	return expressao;
}

// RETORNA "(" expressao ")"
TreeNode *retorna() {

}

// ESCREVA "(" expressao ")"
TreeNode *escreva() {


}

// LEIA "(" var ")"
TreeNode *leia() {

	if(atual()->tokenval != LEIA){
		printf("Err leia\n");
		return NULL;
	}

	TreeNode *leia = novo_node(NULL, B_LEIA);

	if(!verificaEAvanca(ABRE_PARENTESES, TRUE)){
		printf("Err leia.\n");
		return NULL;
	}

	insere_filho(leia, novo_node(atual(), -1));	// inserindo o filho "("
	proximo();									// avança para o próximo token
	insere_filho(leia, var());					// inserindo uma variavel

	if(atual()->tokenval != FECHA_PARENTESES){
		printf("Err leia..\n");
		return NULL;
	}

	insere_filho(leia, novo_node(atual(), -1));	// insere o filho ")"
	proximo();									// avança para o próximo token

	return leia;
}

// var ":=" expressao
TreeNode *atribuicao() {

	TreeNode *atribuicao = novo_node(NULL, B_ATRIBUICAO);
	insere_filho(atribuicao, var());					// insere a variável como filho

	if(atual()->tokenval != ATRIBUICAO){				// verifica se o próximo é ":="
		printf("Err atribuicao\n");
		return NULL;
	}

	insere_filho(atribuicao, novo_node(atual(), -1));	// insere o ":=" como filho
	proximo();
	insere_filho(atribuicao, expressao());

	return atribuicao;
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

	TreeNode *corpo = novo_node(NULL, CORPO);

	while(atual()->tokenval != FIM){

		switch(atual()->tokenval){

			case INTEIRO:
			case FLUTUANTE:
				insere_filho(corpo, declaracao_variaveis());
				break;

			case SE:
				insere_filho(corpo, se());
				break;

			case REPITA:
				insere_filho(corpo, repita());
				break;

			case LEIA:
				insere_filho(corpo, leia());
				break;

			case ESCREVA:
				insere_filho(corpo, escreva());
				break;

			case RETORNA:
				insere_filho(corpo, retorna());
				break;

			// todos estes são o First do expressão
			case ID:
			case ABRE_PARENTESES:
			case NUM_F:
			case NUM_I:
			case SOMA:
			case SUBTRACAO:
				insere_filho(corpo, expressao());
				break;

			default:
				printf("Err corpo: ");
				printToken(atual(), 0, 0);
				return NULL;
			break;
		}
	}

	return corpo;
}

// tipo ":" ID | parametro "[" "]"
TreeNode *parametro() {

	if(atual()->tokenval != INTEIRO && atual()->tokenval != FLUTUANTE){
		printf("Err parametro\n");
		return NULL;
	}

	TreeNode *parametro = novo_node(NULL, PARAMETRO);
	insere_filho(parametro, tipo());					// adiciona o tipo como filho

	if(atual()->tokenval != DOIS_PONTOS) {				// se não for ":"
		printf("Err parametro.\n");
		return parametro;
	}

	insere_filho(parametro, novo_node(atual(), -1));	// insere o ":"

	if(!verificaEAvanca(ID, TRUE)){
		printf("Err parametro..\n");
		return parametro;
	}

	insere_filho(parametro, novo_node(atual(), -1));	// insere o ID
	proximo();											// avança para o próximo token

	// parametro "[" "]"
	while(atual()->tokenval == ABRE_COLCHETES && verProximo()->tokenval == FECHA_COLCHETES){
		insere_filho(parametro, novo_node(atual(), -1));	// insere o "["
		insere_filho(parametro, novo_node(proximo(), -1));	// insere o "]"

		proximo();											// avança para o próximo token
	}

	return parametro;
}

// lista_parametros "," parametro | parametro | vazio
TreeNode *lista_parametros() {

	TreeNode *lista_parametros = novo_node(NULL, LISTA_PARAMETROS);

	while(TRUE) {

		if(atual()->tokenval == FECHA_PARENTESES){				// pode ser vazio ou chegar no final
			return lista_parametros;
		}

		insere_filho(lista_parametros, parametro());

		if(atual()->tokenval != VIRGULA){
			return lista_parametros;
		}

		insere_filho(lista_parametros, novo_node(atual(), -1));	// insere o ","
		proximo();
	}
}

// ID "(" lista_parametros ")" corpo FIM
TreeNode *cabecalho() {

	if(atual()->tokenval != ID){
		printf("Err cabecalho\n");
		return NULL;
	}

	TreeNode *cabecalho = novo_node(NULL, CABECALHO);
	insere_filho(cabecalho, novo_node(atual(), -1));	// insere o filho ID

	if(!verificaEAvanca(ABRE_PARENTESES, TRUE)) {
		printf("Err cabecalho.\n");
		return NULL;
	}

	insere_filho(cabecalho, novo_node(atual(), -1));// insere como filho o "("
	proximo();										// avanca para o próximo token
	insere_filho(cabecalho, lista_parametros());	// insere como filho a lista_parametros

	if(atual()->tokenval != FECHA_PARENTESES){
		printf("Err cabecalho..\n");
		return NULL;
	}

	insere_filho(cabecalho, novo_node(atual(), -1));// insere como filho o ")"
	proximo();										// avança para o próximo token
	insere_filho(cabecalho, corpo());				// insere como filho o corpo

	if(atual()->tokenval != FIM){
		printf("Err cabecalho...\n");
		return NULL;
	}

	insere_filho(cabecalho, novo_node(atual(), -1));// insere como filho o FIM
	proximo();										// vai para o próximo token

	return cabecalho;
}

// tipo cabecalho | cabecalho
TreeNode *declaracao_funcao() {

	TreeNode *declaracao_funcao = novo_node(NULL, DECLARACAO_FUNCAO);

	if(atual()->tokenval == INTEIRO || atual()->tokenval == FLUTUANTE){
		insere_filho(declaracao_funcao, tipo());	// recupera o tipo
	}

	insere_filho(declaracao_funcao, cabecalho());

	return declaracao_funcao;
}

// indice "["  expressao "]" | "[" expressao "]"
TreeNode *indice() {

	TreeNode *indice = novo_node(NULL, INDICE);

	while(TRUE){

		TreeNode *abre = novo_node(atual(), -1);
		proximo();								// avanca para a expressao
		TreeNode *exp = expressao();

		// se não encontrar o FECHA_COLCHETES ou a expressão for NULL
		if(atual()->tokenval != FECHA_COLCHETES || !exp){
			return NULL;
		}

		TreeNode *fecha = novo_node(atual(), -1);
		insere_filho(indice, abre);
		insere_filho(indice, exp);
		insere_filho(indice, fecha);

		if(!verificaEAvanca(ABRE_COLCHETES, TRUE)){
			return indice;
		}
	}
}

// ID | ID indice
TreeNode *var() {

	if (atual()->tokenval != ID){
		printf("Err var\n");
		return NULL;
	}

	TreeNode *var = novo_node(NULL, VAR);		// cria o nó ID
	insere_filho(var, novo_node(atual(), -1));	// no filho da VAR coloca o id

	if (verificaEAvanca(ABRE_COLCHETES, TRUE))
		insere_filho(var, indice());

	proximo();									// avança o próximo token
	return var;
}

// lista_variaveis "," var | var
TreeNode *lista_variaveis() {

	TreeNode *lista_variaveis = novo_node(NULL, LISTA_VARIAVEIS);

	insere_filho(lista_variaveis, var());	// recupera a primeira var

	if(!lista_variaveis->filhos[0]) {		// se não houver variáveis
		return NULL;						// retorna NULL
	}

	while(TRUE) {								// não tem mais nada na gramática para lista_variaveis
		if(atual()->tokenval != VIRGULA)		// se não houver uma vírgula, para
			break;

		insere_filho(lista_variaveis, novo_node(atual(), -1));	// adiciona a vírgula
		proximo();												// avanca para o ID
		insere_filho(lista_variaveis, var());					// recupera a primeira var
	}

	return lista_variaveis;
}

// atribuicao
TreeNode *inicializacao_variaveis() {

	if(atual()->tokenval != ID){
		printf("Err inicializacao_variaveis\n");
		return NULL;
	}

	TreeNode *inicializacao_variaveis = novo_node(NULL, INICIALIZACAO_VARIAVEIS);
	insere_filho(inicializacao_variaveis, atribuicao());

	return inicializacao_variaveis;
}

// INTEIRO | FLUTUANTE
TreeNode *tipo() {

	TokenType tipoToken = atual()->tokenval;			// recupera o tipo do token atual

	if(tipoToken != FLUTUANTE && tipoToken != INTEIRO){	// verifica se está errado
		printf("Err TIPO.\n");
		return NULL;
	}

	TreeNode *tipo = novo_node(NULL, TIPO);				// cria um nó tipo
	insere_filho(tipo, novo_node(atual(), -1));			// insere o novo filho com o token

	proximo();											// avança para o próximo token
	return tipo;
}

// tipo ":" lista_variaveis
TreeNode *declaracao_variaveis() {

	TreeNode *declaracao_variaveis = novo_node(NULL, DECLARACAO_VARIAVEIS);	// recupera o token que trouxe até aqui INTEIRO | FLUTUANTE
	insere_filho(declaracao_variaveis, tipo());								// insere o tipo como filho de declaracao_variaveis

	if(!declaracao_variaveis->filhos[0] || verificaEAvanca(DOIS_PONTOS, TRUE)) {	// se não houver um tipo e o próximo não for DOIS_PONTOS; porém, esta verificação é redundante
		printf("Err declaracao_variaveis\n");
		return NULL;
	}

	insere_filho(declaracao_variaveis, novo_node(atual(), -1));	// insere o ":" como filho de declaracao_variaveis

	if(!verificaEAvanca(ID, TRUE)){
		printf("Err declaracao_variaveis.\n");
		return NULL;
	}

	insere_filho(declaracao_variaveis, lista_variaveis());		// recupera a lista de variáveis
	return declaracao_variaveis;
}

// declaracao_variaveis | inicializacao_variaveis | declaracao_funcao
TreeNode *declaracao() {

    TreeNode *declaracao = novo_node(NULL, DECLARACAO);

    switch(atual()->tokenval) {

    	// pode ser tanto uma declaracao_variaveis ou declaracao_funcao
    	// ambas começam com TIPO, mas declaracao_funcao pode começar com um cabecalho também
    	case INTEIRO:
    	case FLUTUANTE:
    		// em declaracao_variaveis o próximo token será ":"
    		// já em declaracao_funcao será um IDENTIFICADOR
    		if(verificaEAvanca(DOIS_PONTOS, FALSE)) {
    			insere_filho(declaracao, declaracao_variaveis());	// se o token próximo é ":", então é declaração_variaveis
    		} else if (verificaEAvanca(ID, FALSE)) {
    			insere_filho(declaracao, declaracao_funcao());		// em declaracao_funcao, a regra que começa com um tipo, é seguida de um ID
    		} else {
    			printf("Err declaracao\n");
    		}

    		break;

    	// se começar com um ID, então pode ser uma declaração_funcao
    	// mas também pode ser uma inicializacao_variaveis
    	case ID:
    		// em cabecalho o próximo token será "("
    		// já em atribuicao será um ATRIBUICAO
    		if(verificaEAvanca(ABRE_PARENTESES, FALSE)) {
    			insere_filho(declaracao, declaracao_funcao());
    		} else if (verificaEAvanca(ATRIBUICAO, FALSE) || verificaEAvanca(ABRE_COLCHETES, FALSE)) {
    			insere_filho(declaracao, inicializacao_variaveis());
    		} else {
    			printf("Err declaracao.\n");
    		}

    		break;

    	default:
    		printf("Err declaracao..\n");
    		// wrong token
    		break;
    }

    return declaracao;
}

// lista_declaracao declaracao | declaracao
TreeNode *lista_declaracoes() {

	TreeNode *lista_declaracao = novo_node(NULL, LISTA_DECLARACOES);	// cria um novo nó sem token

	while(atual()->tokenval != EOFU){									// enquanto estiver tokens para consumir
		insere_filho(lista_declaracao, declaracao());					// recupera uma declaracao
	}

	return lista_declaracao;
}

TreeNode *parse() {
	tokenProximo = NULL;

	TreeNode *programa = criaPrograma();		// cria o nó programa
	insere_filho(programa, lista_declaracoes());// recupera o programa

	return programa;
}