#include "conjunto.h"

Conjunto *criaConjunto() {
	Conjunto *conjunto = (Conjunto *) malloc(sizeof(Conjunto));
	conjunto->elementos = (TokenType *) malloc(MAX_CONJUNTO*sizeof(TokenType));
	conjunto->tamanho = MAX_CONJUNTO;
	conjunto->pos = 0;

	return conjunto;
}

// dado um conjunto, realoca mais MAX_CONJUNTO para os elementos
void realocaConjunto(Conjunto *conjunto) {

	conjunto->tamanho += MAX_CONJUNTO;		// aumenta sempre de MAX_CONJUNTO
	conjunto->elementos = (TokenType *) realloc(conjunto->elementos, conjunto->tamanho*sizeof(TokenType));
}

// cria um novo conjunto a partir dos dois conjuntos
Conjunto *uniao(Conjunto *conjunto1, Conjunto *conjunto2) {

	Conjunto *merge = criaConjunto();

	unsigned short int i;
	for(i = 0; i < conjunto1->pos; i ++) {			// copia todos os elementos do primeiro conjunto
		adiciona(merge, conjunto1->elementos[i]);
	}

	for(i = 0; i < conjunto2->pos; i ++) {			// copia todos os elementos do segundo conjunto
		adiciona(merge, conjunto2->elementos[i]);
	}

	return merge;
}

// retorna 1 se contém o valor do token neste conjunto
char contem(Conjunto *conjunto, TokenType marca, char incluir) {

	if(incluir && EOFU == marca) {	// se for para incluir este elemento, então já verifica
		return 1;
	}

	unsigned char i;
	for(i = 0; i < conjunto->tamanho; i ++) {
		if(conjunto->elementos[i] == marca) {
			return 1;
		}
	}

	return 0;
}

// adiciona o valor do token no final do conjunto
void adiciona(Conjunto *conjunto, TokenType marca) {

	if(conjunto->pos == conjunto->tamanho) {
		realocaConjunto(conjunto);
	}

	conjunto->elementos[conjunto->pos ++] = marca;
}