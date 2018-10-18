#ifndef _TREE_H_
#define _TREE_H_

#include "lexical/varredura.h"

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
    LISTA_ARGUMENTOS
} EBNFType;

typedef struct TreeNode {
	TokenRecord *token;			// o token deste nó
	char max_filhos;			// quantidade de filhos máxima sem realocar; necessária para o realloc
	struct TreeNode **filhos;	// o vetor para cada um dos filhos
	EBNFType bnfval;			// tipo do nó de acordo com a BNF
} TreeNode;

/********************************************************************
*                   	DEFINICAO DAS FUNÇÕES                       *
********************************************************************/

TreeNode *novo_node(TokenRecord *token, EBNFType bnfval);		// apenas aloca um novo nó e retorna
void insere_filho(TreeNode *pai, TreeNode *filho);				// dado o pai, insere o filho na próximo local
void remove_filho(TreeNode *pai, TreeNode *filho); 				// dado o pai, remove o filho no local específico e desloca para a esquerda os demais
void printArvoreX(TreeNode *raiz, char *nomeArquivo);			// a partir deste nó, printa a árvore no xdot
void printArvoreT(TreeNode *raiz, unsigned short int identacao);// a partir deste nó, printa a árvore no terminal

#endif