#ifndef _TAB_SIMB_H_
#define _TAB_SIMB_H_

#include "../tree/tree.h"

/********************************************************************
*                   ESTRUTURAS NECESSÁRIAS                          *
********************************************************************/

struct TabSimb;
struct Identificador;

typedef struct {

	TokenType tipo;	// tipo da variável [ -1:VAZIO | INTEIRO | FLUTUANTE]
	char *nome;		// nome propriamente dito
	char erro;		// indica se este identificador possui erro
	char funcao;	// apenas para saber se o identificador remete a uma função
	char iniciada;	// indica se a variável foi iniciada em alguma parte do código
	char utilizada;	// indica se a variável está sendo utilizada em alguma parte do código
	unsigned short int *indices;	// se houver índices
	struct Identificador *proximo;	// proximo identificador do escopo

} Identificador;

typedef struct {

	struct TabSimb *escopoSuperior;	// o escopo superior, que tem as demais variáveis que são acessíveis
	Identificador **declarados;		// lista das variáveis declaradas neste escopo
	char *nome;						// nome do escopo: 'nome de função', se, senao, enquanto

} TabSimb;

/********************************************************************
*                   	DEFINICAO DAS FUNÇÕES                       *
********************************************************************/

Identificador *contem(TabSimb *, char *, char);	// retorna o Identificador se este estiver na tabela de símbolos
TabSimb *criaTabSim(TabSimb */*, char */);			// cria a tabela de símbolos e aponta para o escopo superior e atribui um nome
TokenType getTipo(TabSimb *, char *);			// retorna o tipo de um identificador da tabela
void inserir(TabSimb *, char *);				// insere o token do nó na tabela de símbolos
void printTabSimb(TabSimb *);					// printa a tabela de símbolos

#endif