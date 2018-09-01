#ifndef _PARSE_H_
#define _PARSE_H_

#include "../tree.h"

TokenRecord *tokenAtual;	// guarda o token atual que está sendo processado

/********************************************************************
*                   	DEFINICAO DAS FUNÇÕES                       *
********************************************************************/

TreeNode *parse(void);		// retorna o nó raiz do programa

#endif