#ifndef _TREE_H_
#define _TREE_H_

#include "../lexical/varredura.h"
#include "stack.h"

#define NUM_INICIAL_FILHOS  4		// aloca este número de ponteiros; se tiver mais que 5 filhos, realoca

/********************************************************************
*                   ESTRUTURAS NECESSÁRIAS                          *
********************************************************************/
struct TreeNode;

typedef enum {                  /// tipo dos nós da árvore de acordo com a BNF
    PROGRAMA, LISTA_DECLARACOES, DECLARACAO, DECLARACAO_VARIAVEIS, INICIALIZACAO_VARIAVEIS, LISTA_VARIAVEIS, VAR, INDICE, TIPO,
    DECLARACAO_FUNCAO, CABECALHO, LISTA_PARAMETROS, PARAMETRO, CORPO, ACAO, B_SE, B_REPITA, B_ATRIBUICAO, B_LEIA, B_ESCREVA,
    B_RETORNA, EXPRESSAO, EXPRESSAO_LOGICA, EXPRESSAO_SIMPLES, EXPRESSAO_ADITIVA, EXPRESSAO_MULTIPLICATIVA, EXPRESSAO_UNARIA,
    OPERADOR_RELACIONAL, OPERADOR_SOMA, OPERADOR_LOGICO, OPERADOR_MULTIPLICACAO, OPERADOR_NEGACAO, FATOR, NUMERO, CHAMADA_FUNCAO,
    LISTA_ARGUMENTOS, VAZIO
} EBNFType;

typedef struct TreeNode {
	TokenRecord *token;			// o token deste nó
	char max_filhos;			// quantidade de filhos máxima sem realocar; necessária para o realloc
	struct TreeNode **filhos;	// o vetor para cada um dos filhos
	EBNFType bnfval;			// tipo do nó de acordo com a BNF
	/* USADO PELO ANALISADOR SEMÂNTICO */
	TokenType tipoExpressao;	// o tipo desta expressão.
	/* USADO PELO GERADOR DE CÓDIGO */
	void *escopo;				// o escopo onde está este nó; não pode ser TabSimb pois gerará erro de include e ifndef
} TreeNode;

/********************************************************************
*                   	DEFINICAO DAS FUNÇÕES                       *
********************************************************************/

TreeNode *novo_node(TokenRecord *token, EBNFType bnfval);	// apenas aloca um novo nó e retorna
TreeNode *remove_filho(TreeNode *pai);						// remove o último filho e o retorna
void insere_filho(TreeNode *pai, TreeNode *filho);			// dado o pai, insere o filho na próximo local
// analise diz respeito ao tipo da arvore da analise, no caso, sintática ou semântica
// para adicionar um '.ax' ou '.as' no fim do nome do arquivo passado para o xdot, caso as duas árvores seja
// exibidas ao mesmo tempo
void printArvoreX(TreeNode *raiz, char *nomeArquivo, char analise);	// a partir deste nó, printa a árvore no xdot
void printArvoreT(TreeNode *raiz);									// a partir deste nó, printa a árvore no terminal

#endif