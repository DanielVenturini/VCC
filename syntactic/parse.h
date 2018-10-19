#ifndef _PARSE_H_
#define _PARSE_H_

#include "../tree.h"
#include "../desacerto.h"

typedef TokenRecord TK;

TK *tokenAtual;		// guarda o token atual que está sendo processado
TK *tokenProximo;	// guarda o token próximo para verificação á frente
char *nomeArquivo;	// nome do arquivo que será passado para biblioteca de erro e warning

/********************************************************************
*                   	DEFINICAO DAS FUNÇÕES                       *
********************************************************************/

TreeNode *parse(char *aquivo, TokenRecord *token);	// retorna o nó raiz do programa

#endif