#include "geracao.h"
#include <stdio.h>
#include <llvm-c/BitWriter.h>
#include <llvm-c/Core.h>

// valores globais, representando o top-level
LLVMContextRef contextoGlobal;
LLVMModuleRef moduleGlobal;
LLVMBuilderRef builderGlobal;

LLVMValueRef *resolveOperando(TreeNode *no, unsigned char load);
void percorre(TreeNode *no);

TreeNode *funcaoAtual;
TabSimb *escopoGlobal;
LLVMValueRef returnVal = NULL;
LLVMBasicBlockRef exitFunc = NULL;
EBNFType tipoAnterior;
unsigned char pos;

TreeNode *root;

void salva(LLVMModuleRef module, char *fileName, char code) {

	unsigned char tam = strlen(fileName);

	char *fileNameOut = (char *) malloc((tam+3)*sizeof(char *));	// +3 é para inserir o .bc no fim
	sprintf(fileNameOut, "%s.bc", fileName);						// concatenando o nome original com '.bc'

	// Escreve para um arquivo no formato bitcode.
	if (LLVMWriteBitcodeToFile(module, fileNameOut) != 0)
		fprintf(stderr, "error writing bitcode to file, skipping\n");

	// Imprime o código do módulo.
	if(code)
		LLVMDumpModule(module);
}


// troca o nome das funções
// é preciso que haja uma função chamada 'main', e esta deve ser a função 'principal'
void resolveNome(TreeNode *programa) {

	unsigned char i;
	for(i = 0; programa->filhos[i]; i ++) {

		// so altera as funções
		if(programa->filhos[i]->bnfval != DECLARACAO_FUNCAO)
			continue;

		TreeNode *funcao = programa->filhos[i];

		// recupera a posição do nó que contém o nome da função
		TokenType tipo = funcao->filhos[0]->token->tokenval;		// INTEIRO, FLUTUANTE ou VAZIO
		unsigned char pos = (tipo == INTEIRO || tipo == FLUTUANTE) ? 1 : 0;

		char *nome = (char *) funcao->filhos[pos]->token->val;
		Identificador *id = contem(funcao->escopo, nome, 0, 1);

		char *nomeFinal = nome;

		if(!strcmp("principal", nome))	// se for a função principal, troca para main
			nomeFinal = "main";
		else if (!strcmp("main", nome))	// se houver uma função chamada main, troca para principal
			nomeFinal = "principal";

		// substitui pelo nome alterado ou mantém o nome original
		funcao->filhos[pos]->token->val = (void *) nomeFinal;
		id->nome = nomeFinal;
	}
}


LLVMTypeRef getTipoGen(TreeNode *node) {
	return (node->filhos[0]->token->tokenval) == INTEIRO ? LLVMIntType(32) : LLVMFloatType();
}


char isFuncao(TreeNode *expressao) {
	return expressao->bnfval == CHAMADA_FUNCAO;
}

void geraDecVariaveisGlobal(TreeNode *declaracao, LLVMTypeRef tipo, TreeNode *lista) {

	unsigned char i;
	for(i = 0; lista->filhos[i]; i ++) {

		TreeNode *var = lista->filhos[i];
		Identificador *id = contem((TabSimb *) var->escopo, (char *) var->token->val, 0, 0);

		LLVMValueRef *varLLVM = (LLVMValueRef *) malloc(sizeof(LLVMValueRef));
		*varLLVM = LLVMAddGlobal(moduleGlobal, tipo, (char *) id->nome);

		// necessário iniciar
		if(var->tipoExpressao == INTEIRO)
			LLVMSetInitializer(*varLLVM, LLVMConstInt(LLVMInt32Type(), 0, 0));
		else
			LLVMSetInitializer(*varLLVM, LLVMConstReal(LLVMFloatType(), 0.0));

		// common.
		LLVMSetLinkage(*varLLVM, LLVMCommonLinkage);
		// Alignment.
		LLVMSetAlignment(*varLLVM, 4);

		// guarda o valor no Identificador e no nó
		var->llvmValueRef = (void *) varLLVM;
		id->llvmValueRef = (void *) varLLVM;
	}
}


void geraDecVariaveisLocal(TreeNode *declaracao, LLVMTypeRef tipo, TreeNode *lista) {

	unsigned char i;
	for(i = 0; lista->filhos[i]; i ++) {

		TreeNode *var = lista->filhos[i];
		Identificador *id = contem((TabSimb *) var->escopo, (char *) var->token->val, 0, 0);

		LLVMValueRef *varLLVM = (LLVMValueRef *) malloc(sizeof(LLVMValueRef));
		*varLLVM = LLVMBuildAlloca(builderGlobal, tipo, (char *) lista->filhos[i]->token->val);

		LLVMSetAlignment(*varLLVM, 4);

		// guarda o valor no Identificador e no nó
		var->llvmValueRef = (void *) varLLVM;
		id->llvmValueRef = (void *) varLLVM;
	}

}


void geraDecVariaveis(TreeNode *declaracao) {

	// INTEIRO ou FLUTUANTE
	LLVMTypeRef tipo = getTipoGen(declaracao);
	// LISTA_VARIAVEIS
	TreeNode *lista = declaracao->filhos[1];

	// se estiver no escopo global
	if(!funcaoAtual)
		geraDecVariaveisGlobal(declaracao, tipo, lista);
	else
		geraDecVariaveisLocal(declaracao, tipo, lista);
}


LLVMTypeRef *geraParametros(TreeNode *parametros, unsigned char *qtdParametros) {

	if(!parametros) {
		return NULL;
	}

	// recupera a quantidade de parametros
	unsigned char i;
	for(i = 0; parametros->filhos[i]; i ++);

	*qtdParametros = i;

	// aloca um vetor de parametros
	LLVMTypeRef *paramns = (LLVMTypeRef *) malloc(i*sizeof(LLVMTypeRef));

	for(i = 0; i < *qtdParametros; i ++) {
		TreeNode *parametro = parametros->filhos[i];
		paramns[i] = getTipoGen(parametro);
	}

	return paramns;
}


// inicializa as variáveis que estão no parametro
// mas primeiro tem que criar a função, pois isso
// essa função é desacoplada da geraDecFuncao
void inicializaParametros(TreeNode *parametros, LLVMTypeRef *paramns, unsigned char qtd) {

	if(!parametros) {
		return;
	}

	// recupera a quantidade de parametros
	unsigned char i;
	for(i = 0; i < qtd; i ++) {
		TreeNode *parametro = parametros->filhos[i];

		// recuperando a função
		LLVMValueRef funcao = *(LLVMValueRef *) funcaoAtual->llvmValueRef;

		// recupera a variável, parâmetro e cria a variávelLLVM
		Identificador *id = contem((TabSimb *) parametro->escopo, (char *) parametro->filhos[1]->token->val, 0, 0);
		LLVMValueRef param = LLVMGetParam(funcao, i);
		LLVMValueRef *var = (LLVMValueRef *) malloc(sizeof(LLVMValueRef));
		*var = LLVMBuildAlloca(builderGlobal, getTipoGen(parametro), (char *) id->nome);

		LLVMSetAlignment(*var, 4);

		// inicializando a variável
		LLVMBuildStore(builderGlobal, param, *var);

		// salva a variável na tabela e no nó
		id->llvmValueRef = (void *) var;
		parametro->filhos[1]->llvmValueRef = (void *) var;
	}
}


void geraDecFuncao(TreeNode *noFuncao) {

	TokenType tipo = noFuncao->filhos[0]->token->tokenval;
	char *nome;
	unsigned int pos = 1;	// se for tipo INTEIRO ou FLUTUANTE, senão troca

	// ponteiros necessários
	LLVMTypeRef tipoRetorno = NULL;
	LLVMTypeRef retorno = NULL;

	if(tipo == INTEIRO) {
		tipoRetorno = LLVMInt32TypeInContext(contextoGlobal);	// Declara o tipo do retorno da função.
		retorno = LLVMIntType(32);								// alocando a variável de retorno
	} else if (tipo == FLUTUANTE) {
		tipoRetorno = LLVMFloatTypeInContext(contextoGlobal);	// Declara o tipo do retorno da função.
		retorno = LLVMFloatType();
	} else {	// vazio
		tipoRetorno = LLVMVoidTypeInContext(contextoGlobal);
		retorno = LLVMVoidType();
		pos = 0;
	}

	nome = (char *) noFuncao->filhos[pos]->token->val;

	// Cria a função.
	unsigned char qtdParametros = 0;

	// aloca um espaço para a função
	LLVMValueRef *funcao = (LLVMValueRef *) malloc(sizeof(LLVMValueRef));
	// já recupera a lista de parâmetros
	LLVMTypeRef *paramns = geraParametros(noFuncao->filhos[pos+1], &qtdParametros);
	*funcao = LLVMAddFunction(moduleGlobal, nome, LLVMFunctionType(tipoRetorno, paramns, qtdParametros, 0));

	// Declara o bloco de entrada e de retorno.
  	LLVMBasicBlockRef entryBlock = LLVMAppendBasicBlockInContext(contextoGlobal, *funcao, "entry");
	exitFunc = LLVMAppendBasicBlock(*funcao, "ret");

	// adiciona no nó da árvore e no identificador o seu LLVMValueRef
	Identificador *id = contem(noFuncao->escopo, nome, 0, 1);

	// aloca a posição
	id->llvmValueRef = (void *) funcao;
	noFuncao->llvmValueRef = (void *) funcao;

	// Adiciona o bloco de entrada.
	LLVMPositionBuilderAtEnd(builderGlobal, entryBlock);
	// se for void, não pode alocar: %retorno = alloca void
	if(pos)
		returnVal = LLVMBuildAlloca(builderGlobal, retorno, "retorno");	// alocando a variável de retorno
	// recupera os parametros e insere nos nós respectivos
	inicializaParametros(noFuncao->filhos[pos+1], paramns, qtdParametros);
}


// cria um bloco exit: no fim da função
// cria uma label para saltar para este bloco
// se for vazia a função, cria o bloco unreachable
void geraEndFuncao(TreeNode *noFuncao) {

	TokenType tipo = noFuncao->filhos[0]->token->tokenval;
	unsigned char pos = 0;

	if(tipo == INTEIRO || tipo == FLUTUANTE) {
		pos = 1;
	}

	// Cria um salto para o bloco de saída.
	LLVMBuildBr(builderGlobal, exitFunc);

	// Adiciona o bloco de saída.
	LLVMPositionBuilderAtEnd(builderGlobal, exitFunc);

	// Cria o return
	if(pos)
		LLVMBuildRet(builderGlobal, LLVMBuildLoad(builderGlobal, returnVal, "_end"));
	else
		LLVMBuildRetVoid(builderGlobal);	// Cria um return vazio
}


// retorna 1 se a variável for global
// se a variável for encontrada no escopo local, então não é global
char isGlobal(TreeNode *node) {
	// se encontrar no local, retorna falso
	if(contem(node->escopo, (char *)node->token->val, 0, isFuncao(node)))
		return 0;

	// se achar no escopo global, então retorna
	return contem(escopoGlobal, (char *)node->token->val, 0, isFuncao(node)) ? 1 : 0;
}


void atribuicaoNumero(TreeNode *node, LLVMValueRef *varOp, LLVMValueRef *expOp) {
	// se estiver no escopo global
	if(isGlobal(node))
		LLVMSetInitializer(*varOp, *expOp);
	else
		LLVMBuildStore(builderGlobal, *expOp, *varOp);
}


void geraAtribuicao(TreeNode *node) {

	// nó que vai receber o valor
	TreeNode *var = node->filhos[0];
	TreeNode *expressao = node->filhos[1];

	LLVMValueRef *varOp = resolveOperando(var, 0);
	LLVMValueRef *expOp = resolveOperando(expressao, 1);

	switch (expressao->bnfval) {
		// se for um número, apenas inicializa a variável
		case NUMERO:
			LLVMBuildStore(builderGlobal, *expOp, *varOp);
			break;

		// se for um var, primeiro carrega
		case VAR:
			LLVMBuildLoad(builderGlobal, *expOp, "_var");

		// se for uma var, também entrará aqui
		// qualquer outra coisa, realiza um store
		default:
			LLVMBuildStore(builderGlobal, *expOp, *varOp);
	}
}


// apenas cria um número do LLVM e insere no nó
void geraNumero(TreeNode *node) {

	LLVMValueRef *num = (LLVMValueRef *) malloc(sizeof(LLVMValueRef));

	if(node->tipoExpressao == INTEIRO)
		*num = LLVMConstInt(LLVMIntType(32), *(int *) node->token->val, 0);	// último parâmetro é false
	else
		*num = LLVMConstReal(LLVMFloatType(), *(float *) node->token->val);	// penúltimo parâmetro é false

	node->llvmValueRef = (void *) num;
}


// escolhe qual a função que será usada:
// se o operador for de soma, retorna o ADD ou o SUB
LLVMValueRef *getOperador(TreeNode *node, TokenType tipo, void *func1, void *func2) {
	if(node->token->tokenval == tipo)
		return func1;
	else
		return func2;
}


// retorna o tipo da expressão:
// se for dois inteiro ou dois flutuantes, a expressão se mantém
// se for diferente, realiza o casting para o tipo do nó que está recebendo
TokenType resolveTipo(TreeNode *no) {
	if(no->filhos[0]->tipoExpressao == no->filhos[1]->tipoExpressao)
		return no->filhos[0]->tipoExpressao;

	TreeNode *noCoercao = NULL;
	if(no->filhos[0]->tipoExpressao == INTEIRO)
		noCoercao = no->filhos[1];
	else
		noCoercao = no->filhos[0];

	// realiza a coerção
	LLVMValueRef *novoVar = (LLVMValueRef *) malloc(sizeof(LLVMValueRef));
	*novoVar = LLVMBuildAlloca(builderGlobal, LLVMIntType(32), "_coerc");
	LLVMSetAlignment(*novoVar, 4);

	// inicializando a variável
	int *valor = (int *) malloc(sizeof(int));
	*valor = *(float *) noCoercao->token->val;
	LLVMBuildStore(builderGlobal, LLVMConstInt(LLVMIntType(32), *valor, 0), *novoVar);

	// SE FOR VARIÁVEL, TALVEZ SEJA PERIGOSO AQUI
	// altera tudo o que tiver para INTEIRO
	noCoercao->tipoExpressao = INTEIRO;
	noCoercao->token->tokenval = NUM_I;
	noCoercao->llvmValueRef = novoVar;
	noCoercao->token->val = (void *) valor;
	return INTEIRO;
}


// distingue entre os tipos de operações que usam três endereços
// verifica qual será o tipo do nó da expressão
// e retorna a função para realizar a operação
void *getOperacao(TreeNode *node) {

	node->tipoExpressao = resolveTipo(node);
	switch(node->bnfval) {
		case OPERADOR_SOMA:
			// se o novo tipo for INTEIRO, então recupera a função para inteiro
			if(node->tipoExpressao == INTEIRO)
				return getOperador(node, SOMA, &LLVMBuildAdd, &LLVMBuildSub);
			else
				return getOperador(node, SOMA, &LLVMBuildFAdd, &LLVMBuildFSub);

		case OPERADOR_MULTIPLICACAO:
			// se o novo tipo for INTEIRO, então recupera a função para inteiro
			if(node->tipoExpressao == INTEIRO)
				return getOperador(node, MULTIPLICACAO, &LLVMBuildMul, &LLVMBuildSDiv);
			else
				return getOperador(node, MULTIPLICACAO, &LLVMBuildFMul, &LLVMBuildFDiv);

		case OPERADOR_RELACIONAL:
			if(node->tipoExpressao == INTEIRO)
				return &LLVMBuildICmp;
			else
				return &LLVMBuildFCmp;

		case OPERADOR_LOGICO:
			return getOperador(node, SOMA, &LLVMBuildAdd, &LLVMBuildSub);

		default:
			printf("OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO.\n");
			return NULL;
	}
}


// retorna um LLVMValueRef representando este operando
// este operando pode ser um número, função ou variável
// se precisar, realiza um load se for variável
LLVMValueRef *resolveOperando(TreeNode *no, unsigned char load) {

	if(no->bnfval == NUMERO)
		return (LLVMValueRef *) no->llvmValueRef;
	else if(no->bnfval != VAR && no->bnfval != CHAMADA_FUNCAO)
		return (LLVMValueRef *) no->llvmValueRef;

	// é uma função ou uma variável
	// então recupera o id e retorna o llvmValueRef
	LLVMValueRef *var = (LLVMValueRef *) contem(no->escopo, (char *) no->token->val, 1, isFuncao(no))->llvmValueRef;
	if(!load)
		return var;

	// se for para executar o load
	//OUTRA ABORDAGEM É NÃO USAR O MALLOC E JOGAR O RETORNO NO PRÓPRIO *var = LLVMBui...
	LLVMValueRef *varL = (LLVMValueRef *) malloc(sizeof(LLVMValueRef));
	*varL = LLVMBuildLoad(builderGlobal, *var, "_var");
	return varL;
}


LLVMIntPredicate getComparacao(TreeNode *node) {

	switch(node->token->tokenval) {

		case MAIOR:
			return LLVMIntSGT;

		case MAIOR_IGUAL:
			return LLVMIntSGE;

		case MENOR:
			return LLVMIntSLT;

		case MENOR_IGUAL:
			return LLVMIntSLE;

		case IGUALDADE:
			return LLVMIntEQ;

		default:
			return LLVMIntNE;
	}
}


LLVMValueRef geraTresEnderecosNormal(TreeNode *node, LLVMValueRef *op1, LLVMValueRef *op2) {

	// recupera a função
	LLVMValueRef (*operacao)(LLVMBuilderRef, LLVMValueRef, LLVMValueRef, const char *) = getOperacao(node);

	return operacao(builderGlobal, *op1, *op2, "temp");
}


// operador lógico é um pouco diferente dos normais
LLVMValueRef geraTresEnderecosLogico(TreeNode *node, LLVMValueRef *op1, LLVMValueRef *op2) {

	// recupera a função
	LLVMValueRef (*operacao)(LLVMBuilderRef, LLVMIntPredicate, LLVMValueRef, LLVMValueRef, const char *) = getOperacao(node);

	return operacao(builderGlobal, getComparacao(node), *op1, *op2, "se_teste_1");
}


void geraTresEnderecos(TreeNode *node) {

	TreeNode *noL = node->filhos[0];
	TreeNode *noR = node->filhos[1];

	// recupera o LLVMValueRef dos dois operando
	LLVMValueRef *op1 = resolveOperando(noL, 1);
	LLVMValueRef *op2 = resolveOperando(noR, 1);

	// executa a operacao, e guarda em uma variável temporária
	LLVMValueRef *temp = (LLVMValueRef *) malloc(sizeof(LLVMValueRef));

	switch(node->bnfval) {
		case OPERADOR_SOMA:
		case OPERADOR_LOGICO:
		case OPERADOR_MULTIPLICACAO:
			*temp = geraTresEnderecosNormal(node, op1, op2);
			break;

		// OPERADOR RELACIONAL
		default:
			*temp = geraTresEnderecosLogico(node, op1, op2);
			break;
	}

	// coloca no nó o seu LLVMValueRef
	node->llvmValueRef = (void *) temp;
}


void blocoIf(TreeNode **node, LLVMBasicBlockRef bloco, LLVMBasicBlockRef fim) {

	LLVMPositionBuilderAtEnd(builderGlobal, bloco);
	if(node)
		percorre(*node);

	// não permite chamar na volta da recursão
	*node = NULL;
	LLVMBuildBr(builderGlobal, fim);
}


void geraSe(TreeNode *node) {

	// percorre para montar a condição
	percorre(node->filhos[0]);

	LLVMValueRef *escopo = (LLVMValueRef *) funcaoAtual->llvmValueRef;

	LLVMBasicBlockRef se_verdade = LLVMAppendBasicBlock(*escopo, "se_verdade");
	LLVMBasicBlockRef se_falso = LLVMAppendBasicBlock(*escopo, "se_falso");
	LLVMBasicBlockRef se_fim = LLVMAppendBasicBlock(*escopo, "se_fim");

	// resolve a o resultado da condição
	LLVMValueRef *condicao = resolveOperando(node->filhos[0], 1);
	LLVMBuildCondBr(builderGlobal, *condicao, se_verdade, se_falso);

	blocoIf(&node->filhos[1], se_verdade, se_fim);
	blocoIf(&node->filhos[2], se_falso, se_fim);

	LLVMPositionBuilderAtEnd(builderGlobal, se_fim);
}

void geraRetorna(TreeNode *node) {

	// precisa carregar a variável para o %retorno
	// dar um br para o ret:
	// da um load no returnVal e depois um store nele mesmo
	LLVMBuildStore(builderGlobal, *resolveOperando(node->filhos[0], 1), returnVal);
	// Cria um salto para o bloco de saída.
	LLVMBuildBr(builderGlobal, exitFunc);
}

void percorre(TreeNode *node) {
	if(!node)
		return;

	// casos que precisa ser tratado antes de continuar descendo na árvore
	// pois as variávies/funções já devem ter sido declaradas antes mesmo de continuar
	switch(node->bnfval) {

		case DECLARACAO_VARIAVEIS:
			// cria os LLVMValueRef, insere na tabela de símbolos e na árvore e gera código
			geraDecVariaveis(node);
			break;

		case DECLARACAO_FUNCAO:
			// salva a função atual
			funcaoAtual = node;
			// gera o código do cabeçalho da função
			geraDecFuncao(node);
			break;

		case B_SE:
			geraSe(node);
			break;

		default:
			printf("");
	}

	unsigned char i;
	for(i = 0; node->filhos[i]; i ++) {
		pos = i;
		tipoAnterior = node->bnfval;
		TreeNode *filho = node->filhos[i];

		percorre(filho);
	}

	// há algumas regras que devem ser computadas
	// somente na volta da recursão
	// se for uma função, finaliza ela
	switch(node->bnfval) {

		case DECLARACAO_FUNCAO:
			geraEndFuncao(node);
			funcaoAtual = NULL;
			break;

		case B_ATRIBUICAO:
			geraAtribuicao(node);
			break;

		case NUMERO:
			geraNumero(node);
			break;

		// CÓDIGO DE TRÊS ENDEREÇOS
		case OPERADOR_SOMA:
		case OPERADOR_RELACIONAL:
		case OPERADOR_LOGICO:
		case OPERADOR_MULTIPLICACAO:
			geraTresEnderecos(node);
			break;

		case B_RETORNA:
			geraRetorna(node);
			break;

		default:
			printf("");
	}
}

void geraCodigo(TreeNode *programa, char *fileName, char code) {
	contextoGlobal = LLVMGetGlobalContext();
	builderGlobal = LLVMCreateBuilderInContext(contextoGlobal);
	LLVMModuleRef module = LLVMModuleCreateWithNameInContext(fileName, contextoGlobal);

	moduleGlobal = module;
	escopoGlobal = programa->escopo;

	// troca o nome da função principal para 'main'
	// se houver alguma função main, troca para 'principal'
	tipoAnterior = -1;
	resolveNome(programa);
	percorre(programa);

	salva(module, fileName, code);
}