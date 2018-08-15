#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
/// dados que serao usados para processar os tokens

#define FALSE 0
#define TRUE 1

#define SIZE_NUM 10				/// quantidade maxima - inicialmente - de digitos em um numero
#define SIZE_IDENT 21			/// quantidade maxima - inicialmente - de caracteres em uma string

/********************************************************************
*                   ESTRUTURAS NECESSARIAS                          *
********************************************************************/

typedef enum {                  /// palavras reservadas e simbolos
    SE, ENTAO, SENAO, REPITA, MAIS, MENOS, INTEIRO, FLOAT, MULTIPLICACAO, DIVISAO, NUM_I, NUM_F, ID, ATRIBUICAO, MENOR, MAIOR, IGUAL, ABRE_COLCHETES, FECHA_COLCHETES, ABRE_PARENTESES, FECHA_PARENTESES, VIRGULA, DOIS_PONTOS
} TokenType;

typedef struct {                /// tokens propriamente ditos

    TokenType tokenval;			/// tipo do token
    void *val;					/// sua posicao na memoria

} TokenRecord;

/********************************************************************
*                   	DEFINICAO DAS FUNCOES                       *
********************************************************************/

void *desaloca(void *ptr);		/// desaloca a 'ptr' memoria usando uma thread
char openFile(char *filename);	/// realiza as operacoes de abertura do arquivo
char* getCharacter();    		/// le um unico caracter do arquivo ou retorna um que ja foi lido mas nao processado
TokenRecord* getToken(void);    /// retorna um token
