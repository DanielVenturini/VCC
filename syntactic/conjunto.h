#include "../lexical/varredura.h"

#ifndef CONJUNTO_H
#define CONJUNTO_H

#define MAX_CONJUNTO 9

typedef struct {
	unsigned char tamanho;	// tamanho total
	unsigned char pos;		// próxima posição vazia
	TokenType *elementos;	// cada um dos elementos
} Conjunto;

/********************************************************************
*                   	DEFINICAO DAS FUNÇÕES                       *
********************************************************************/

Conjunto *criaConjunto();
Conjunto *uniao(Conjunto *conjunto1, Conjunto *conjunto2);
char contem(Conjunto *conjunto, TokenType marca, char incluir);	// incluir a marca EOFU? isso para evitar sempre ficar criando um novo conjunto
void adiciona(Conjunto *conjunto, TokenType marca);

#endif