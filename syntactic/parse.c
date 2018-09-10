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

// esta função retorna o token atual e já avança
// isto para evitar usar atual() e depois o proximo()
TokenRecord *atualEAvanca(){
	TokenRecord *tAtual = atual();	// recupera o token atual
	proximo();						// avanca para o próximo token

	return tAtual;					// retorna o token passado
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
		insere_filho(expressao, novo_node(atualEAvanca(), -1));
	}

	return expressao;
}

// RETORNA "(" expressao ")"
TreeNode *retorna() {

	TreeNode *retorna = novo_node(NULL, B_RETORNA);
	insere_filho(retorna, novo_node(atual(), -1));			// adicionando o RETORNA

	if(!verificaEAvanca(ABRE_PARENTESES, TRUE)){
		erro(nomeArquivo, verProximo(), "Token esperado '('.");
		printf("Err retorna\n");
		return NULL;
	}

	insere_filho(retorna, novo_node(atualEAvanca(), -1));	// adicionando o "("
	insere_filho(retorna, expressao());

	if(atual()->tokenval != FECHA_PARENTESES){
		erro(nomeArquivo, atual(), "Token esperado ')'.");
		printf("Err retorna.\n");
		return NULL;
	}

	insere_filho(retorna, novo_node(atualEAvanca(), -1));	// adicionando o ")"

	return retorna;
}

// ESCREVA "(" expressao ")"
TreeNode *escreva() {

	TreeNode *escreva = novo_node(NULL, B_ESCREVA);
	insere_filho(escreva, novo_node(atual(), -1));

	if(!verificaEAvanca(ABRE_PARENTESES, TRUE)){
		erro(nomeArquivo, verProximo(), "Token esperado '('.");
		printf("Err escreva\n");
		return NULL;
	}

	insere_filho(escreva, novo_node(atualEAvanca(), -1));	// insere como filho o "("
	insere_filho(escreva, expressao());						// insere como filho a expressao

	if(atual()->tokenval != FECHA_PARENTESES){
		erro(nomeArquivo, atual(), "Token esperado ')'.");
		printf("Err escreva.\n");
		return NULL;
	}

	insere_filho(escreva, novo_node(atualEAvanca(), -1));	// insere como filho o ")"

	return escreva;
}

// LEIA "(" var ")"
TreeNode *leia() {

	TreeNode *leia = novo_node(NULL, B_LEIA);
	insere_filho(leia, novo_node(atual(), -1));

	if(!verificaEAvanca(ABRE_PARENTESES, TRUE)){
		erro(nomeArquivo, verProximo(), "Token esperado '('.");
		printf("Err leia.\n");
		return NULL;
	}

	insere_filho(leia, novo_node(atualEAvanca(), -1));	// inserindo o filho "("
	insere_filho(leia, var());							// inserindo uma variavel

	if(atual()->tokenval != FECHA_PARENTESES){
		erro(nomeArquivo, atual(), "Token esperado ')'.");
		printf("Err leia..\n");
		return NULL;
	}

	insere_filho(leia, novo_node(atualEAvanca(), -1));	// insere o filho ")"

	return leia;
}

// var ":=" expressao
TreeNode *atribuicao() {

	TreeNode *atribuicao = novo_node(NULL, B_ATRIBUICAO);
	insere_filho(atribuicao, var());						// insere a variável como filho
	insere_filho(atribuicao, novo_node(atualEAvanca(), -1));// insere o ":=" como filho
	insere_filho(atribuicao, expressao());

	return atribuicao;
}

// REPITA corpo ATE expressao
TreeNode *repita() {

	TreeNode *repita = novo_node(NULL, B_REPITA);
	insere_filho(repita, novo_node(atualEAvanca(), -1));// adicionando o REPITA
	insere_filho(repita, corpo());						// adiciona o corpo

	if(atual()->tokenval != ATE){
		printf("Err repita\n");
		return NULL;
	}

	insere_filho(repita, novo_node(atualEAvanca(), -1));// adiciona o ATE
	insere_filho(repita, expressao());				// insere a expressão

	return repita;
}

// SE expressao ENTAO corpo FIM | SE expressao ENTAO corpo SENAO corpo FIM
TreeNode *se() {

	TreeNode *se = novo_node(NULL, B_SE);
	insere_filho(se, novo_node(atualEAvanca(), -1));// insere como filho o SE
	insere_filho(se, expressao());					// adiciona a expressao como filho

	if(atual()->tokenval != ENTAO){
		printf("Err se\n");
		return NULL;
	}

	insere_filho(se, novo_node(atualEAvanca(), -1));// insere como filho o ENTAO
	insere_filho(se, corpo());						// adiciona como filho o corpo

	// parte opcional
	if(atual()->tokenval == SENAO){
		insere_filho(se, novo_node(atualEAvanca(), -1));// adiciona o SENÃO
		insere_filho(se, corpo());						// insere o corpo do SENÃO
	}

	if(atual()->tokenval != FIM){
		printf("Err se.\n");
		return NULL;
	}

	insere_filho(se, novo_node(atualEAvanca(), -1));	// adiciona como filho o FIM

	return se;
}

// expressao | declaracao_variaveis | se | repita | leia | escreva | retorna | erro
TreeNode *acao() {

	TreeNode *acao = novo_node(NULL, ACAO);

	switch(atual()->tokenval){

		case INTEIRO:
		case FLUTUANTE:
			insere_filho(acao, declaracao_variaveis());
			break;

		case SE:
			insere_filho(acao, se());
			break;

		case REPITA:
			insere_filho(acao, repita());
			break;

		case LEIA:
			insere_filho(acao, leia());
			break;

		case ESCREVA:
			insere_filho(acao, escreva());
			break;

		case RETORNA:
			insere_filho(acao, retorna());
			break;

		// todos estes são o First do expressão
		case ID:
		case ABRE_PARENTESES:
		case NUM_F:
		case NUM_I:
		case SOMA:
		case SUBTRACAO:
			insere_filho(acao, expressao());
			break;

		// no caso do SENÃO, ATÉ
		default:
			erro(nomeArquivo, atual(), "Token inesperado.");
			printf("Err acao: ");
			printToken(atual(), 0, 0);
			return acao;
		break;
	}

	return acao;
}

// corpo acao | vazio
TreeNode *corpo() {

	TreeNode *corpo = novo_node(NULL, CORPO);

	// pode ser vazio
	while(atual()->tokenval != FIM && atual()->tokenval != ATE && atual()->tokenval != SENAO){
		insere_filho(corpo, acao());
	}

	return corpo;
}

// tipo ":" ID | parametro "[" "]"
TreeNode *parametro() {

	TreeNode *parametro = novo_node(NULL, PARAMETRO);
	insere_filho(parametro, tipo());					// adiciona o tipo como filho

	if(atual()->tokenval != DOIS_PONTOS) {				// se não for ":"
		erro(nomeArquivo, atual(), "Token esperado ':'.");
		printf("Err parametro\n");
		return parametro;
	}

	insere_filho(parametro, novo_node(atual(), -1));	// insere o ":"

	if(!verificaEAvanca(ID, TRUE)){
		erro(nomeArquivo, verProximo(), "Token esperado 'ID'.");
		printf("Err parametro.\n");
		return parametro;
	}

	insere_filho(parametro, novo_node(atualEAvanca(), -1));	// insere o ID

	// parametro "[" "]"
	while(atual()->tokenval == ABRE_COLCHETES && verProximo()->tokenval == FECHA_COLCHETES){
		insere_filho(parametro, novo_node(atualEAvanca(), -1));	// insere o "["
		insere_filho(parametro, novo_node(atualEAvanca(), -1));	// insere o "]"
	}

	return parametro;
}

// lista_parametros "," parametro | parametro | vazio
TreeNode *lista_parametros() {

	TreeNode *lista_parametros = novo_node(NULL, LISTA_PARAMETROS);

	if(atual()->tokenval == FECHA_PARENTESES){				// pode ser vazio
		return lista_parametros;
	}

	while(TRUE) {

		if(atual()->tokenval != INTEIRO && atual()->tokenval != FLUTUANTE){
			erro(nomeArquivo, atual(), "Token esperado: 'INTEIRO' ou 'FLUTUANTE'.");
			printf("Err lista_parametros\n");
			exit(1);
			return NULL;
		}

		insere_filho(lista_parametros, parametro());

		if(atual()->tokenval == FECHA_PARENTESES) {
			return lista_parametros;
		}

		if(atual()->tokenval != VIRGULA) {
			erro(nomeArquivo, atual(), "token esperado: ','.");
			printf("Err lista_parametros.\n");
			exit(1);
			return NULL;
		}

		insere_filho(lista_parametros, novo_node(atualEAvanca(), -1));	// insere o ","
	}
}

// ID "(" lista_parametros ")" corpo FIM
TreeNode *cabecalho() {

	TreeNode *cabecalho = novo_node(NULL, CABECALHO);
	insere_filho(cabecalho, novo_node(atual(), -1));	// insere o filho ID

	if(!verificaEAvanca(ABRE_PARENTESES, TRUE)) {
		printf("Err cabecalho\n");
		return NULL;
	}

	insere_filho(cabecalho, novo_node(atualEAvanca(), -1));	// insere como filho o "("
	insere_filho(cabecalho, lista_parametros());			// insere como filho a lista_parametros

	if(atual()->tokenval != FECHA_PARENTESES){
		printf("Err cabecalho.\n");
		return NULL;
	}

	insere_filho(cabecalho, novo_node(atualEAvanca(), -1));	// insere como filho o ")"
	insere_filho(cabecalho, corpo());						// insere como filho o corpo

	if(atual()->tokenval != FIM){
		printf("Err cabecalho..\n");
		return NULL;
	}

	insere_filho(cabecalho, novo_node(atualEAvanca(), -1));	// insere como filho o FIM

	return cabecalho;
}

// tipo cabecalho | cabecalho
TreeNode *declaracao_funcao() {

	TreeNode *declaracao_funcao = novo_node(NULL, DECLARACAO_FUNCAO);

	// verificação não redundante, pois poderia chegar até esta função com o cabecalho
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

		TreeNode *abre = novo_node(atualEAvanca(), -1);		// cria o nó "]"
		TreeNode *exp = expressao();						// cria o nó expressao

		// se não encontrar o FECHA_COLCHETES ou a expressão for NULL
		if(atual()->tokenval != FECHA_COLCHETES || !exp){
			printf("Err indice\n");
			return NULL;
		}

		insere_filho(indice, abre);							// adiciona o nó "["
		insere_filho(indice, exp);							// adiciona o nó expressao
		insere_filho(indice, novo_node(atualEAvanca(), -1));// adiciona o nó "]"

		// se for igual a "[", então recupera o próximo indice
		if(atual()->tokenval != ABRE_COLCHETES){
			return indice;
		}
	}
}

// ID | ID indice
TreeNode *var() {

	TreeNode *var = novo_node(NULL, VAR);				// cria o nó ID
	insere_filho(var, novo_node(atualEAvanca(), -1));	// adiciona o filho ID e avança

	if (atual()->tokenval == ABRE_COLCHETES)
		insere_filho(var, indice());

	return var;
}

// lista_variaveis "," var | var
TreeNode *lista_variaveis() {

	TreeNode *lista_variaveis = novo_node(NULL, LISTA_VARIAVEIS);

	insere_filho(lista_variaveis, var());	// recupera a primeira var

	while(TRUE) {
		// enquanto o atual for VIRGULA
		if(atual()->tokenval != VIRGULA)
			break;

		// se houver um token "," e não houver após um ID
		if(!verificaEAvanca(ID, FALSE)){
			erro(nomeArquivo, verProximo(), "Token esperado 'ID'.");
			printf("Err lista_variaveis\n");
			return NULL;
		}

		insere_filho(lista_variaveis, novo_node(atualEAvanca(), -1));	// adiciona a vírgula
		insere_filho(lista_variaveis, var());							// recupera a var
	}

	return lista_variaveis;
}

// atribuicao
TreeNode *inicializacao_variaveis() {

	TreeNode *inicializacao_variaveis = novo_node(NULL, INICIALIZACAO_VARIAVEIS);
	insere_filho(inicializacao_variaveis, atribuicao());

	return inicializacao_variaveis;
}

// INTEIRO | FLUTUANTE
TreeNode *tipo() {

	TreeNode *tipo = novo_node(NULL, TIPO);				// cria um nó tipo
	insere_filho(tipo, novo_node(atualEAvanca(), -1));	// insere o novo filho com o token e avança o token

	return tipo;
}

// tipo ":" lista_variaveis
TreeNode *declaracao_variaveis() {

	TreeNode *declaracao_variaveis = novo_node(NULL, DECLARACAO_VARIAVEIS);	// recupera o token que trouxe até aqui INTEIRO | FLUTUANTE
	insere_filho(declaracao_variaveis, tipo());								// insere o tipo como filho de declaracao_variaveis

	if(verificaEAvanca(DOIS_PONTOS, TRUE)) {								// se não houver um tipo e o próximo não for DOIS_PONTOS; porém, esta verificação é redundante
		printf("Err declaracao_variaveis\n");
		return NULL;
	}

	insere_filho(declaracao_variaveis, novo_node(atual(), -1));				// insere o ":" como filho de declaracao_variaveis

	if(!verificaEAvanca(ID, TRUE)){
		erro(nomeArquivo, verProximo(), "esperando token 'ID'.");
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
    			printToken(atual(), 1, 1);
    			erro(nomeArquivo, verProximo(), "esperando token ':' ou 'ID'.");
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

TreeNode *parse(char *arquivo) {
	tokenProximo = NULL;
	nomeArquivo = arquivo;						// quando houver algum erro/warning, este arquivo será reaberto

	TreeNode *programa = criaPrograma();		// cria o nó programa
	insere_filho(programa, lista_declaracoes());// recupera o programa

	return programa;
}