#ifndef _TREE_H_
#define _TREE_H_

#include "lexical/varredura.h"

#define NUM_INICIAL_FILHOS  4		// aloca este número de ponteiros; se tiver mais que 5 filhos, realoca

/********************************************************************
*                   ESTRUTURAS NECESSÁRIAS                          *
********************************************************************/
struct TreeNode;

typedef struct TreeNode {
	TokenRecord *token;			// o token deste nó
	char letra;
	char max_filhos;			// quantidade de filhos máxima sem realocar; necessária para o realloc
	struct TreeNode **filhos;	// o vetor para cada um dos filhos
} TreeNode;

/********************************************************************
*                   	DEFINICAO DAS FUNÇÕES                       *
********************************************************************/

TreeNode *novo_node(char letra);					// apenas aloca um novo nó e retorna
void insere_filho(TreeNode *pai, TreeNode *filho);	// dado o pai, insere o filho na próximo local
void remove_filho(TreeNode *pai, TreeNode *filho); 	// dado o pai, remove o filho no local específico e desloca para a esquerda os demais
void printArvore(TreeNode *raiz);					// a partir deste nó, printa a árvore

#endif