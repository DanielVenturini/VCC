#include "geracao.h"
#include <stdio.h>
#include <llvm-c/BitWriter.h>
#include <llvm-c/Core.h>

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

void geraCodigo(TreeNode *programa, TabSimb *tabela, char *fileName, char code) {
	LLVMContextRef context = LLVMGetGlobalContext();
	LLVMModuleRef module = LLVMModuleCreateWithNameInContext(fileName, context);

	salva(module, fileName, code);
}