#ifndef _PARSE_H_
#define _PARSE_H_

#include "../tree.h"
#include "../desacerto.h"

TokenRecord *tokenAtual;	// guarda o token atual que está sendo processado
TokenRecord *tokenProximo;	// guarda o token próximo para verificação á frente
char *nomeArquivo;			// nome do arquivo que será passado para biblioteca de erro e warning

/********************************************************************
*                   	DEFINICAO DAS FUNÇÕES                       *
********************************************************************/

TreeNode *parse(char *aquivo);	// retorna o nó raiz do programa

#endif