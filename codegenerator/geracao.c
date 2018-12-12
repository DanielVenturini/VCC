#include "geracao.h"
#include <stdio.h>
#include <llvm-c/BitWriter.h>
#include <llvm-c/Core.h>

// valores globais, representando o top-level
LLVMContextRef contextoGlobal;
LLVMModuleRef moduleGlobal;
LLVMBuilderRef builderGlobal;

TreeNode *funcaoAtual;

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

		if(var->tipoExpressao == INTEIRO)
			LLVMSetInitializer(*varLLVM, LLVMConstInt(tipo, 0, 0));
		else
			LLVMSetInitializer(*varLLVM, LLVMConstReal(tipo, 0.0));

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
		LLVMBuildStore(builderGlobal, LLVMConstInt(tipo, 0, 0), *varLLVM);		// penúltimo parâmetro é false

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


// cria um bloco exit: no fim da função
// cria uma label para saltar para este bloco
// se for vazia a função, cria o bloco unreachable
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
		valorRetorno = LLVMConstReal(LLVMVoidType(), 0);			// Cria um valor zero para colocar no retorno.
		tipoFuncaoRetorno = &LLVMVoidType;
		pos = 0;
	}

	// recupera a função criada
	LLVMValueRef *funcao = (LLVMValueRef *) noFuncao->llvmValueRef;

	// Declara o bloco de saída.
	LLVMBasicBlockRef exitFunc = NULL;
	LLVMValueRef returnVal = NULL;

	// se for VOID, não cria tipo de retorno
	if(pos) {
		// Cria o valor de retorno e inicializa com zero.
		returnVal = LLVMBuildAlloca(builderGlobal, tipoFuncaoRetorno(), "retorno");
		LLVMBuildStore(builderGlobal, valorRetorno, returnVal);
		// cria um bloco exit, pois a função possui retorna()
		exitFunc = LLVMAppendBasicBlock(*funcao, "ret");
	} else {
		// cria um bloco unreachable
		exitFunc = LLVMAppendBasicBlock(*funcao, "unreachable");
	}

	// Cria um salto para o bloco de saída.
	LLVMBuildBr(builderGlobal, exitFunc);

	// Adiciona o bloco de saída.
	LLVMPositionBuilderAtEnd(builderGlobal, exitFunc);

	// Cria o return
	if(pos)
		LLVMBuildRet(builderGlobal, LLVMBuildLoad(builderGlobal, returnVal, "retorno"));
	else
		LLVMBuildRetVoid(builderGlobal);	// Cria um return vazio
}


void geraAtribuicao(TreeNode *node) {

	// nó que vai receber o valor
	TreeNode *var = node->filhos[0];
	TreeNode *expressao = node->filhos[1];

	// sempre procura nos escopos superiores, para poder usar variáveis globais
	Identificador *id = contem((TabSimb *) var->escopo, (char *) var->token->val, 1, isFuncao(expressao));

	LLVMValueRef *resp = (LLVMValueRef *) id->llvmValueRef;
	LLVMValueRef *exp = (LLVMValueRef *) expressao->llvmValueRef;

	// atribui com o tipo certo
	if(expressao->tipoExpressao == INTEIRO)
		LLVMBuildStore(builderGlobal, *exp, *resp);
	else
		LLVMBuildStore(builderGlobal, *exp, *resp);
}


// apenas cria um número do LLVM e insere no nó
void geraNumero(TreeNode *node) {

	LLVMValueRef *num = (LLVMValueRef *) malloc(sizeof(LLVMValueRef));
	if(node->tipoExpressao == INTEIRO)
		*num = LLVMConstInt(LLVMIntType(32), *(int *) node->token->val, 0);
	else
		*num = LLVMConstReal(LLVMFloatType(), 0.78);

	node->llvmValueRef = (void *) num;
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

		default:
			printf("");
	}

	unsigned char i;
	for(i = 0; node->filhos[i]; i ++) {
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

		default:
			printf("");
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