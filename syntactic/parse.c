#include "parse.h"
#include "../lexical/varredura.h"

TreeNode *lista_argumentos(void) {

}

TreeNode *chamada_funcao(void) {


}

TreeNode *numero(void) {


}

TreeNode *fator(void) {


}

TreeNode *operador_multiplicacao(void) {


}

TreeNode *operador_logico(void) {

}


TreeNode *operador_soma(void) {


}

TreeNode *operador_relacional(void) {

	tokenAtual = getToken();// lê o próximo token

	switch(tokenAtual->tokenval) {

		case MENOR:
		case MAIOR:
		case IGUALDADE:
		case DIFERENTE:
		case MENOR_IGUAL:
		case MAIOR_IGUAL:
			0;				// apenas porque uma declaração - linha seguinte - não é considerado um statement em c
			TreeNode *node = novo_node(tokenAtual);
			printf("Aqui Ooh\n");
			break;

		default:			// marca não reconhecida
			printf("Syntax error at line %d: %s.\n", tokenAtual->numline, (char *)tokenAtual->val);
	}

}

TreeNode *expressao_unaria(void) {


}

TreeNode *expressao_multiplicativa(void) {

}

TreeNode *expressao_aditiva(void) {

}

TreeNode *expressao_simples(void) {


}

TreeNode *expressao_logica(void) {

}

TreeNode *expressao(void) {


}

TreeNode *retorna(void) {

}

TreeNode *escreva(void) {


}

TreeNode *leia(void) {

}

TreeNode *atribuicao(void) {


}


TreeNode *repita(void) {


}

TreeNode *se(void) {


}

TreeNode *acao(void) {

}

TreeNode *corpo(void) {


}

TreeNode *parametro(void) {

}

TreeNode *lista_parametros(void) {


}

TreeNode *cabecalho(void) {


}

TreeNode *declaracao_funcao(void) {


}

TreeNode *tipo(void) {

}

TreeNode *indice(void) {


}

TreeNode *var(void) {


}

TreeNode *lista_variaveis(void) {


}

TreeNode *inicializacao_variaveis(void) {

}

TreeNode *declaracao_variaveis(void) {

}

TreeNode *declaracao(void) {

}

TreeNode *lista_declaracao(void) {

}

TreeNode *programa(void) {
	return lista_declaracao();
}

TreeNode *parse(void) {
	TreeNode *raiz = programa();
}