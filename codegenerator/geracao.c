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
char *resolveNome(char *nome) {
	char *nomeFinal = NULL;

	if(!strcmp("principal", nome)) {	// se for a função principal, troca para main
		nomeFinal = "main";
	} else if (!strcmp("main", nome)) {	// se houver uma função chamada main, troca para principal
		nomeFinal = "principal";
	} else {
		nomeFinal = nome;
	}

	return nomeFinal;
}


void geraDecVariaveis(TreeNode *declaracao, TabSimb *tabela) {

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


LLVMTypeRef *geraParametros(TreeNode *parametros, TabSimb *tabela, unsigned char *qtdParametros) {

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

void geraDecFuncao(TreeNode *declaracao, TabSimb *tabela) {

	TokenType tipo = declaracao->filhos[0]->token->tokenval;
	char *nome;
	//TreeNode *lista;
	unsigned int pos = 1;	// se for tipo INTEIRO ou FLUTUANTE, senão troca
	LLVMBuilderRef builder = builderGlobal;

	// ponteiros necessários
	LLVMValueRef valorRetorno = NULL;
	LLVMTypeRef tipoRetorno = NULL;
	LLVMTypeRef (*tipoFuncaoRetorno)(void);
	tipoFuncaoRetorno = NULL;

	if(tipo == INTEIRO) {
		valorRetorno = LLVMConstInt(LLVMInt32Type(), 0, 0);			// Cria um valor zero para colocar no retorno.
		tipoRetorno = LLVMInt32TypeInContext(contextoGlobal);		// Declara o tipo do retorno da função.
		tipoFuncaoRetorno = &LLVMInt32Type;
	} else if (tipo == FLUTUANTE) {
		valorRetorno = LLVMConstReal(LLVMFloatType(), 0);			// Cria um valor zero para colocar no retorno.
		tipoRetorno = LLVMFloatTypeInContext(contextoGlobal);		// Declara o tipo do retorno da função.
		tipoFuncaoRetorno = &LLVMFloatType;
	} else {	// vazio
		valorRetorno = LLVMConstReal(LLVMFloatType(), 0);			// Cria um valor zero para colocar no retorno.
		//valorRetorno = LLVMVoidType();
		tipoRetorno = LLVMVoidTypeInContext(contextoGlobal);
		tipoFuncaoRetorno = &LLVMFloatType;
		pos = 0;
	}

	nome = resolveNome((char *) declaracao->filhos[pos]->token->val);

	// Cria a função.
	unsigned char qtdParametros = 0;
	LLVMValueRef funcao = LLVMAddFunction(moduleGlobal, nome, LLVMFunctionType(tipoRetorno, geraParametros(declaracao->filhos[pos+1], tabela, &qtdParametros), qtdParametros, 0));

	// Declara o bloco de entrada.
  	LLVMBasicBlockRef entryBlock = LLVMAppendBasicBlockInContext(contextoGlobal, funcao, "entry");
  	// Declara o bloco de saída.
  	LLVMBasicBlockRef endBasicBlock = LLVMAppendBasicBlock(funcao, "exit");

	// Adiciona o bloco de entrada.
	LLVMPositionBuilderAtEnd(builder, entryBlock);

	// acho que aqui vai o código do CORPO
	// acho que não

	LLVMValueRef returnVal = NULL;
	// se for VOID, não cria tipo de retorno
	if(pos) {
		// Cria o valor de retorno e inicializa com zero.
		returnVal = LLVMBuildAlloca(builder, tipoFuncaoRetorno(), "retorno");
		LLVMBuildStore(builder, valorRetorno, returnVal);
	}

	// Cria um salto para o bloco de saída.
	LLVMBuildBr(builder, endBasicBlock);

	// Adiciona o bloco de saída.
	LLVMPositionBuilderAtEnd(builder, endBasicBlock);

	if(pos) {
		// Cria o return.
		LLVMBuildRet(builder, LLVMBuildLoad(builder, returnVal, ""));
	}
}

void percorre(TreeNode *node, TabSimb *tabela) {
	if(!node)
		return;

	switch(node->bnfval) {

		case DECLARACAO_VARIAVEIS:
			//geraDecVariaveis(node, tabela);
			break;

		case DECLARACAO_FUNCAO:
			geraDecFuncao(node, tabela);
			break;

		default:
			tabela = NULL;
	}

	unsigned char i;
	for(i = 0; node->filhos[i]; i ++) {		// cada uma das declaracoes globais
		TreeNode *filho = node->filhos[i];
		percorre(filho, tabela);
	}
}


void geraCodigo(TreeNode *programa, TabSimb *tabela, char *fileName, char code) {
	contextoGlobal = LLVMGetGlobalContext();
	builderGlobal = LLVMCreateBuilderInContext(contextoGlobal);
	LLVMModuleRef module = LLVMModuleCreateWithNameInContext(fileName, contextoGlobal);

	moduleGlobal = module;

	percorre(programa, tabela);

	salva(module, fileName, code);
}