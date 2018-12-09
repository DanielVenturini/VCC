#ifndef _TAB_SIMB_H_
#define _TAB_SIMB_H_

#include <string.h>
#include "../tree/tree.h"
#include "../desacerto.h"

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
	char declarada;	// se a variável foi declarada, visto que é adicionado na tabela mesmo se não for declarado
	char retornou;	// se a função retornou algum valor
	int *indices;	// se houver índices. Cada posição representa uma dimensão, e cada valor representa o tamanho máximo daquela dimensão
	TokenType *parametros;			// lista dos parâmetros, caso seja uma função
	struct Identificador *proximo;	// proximo identificador do escopo
	TokenRecord *token;				// o token deste identificador, pois se precisar printar erro
	/* USADO PELO GERADOR DE CÓDIGO */
	void *llvmValueRef;				// representa a variável no llvm

} Identificador;

typedef struct {

	struct TabSimb *escopoSuperior;	// o escopo superior, que tem as demais variáveis que são acessíveis
	struct TabSimb *escoposInferior;// o escopo inferior
	struct TabSimb *proximo;		// o próximo escopo na lista inferior
	Identificador *declarados;		// lista das variáveis declaradas neste escopo
	unsigned char numEscopo;		// numero deste escopo
	char *nome;						// nome do escopo: 'nome de função', se, senao, enquanto

} TabSimb;

/********************************************************************
*                   	DEFINICAO DAS FUNÇÕES                       *
********************************************************************/

Identificador *contem(TabSimb *, char *, char, char);	// retorna o Identificador se este estiver na tabela de símbolos
TabSimb *criaTabSim(TabSimb *);							// cria a tabela de símbolos e aponta para o escopo superior e atribui um nome
TokenType getTipo(TabSimb *, char *);					// retorna o tipo de um identificador da tabela
void printEscopo(TabSimb *, char);						// printa a tabela de símbolos
Identificador *insere_escopo(TabSimb *, TreeNode *, char, char *);	// insere o token do nó na tabela de símbolos

#endif