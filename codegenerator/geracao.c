#include "geracao.h"
#include <stdio.h>
#include <llvm-c/BitWriter.h>
#include <llvm-c/Core.h>

// valores globais, representando o top-level
LLVMContextRef contextoGlobal;
LLVMModuleRef moduleGlobal;
LLVMBuilderRef builderGlobal;

// essas conterão os valores atuais em um determinado instante
LLVMContextRef contextoAtual;
LLVMModuleRef moduleAtual;
LLVMBuilderRef builderAtual;

TreeNode *funcaoAtual;

void salva(LLVMModuleRef module, char *fileName, char code) {

	unsigned char tam = strlen(fileName);

	char *fileNameOut = (char *) malloc((tam+3)*sizeof(char *));	// +3 é para inserir o .bc no fim
	sprintf(fileNameOut, "%s.bc", fileName);						// concatenando o nome original com '.bc'

	// Escreve para um arquivo no formato bitcode.
	if (LLVMWriteBitcodeToFile(module, fileNameOut) != 0) {
		fprintf(stderr, "error writing bitcode to file, skipping\n");
	}

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
		TokenType tipo = funcao->filhos[0]->tipoExpressao;	// INTEIRO, FLUTUANTE ou VAZIO
		unsigned char pos = (tipo == INTEIRO || tipo == FLUTUANTE) ? 1 : 0;

		char *nome = (char *) funcao->filhos[pos]->token->val;
		Identificador *id = contem(funcao->escopo, nome, 0, 1);

		char *nomeFinal = nome;

		if(!strcmp("principal", nome)) {	// se for a função principal, troca para main
			nomeFinal = "main";
		} else if (!strcmp("main", nome)) {	// se houver uma função chamada main, troca para principal
			nomeFinal = "principal";
		}

		// substitui pelo nome alterado ou mantém o nome original
		funcao->filhos[pos]->token->val = nomeFinal;
		id->nome = nomeFinal;
	}
}


void geraDecVariaveis(TreeNode *declaracao) {

	TokenType tipo = declaracao->filhos[0]->tipoExpressao;	// INTEIRO ou FLUTUANTE
	TreeNode *lista = declaracao->filhos[1];				// LISTA_VARIAVEIS

	LLVMTypeRef tipoLista = tipo == INTEIRO ? LLVMIntType(32) : LLVMFloatType();
	unsigned char i;
	for(i = 0; lista->filhos[i]; i ++) {

		LLVMValueRef var = LLVMBuildAlloca(builderGlobal, tipoLista, (char *) lista->filhos[i]->token->val);
		LLVMSetAlignment(var, 4);
		LLVMBuildStore(builderGlobal, LLVMConstInt(tipoLista, 0, 0), var);		// penúltimo parâmetro é false
	}
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

		if(parametro->filhos[0]->token->tokenval == INTEIRO) {
			paramns[i] = LLVMInt32Type();
		} else {
			paramns[i] = LLVMFloatType();
		}
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

		// recuperando o parametro
		LLVMValueRef *var = (LLVMValueRef *) malloc(sizeof(LLVMValueRef));
		*var = LLVMGetParam(funcao, i);

		// salvando na tabela de símbolos e no nó
		Identificador *id = contem((TabSimb *) parametro->escopo, (char *) parametro->filhos[1]->token->val, 0, 0);
		id->llvmValueRef = (void *) var;
		parametro->filhos[1]->llvmValueRef = (void *) var;

		// iniciando os parametros
		// cria uma variável do tipo do parametro e inicializa com zero rhs
		// soma ao parametro para inicia-lo
		LLVMValueRef rhs;
		if(parametro->tipoExpressao == INTEIRO)
			rhs = LLVMConstInt(LLVMInt32Type(), 0, 0);
		else
			rhs = LLVMConstInt(LLVMFloatType(), 0, 0);

		// parametro propriamente dito
		LLVMValueRef lhs = LLVMGetParam(funcao, i);
		LLVMBuildAdd(builderGlobal, lhs, rhs, id->nome);
		//LLVMBuildLShr(builderGlobal, lhs, rhs, id->nome);
	}
}


void geraDecFuncao(TreeNode *noFuncao) {

	TokenType tipo = noFuncao->filhos[0]->token->tokenval;
	char *nome;
	unsigned int pos = 1;	// se for tipo INTEIRO ou FLUTUANTE, senão troca

	// ponteiros necessários
	LLVMTypeRef tipoRetorno = NULL;

	if(tipo == INTEIRO) {
		tipoRetorno = LLVMInt32TypeInContext(contextoGlobal);		// Declara o tipo do retorno da função.
	} else if (tipo == FLUTUANTE) {
		tipoRetorno = LLVMFloatTypeInContext(contextoGlobal);		// Declara o tipo do retorno da função.
	} else {	// vazio
		tipoRetorno = LLVMVoidTypeInContext(contextoGlobal);
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

	// Declara o bloco de entrada.
  	LLVMBasicBlockRef entryBlock = LLVMAppendBasicBlockInContext(contextoGlobal, *funcao, "entry");

	// adiciona no nó da árvore e no identificador o seu LLVMValueRef
	Identificador *id = contem(noFuncao->escopo, nome, 0, 1);

	// aloca a posição
	id->llvmValueRef = (void *) funcao;
	noFuncao->llvmValueRef = (void *) funcao;

	// Adiciona o bloco de entrada.
	LLVMPositionBuilderAtEnd(builderGlobal, entryBlock);

	// recupera os parametros e insere nos nós respectivos
	inicializaParametros(noFuncao->filhos[pos+1], paramns, qtdParametros);
}


void geraEndFuncao(TreeNode *noFuncao) {

	TokenType tipo = noFuncao->filhos[0]->token->tokenval;
	unsigned int pos = 1;	// se for tipo INTEIRO ou FLUTUANTE, senão troca

	// ponteiros necessários
	LLVMValueRef valorRetorno = NULL;
	LLVMTypeRef (*tipoFuncaoRetorno)(void);
	tipoFuncaoRetorno = NULL;

	if(tipo == INTEIRO) {
		valorRetorno = LLVMConstInt(LLVMInt32Type(), 0, 0);			// Cria um valor zero para colocar no retorno.
		tipoFuncaoRetorno = &LLVMInt32Type;
	} else if (tipo == FLUTUANTE) {
		valorRetorno = LLVMConstReal(LLVMFloatType(), 0);			// Cria um valor zero para colocar no retorno.
		tipoFuncaoRetorno = &LLVMFloatType;
	} else {	// vazio
		valorRetorno = LLVMConstReal(LLVMFloatType(), 0);			// Cria um valor zero para colocar no retorno.
		tipoFuncaoRetorno = &LLVMFloatType;
		pos = 0;
	}

	// recupera a função criada
	LLVMValueRef *funcao = (LLVMValueRef *) noFuncao->llvmValueRef;

	// Declara o bloco de saída.
	LLVMBasicBlockRef exitFunc = LLVMAppendBasicBlock(*funcao, "exit");

	LLVMValueRef returnVal = NULL;
	// se for VOID, não cria tipo de retorno
	if(pos) {
		// Cria o valor de retorno e inicializa com zero.
		returnVal = LLVMBuildAlloca(builderGlobal, tipoFuncaoRetorno(), "retorno");
		LLVMBuildStore(builderGlobal, valorRetorno, returnVal);
	}

	// Cria um salto para o bloco de saída.
	LLVMBuildBr(builderGlobal, exitFunc);

	// Adiciona o bloco de saída.
	LLVMPositionBuilderAtEnd(builderGlobal, exitFunc);

	if(pos) {
		// Cria o return.
		LLVMBuildRet(builderGlobal, LLVMBuildLoad(builderGlobal, returnVal, ""));
	}
}


void percorre(TreeNode *node) {
	if(!node)
		return;

	// casos que precisa ser tratado antes de continuar descendo na árvore
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

		default:
			printf("");
	}

	unsigned char i;
	for(i = 0; node->filhos[i]; i ++) {
		TreeNode *filho = node->filhos[i];
		percorre(filho);
	}

	// se for uma função, finaliza ela
	if(node->bnfval == DECLARACAO_FUNCAO) {
		geraEndFuncao(node);
		funcaoAtual = NULL;
	}
}


void geraCodigo(TreeNode *programa, char *fileName, char code) {
	contextoGlobal = LLVMGetGlobalContext();
	builderGlobal = LLVMCreateBuilderInContext(contextoGlobal);
	LLVMModuleRef module = LLVMModuleCreateWithNameInContext(fileName, contextoGlobal);

	moduleGlobal = module;

	// troca o nome da função principal para 'main'
	// se houver alguma função main, troca para 'principal'
	resolveNome(programa);
	percorre(programa);

	salva(module, fileName, code);
}