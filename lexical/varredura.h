#ifndef _VARREDURA_H_
#define _VARREDURA_H_

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

/// dados que serão usados para processar os tokens
#define FALSE 0
#define TRUE 1

#define SIZE_NUM 10				/// quantidade máxima - inicialmente - de dígitos em um numero
#define SIZE_IDENT 21			/// quantidade máxima - inicialmente - de caracteres em uma string

#define EOFU 255				/// EOF Unsigned

/********************************************************************
*                   ESTRUTURAS NECESSÁRIAS                          *
********************************************************************/

typedef enum {                  /// palavras reservadas e símbolos
    SE, ENTAO, SENAO, FIM, REPITA, FLUTUANTE, RETORNA, ATE, LEIA, ESCREVA, INTEIRO, SOMA, SUBTRACAO, MULTIPLICACAO, DIVISAO, IGUALDADE, VIRGULA, NUM_I, NUM_F, ID, ATRIBUICAO, MENOR, MAIOR, MAIOR_IGUAL, MENOR_IGUAL, DIFERENTE, ABRE_COLCHETES, FECHA_COLCHETES, ABRE_PARENTESES, FECHA_PARENTESES, DOIS_PONTOS, E_LOGICO, OU_LOGICO, NEGACAO, NAO_IDENTIFICADO
} TokenType;

typedef struct {                /// tokens propriamente ditos

    TokenType tokenval;			/// tipo do token
    unsigned short int numline;	/// número da linha que este token está. Permite arquivos de 0 até 65,535 linhas
    unsigned char numcaracter;	/// número do caracter na linha especificada
    void *val;					/// posição deste na memoria. Foi escolhido um ponteiro void para guardar o endereço de qualquer valor na memória

} TokenRecord;

/********************************************************************
*                   	DEFINICAO DAS FUNÇÕES                       *
********************************************************************/
/// estas são as funções que serão usadas fora do arquivo varredura.c
void *desaloca(void *ptr);		/// desaloca a 'ptr' memória usando uma thread
char openFile(char *filename);	/// realiza as operações de abertura do arquivo
TokenRecord* getToken(void);	/// retorna um token
void printToken(TokenRecord *token, char printLine, char printCaracter);	/// imprime o valor de um token

#endif