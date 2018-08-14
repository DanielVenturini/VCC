#include <stdlib.h>
#include <stdio.h>
/// dados que serao usados para processar os tokens

#define FALSE 0
#define TRUE 1

#define SIZE_NUM 7
#define SIZE_IDENT 20

/********************************************************************
*                   ESTRUTURAS NECESSARIAS                          *
********************************************************************/

typedef enum {                  /// palavras reservadas e simbolos
    SE, ENTAO, SENAO, REPITA, SOMA, SUB, INTEIRO, FLOAT, MULT, DIV, NUM, ID, ATTR, MENOR, MAIOR, IGUAL, A_COL, F_COL, A_PAR, F_PAR
} TokenType;

typedef struct {                /// tokens propriamente ditos

    TokenType tokenval;
    union {
        char *stringval;
        int numval;
    } attribute;

} TokenRecord;

/********************************************************************
*                   	DEFINICAO DAS FUNCOES                       *
********************************************************************/

int openFile(char *filename);   /// realiza as operacoes de abertura do arquivo
char* getCharacter();    /// le um unico caracter do arquivo ou retorna um que ja foi lido mas nao processado
TokenRecord* getToken(void);       /// retorna um token
