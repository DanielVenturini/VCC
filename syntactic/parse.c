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

	TreeNode *lista_argumentos = novo_node(NULL, LISTA_ARGUMENTOS);

	// vazio
	if(atual()->tokenval == FECHA_PARENTESES){
		return lista_argumentos;
	}

	while(TRUE){

		insere_filho(lista_argumentos, expressao());	// insere o filho expressao

		if(atual()->tokenval != VIRGULA){
			return lista_argumentos;
		}

		insere_filho(lista_argumentos, novo_node(atualEAvanca(), -1));	// insere o filho ","
	}


	return lista_argumentos;
}

// ID "(" lista_argumentos ")"
TreeNode *chamada_funcao() {

	TreeNode *chamada_funcao = novo_node(NULL, CHAMADA_FUNCAO);
	insere_filho(chamada_funcao, novo_node(atualEAvanca(), -1));	// adiciona o filho "ID"
	insere_filho(chamada_funcao, novo_node(atualEAvanca(), -1));	// adiciona o filho "("
	insere_filho(chamada_funcao, lista_argumentos());				// adiciona o filho lista_argumentos

	if(atual()->tokenval != FECHA_PARENTESES){
		printf("Err chamada_funcao\n");
		erro(nomeArquivo, atual(), "Token esperado: ')'.");
		return NULL;
	}

	insere_filho(chamada_funcao, novo_node(atualEAvanca(), -1));	// adiciona o filho ")"

	return chamada_funcao;
}

// NUM_INTEIRO | NUM_PONTO_FLUTUANTE | NUM_NOTACAO_CIENTIFICA
TreeNode *numero() {

	TreeNode *numero = novo_node(NULL, NUMERO);
	insere_filho(numero, novo_node(atualEAvanca(), -1));	// insere o filho numero

	return numero;
}

// "(" expressao ")" | var | chamada_funcao | numero
TreeNode *fator() {

	TreeNode *fator = novo_node(NULL, FATOR);

	switch(tokenAtual->tokenval){
		case ABRE_PARENTESES:									// "(" expressao ")"
			insere_filho(fator, novo_node(atualEAvanca(), -1));	// insere o filho "("
			insere_filho(fator, expressao());					// recupera a expressão

			if(atual()->tokenval != FECHA_PARENTESES){			// verifica o fecha parenteses
				printf("Err fator\n");
				erro(nomeArquivo, atual(), "Token esperado: '('.");
				return NULL;
			}

			insere_filho(fator, novo_node(atualEAvanca(), -1));	// insere o filho ")"
			break;

		case ID:
			if(verificaEAvanca(ABRE_PARENTESES, FALSE)) {
				insere_filho(fator, chamada_funcao());			// insere o filho chamada_funcao()
			} else {
				insere_filho(fator, var());						// insere o filho var()
			}

			break;

		case NUM_F:
		case NUM_I:
			insere_filho(fator, numero());
			break;

		default:
			printf("Err fator\n");
			erro(nomeArquivo, atual(), "Token esperado: '(', 'ID' ou NUMERO.");
			return NULL;
	}

	return fator;
}

TreeNode *operador_negacao() {

	TreeNode *operador_negacao = novo_node(NULL, OPERADOR_NEGACAO);

	switch(atual()->tokenval) {
		case NEGACAO:
			insere_filho(operador_negacao, novo_node(atualEAvanca(), -1));
			break;

		default:
			printf("Err operador_negacao\n");
			erro(nomeArquivo, atual(), "Token esperado: '!'.");
			operador_negacao = NULL;
			break;
	}

	return operador_negacao;
}

// "*" | "/"
TreeNode *operador_multiplicacao() {

	TreeNode *operador_multiplicacao = novo_node(NULL, OPERADOR_MULTIPLICACAO);

	switch(atual()->tokenval){
		case MULTIPLICACAO:
		case DIVISAO:
			insere_filho(operador_multiplicacao, novo_node(atualEAvanca(), -1));
			break;

		default:
			printf("Erro operador_multiplicacao\n");
			erro(nomeArquivo, atual(), "Token esperado: '*' ou '/'.");
			operador_multiplicacao = NULL;
	}

	return operador_multiplicacao;
}

// "&&" | "||"
TreeNode *operador_logico() {

	TreeNode *operador_logico = novo_node(NULL, OPERADOR_LOGICO);

	switch(atual()->tokenval){
		case OU_LOGICO:
		case E_LOGICO:
			insere_filho(operador_logico, novo_node(atualEAvanca(), -1));
			break;

		default:
			printf("Err operador_logico\n");
			erro(nomeArquivo, atual(), "Token esperado: '&&' ou '||'.");
			operador_logico = NULL;
	}

	return operador_logico;
}

// "+" | "-"
TreeNode *operador_soma() {

	TreeNode *operador_soma = novo_node(NULL, OPERADOR_SOMA);

	switch(atual()->tokenval){
		case SOMA:
		case SUBTRACAO:
			insere_filho(operador_soma, novo_node(atualEAvanca(), -1));
			break;

		default:
			printf("Err operador_soma\n");
			erro(nomeArquivo, atual(), "Token esperado: '+' ou '-'.");
			operador_soma = NULL;
	}

	return operador_soma;
}

// "<" | ">" | "=" | "<>" "<=" | ">="
TreeNode *operador_relacional() {

	TreeNode *operador_relacional = novo_node(NULL, OPERADOR_RELACIONAL);

	switch(atual()->tokenval) {

		case MENOR:
		case MAIOR:
		case IGUALDADE:
		case DIFERENTE:
		case MENOR_IGUAL:
		case MAIOR_IGUAL:
			insere_filho(operador_relacional, novo_node(atualEAvanca(), -1));
			break;

		default:			// marca não reconhecida
			printf("Err operador_relacional\n");
			erro(nomeArquivo, atual(), "Token esperado: '<', '>', '=', '<>', '<=' ou '>='");
			operador_relacional = NULL;
	}

	return operador_relacional;
}

// fator | operador_soma fator | operador_negacao fator
TreeNode *expressao_unaria() {

	TreeNode *exp_unaria = novo_node(NULL, EXPRESSAO_UNARIA);
	switch(atual()->tokenval){

		case ABRE_PARENTESES:
		case ID:
		case NUM_F:
		case NUM_I:
			insere_filho(exp_unaria, fator());
			break;

		case SOMA:
		case SUBTRACAO:
			insere_filho(exp_unaria, operador_soma());
			insere_filho(exp_unaria, fator());
			break;

		case NEGACAO:
			insere_filho(exp_unaria, operador_negacao());
			insere_filho(exp_unaria, fator());
			break;
	}

	return exp_unaria;
}

// expressao_unaria | expressao_multiplicativa operador_multiplicacao expressao_unaria
TreeNode *expressao_multiplicativa() {

	TreeNode *exp_multiplicativa = novo_node(NULL, EXPRESSAO_MULTIPLICATIVA);
	insere_filho(exp_multiplicativa, expressao_unaria());	// insere o filho expressao_unaria()

	if(atual()->tokenval != MULTIPLICACAO && atual()->tokenval != DIVISAO) {
		return exp_multiplicativa;
	}

	//insere_filho(exp_multiplicativa, expressao_multiplicativa());	// recursão à esquerda
	insere_filho(exp_multiplicativa, operador_multiplicacao());		// insere como filho o operador
	insere_filho(exp_multiplicativa, expressao_unaria());			// insere como filho o operador

	return exp_multiplicativa;
}

// expressao_multiplicativa | expressao_aditiva operador_soma expressao_multiplicativa
TreeNode *expressao_aditiva() {

	TreeNode *exp_aditiva = novo_node(NULL, EXPRESSAO_ADITIVA);
	insere_filho(exp_aditiva, expressao_multiplicativa());	// insere o filho expressao_multiplicativa()

	// se for um destes, é a recursão à esquerda
	if(atual()->tokenval != SOMA && atual()->tokenval != SUBTRACAO) {

		return exp_aditiva;
	}

	//insere_filho(exp_aditiva, expressao_aditiva());			// recursão à esquerda
	insere_filho(exp_aditiva, operador_soma());				// insere como filho o operador
	insere_filho(exp_aditiva, expressao_multiplicativa());	// insere como filho o operador

	return exp_aditiva;
}

// expressao_aditiva | expressao_simples operador_relacional expressao_aditiva
TreeNode *expressao_simples() {

	TreeNode *exp_simples = novo_node(NULL, EXPRESSAO_SIMPLES);
	insere_filho(exp_simples, expressao_aditiva());	// insere o filho expressao_aditiva()

	// se não for uma expressao_relacional
	if(atual()->tokenval != MENOR && atual()->tokenval != MAIOR && atual()->tokenval != MENOR_IGUAL &&
		atual()->tokenval != MAIOR_IGUAL && atual()->tokenval != IGUALDADE && atual()->tokenval != DIFERENTE) {

		return exp_simples;
	}

	//insere_filho(exp_simples, expressao_simples());		// recursão à esquerda
	insere_filho(exp_simples, operador_relacional());	// insere como filho o operador
	insere_filho(exp_simples, expressao_aditiva());		// insere como filho o operador

	return exp_simples;
}

// expressao_simples | expressao_logica operador_logico expressao_simples
TreeNode *expressao_logica() {

	TreeNode *exp_logica = novo_node(NULL, EXPRESSAO_LOGICA);
	insere_filho(exp_logica, expressao_simples());	// insere o filho expressao_simples()

	// se for um destes, é a recursão à esquerda
	if(atual()->tokenval != OU_LOGICO && atual()->tokenval != E_LOGICO) {
		return exp_logica;
	}

	//insere_filho(exp_logica, expressao_logica());	// recursão à esquerda
	insere_filho(exp_logica, operador_logico());	// insere como filho o operador
	insere_filho(exp_logica, expressao_simples());	// insere como filho o operador

	return exp_logica;
}

// expressao_logica | atribuicao
TreeNode *expressao() {

	if(atual()->tokenval != ABRE_PARENTESES && atual()->tokenval != ID && atual()->tokenval != NUM_F &&
		atual()->tokenval != NUM_I && atual()->tokenval != SOMA && atual()->tokenval != SUBTRACAO &&
		atual()->tokenval != DIFERENTE){

		printf("Err expressao\n");
		erro(nomeArquivo, atual(), "Token esperado: '(', 'ID', 'NUMERO', '+', '-' ou '!'.");
		return NULL;
	}

	TreeNode *expressao = novo_node(NULL, EXPRESSAO);

	// se o próximo for ":="
	if(atual()->tokenval == ID && verificaEAvanca(ATRIBUICAO, FALSE))
		insere_filho(expressao, atribuicao());
	else {
		insere_filho(expressao, expressao_logica());
	}

	return expressao;
}

// RETORNA "(" expressao ")"
TreeNode *retorna() {

	TreeNode *retorna = novo_node(NULL, B_RETORNA);
	insere_filho(retorna, novo_node(atual(), -1));			// adicionando o RETORNA

	if(!verificaEAvanca(ABRE_PARENTESES, TRUE)){
		printf("Err retorna\n");
		erro(nomeArquivo, verProximo(), "Token esperado '('.");
		return NULL;
	}

	insere_filho(retorna, novo_node(atualEAvanca(), -1));	// adicionando o "("
	insere_filho(retorna, expressao());

	if(atual()->tokenval != FECHA_PARENTESES){
		printf("Err retorna.\n");
		erro(nomeArquivo, atual(), "Token esperado ')'.");
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
		printf("Err escreva\n");
		erro(nomeArquivo, verProximo(), "Token esperado '('.");
		return NULL;
	}

	insere_filho(escreva, novo_node(atualEAvanca(), -1));	// insere como filho o "("
	insere_filho(escreva, expressao());						// insere como filho a expressao

	if(atual()->tokenval != FECHA_PARENTESES){
		printf("Err escreva.\n");
		erro(nomeArquivo, atual(), "Token esperado ')'.");
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
		printf("Err leia.\n");
		erro(nomeArquivo, verProximo(), "Token esperado '('.");
		return NULL;
	}

	insere_filho(leia, novo_node(atualEAvanca(), -1));	// inserindo o filho "("
	insere_filho(leia, var());							// inserindo uma variavel

	if(atual()->tokenval != FECHA_PARENTESES){
		printf("Err leia..\n");
		erro(nomeArquivo, atual(), "Token esperado ')'.");
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
		erro(nomeArquivo, atual(), "Token esperado: 'FIM'.");
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
		erro(nomeArquivo, atual(), "Token esperado: 'ENTÃO'.");
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
		erro(nomeArquivo, atual(), "Token esperado: 'FIM'.");
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
			printf("Err acao: ");
			printToken(atual(), 0, 0);
			erro(nomeArquivo, atual(), "Token inesperado.");
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
		printf("Err parametro\n");
		erro(nomeArquivo, atual(), "Token esperado ':'.");
		return parametro;
	}

	insere_filho(parametro, novo_node(atual(), -1));	// insere o ":"

	if(!verificaEAvanca(ID, TRUE)){
		printf("Err parametro.\n");
		erro(nomeArquivo, verProximo(), "Token esperado 'ID'.");
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
			printf("Err lista_parametros\n");
			erro(nomeArquivo, atual(), "Token esperado: 'INTEIRO' ou 'FLUTUANTE'.");
			exit(1);
			return NULL;
		}

		insere_filho(lista_parametros, parametro());

		if(atual()->tokenval == FECHA_PARENTESES) {
			return lista_parametros;
		}

		if(atual()->tokenval != VIRGULA) {
			printf("Err lista_parametros.\n");
			erro(nomeArquivo, atual(), "Token esperado: ',' ou ')'.");
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
		erro(nomeArquivo, verProximo(), "Token esperado: '('.");
		return NULL;
	}

	insere_filho(cabecalho, novo_node(atualEAvanca(), -1));	// insere como filho o "("
	insere_filho(cabecalho, lista_parametros());			// insere como filho a lista_parametros

	if(atual()->tokenval != FECHA_PARENTESES){
		printf("Err cabecalho.\n");
		erro(nomeArquivo, atual(), "Token esperado: ']'.");
		return NULL;
	}

	insere_filho(cabecalho, novo_node(atualEAvanca(), -1));	// insere como filho o ")"
	insere_filho(cabecalho, corpo());						// insere como filho o corpo

	if(atual()->tokenval != FIM){
		printf("Err cabecalho..\n");
		erro(nomeArquivo, atual(), "Token esperado: 'FIM'.");
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
			erro(nomeArquivo, atual(), "Token esperado: ']'.");
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
			printf("Err lista_variaveis\n");
			erro(nomeArquivo, verProximo(), "Token esperado 'ID'.");
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
		erro(nomeArquivo, verProximo(), "Token esperado: ':'.");
		return NULL;
	}

	insere_filho(declaracao_variaveis, novo_node(atual(), -1));				// insere o ":" como filho de declaracao_variaveis

	if(!verificaEAvanca(ID, TRUE)){
		printf("Err declaracao_variaveis.\n");
		erro(nomeArquivo, verProximo(), "Token esperado: 'ID'.");
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
    			erro(nomeArquivo, verProximo(), "Token esperado: ':' ou 'ID'.");
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
				erro(nomeArquivo, verProximo(), "Token esperado: '(' ou ':='.");
    		}

    		break;

    	default:
    		printf("Err declaracao..\n");
			erro(nomeArquivo, atual(), "Token esperado: 'ID', 'INTEIRO' ou 'FLUTUANTE'.");
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