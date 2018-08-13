#ifndef STDIO_H
    #include <stdio.h>
#endif // STDIO_H

#ifndef STDLIB_H
    #include <stdlib.h>
#endif // STDLIB_H

#define FALSE 0
#define TRUE 1

/********************************************************************
*                   ESTRUTURAS NECESSARIAS                          *
********************************************************************/
char currentCharacter = ' ';    /// salvar o caracter que nao deveria ter sido lido - verificacao a frente
FILE *fileReader;               /// ponteiro para leitura do arquivo

typedef enum {                  /// palavras reservadas e simbolos
    SE, ENTAO, SENAO, ENQUANTO, MAIS, MENOS, NUM, ID, ATTR
} TokenType;

typedef struct {                /// tokens propriamente ditos

    TokenType tokenval;
    union {
        char *stringval;
        int numval;
    } attribute;

} TokenRecord;

/********************************************************************
*                   IMPLEMENTACAO DAS FUNCOES                       *
********************************************************************/
int openFile(char *filename){
    fileReader = fopen(filename, "r");

    if(fileReader == NULL){
        fprintf(stderr, "ERR: arquivo '%s' nao existe.\n", filename);
        return 1;
    }

    return 0;
}

char* getCharacter(){

    char *c = malloc(sizeof(char));

    if(currentCharacter != ' '){    /// se ja tiver um caracter que nao foi processado pela getToken(void)
        *c = currentCharacter;
        currentCharacter = ' ';

        return c;
    }

    *c = getc(fileReader);

    return c;
}

/// estes sao os possiveis tokens para processar
/// eles sao usados no estado inicial para saber qual sera o token a processar
#define INICIAL 0
#define ESPACO 1        /// ' '
#define NUMERO 2        /// '123.213'
#define IDENTIFICADOR 3 /// 'string' pode ser uma variavel ou palavra reservada
#define ATRIBUICAO 4    /// ':='
#define UNICO 5         /// '+' '-' '*' '/' '[' ']' '<' '=' '>' estes sao unicos e seus valores sao seus significados
#define ERRO 6
#define FIM -1

TokenRecord* getToken(void){

    char *c;                    /// cada caracter lido
    char *resp;                /// o resultado do token
    int finishToken = FALSE;    /// se terminou de ler o token
    int currentToken = INICIAL; /// o token atual de processamento
    TokenRecord *token;         /// o token propriamente dito

    while(!finishToken){
        c = getCharacter();
        printf("\n");

        if(*c == FIM){
            currentToken = FIM;
        }

        /// usando switch duplamente aninhado:
        /// o primeiro nivel diz respeito ao que esta processando
        /// o segundo nivel diz respeito ao estado de processamento do token atual

        recomputaSwitch:
        switch(currentToken){

            case INICIAL:               /// primeira vez, entao ve qual sera o proximo a processar
                                        /// verifica com a tabela ascii
                if(*c == 32){
                    currentToken = ESPACO;
                } else if (*c >= 48 && *c <= 57){
                    currentToken = NUMERO;
                } else if ( (*c >= 65 && *c <= 90) || (*c >= 97 && *c <= 122) ){
                    currentToken = IDENTIFICADOR;
                } else if (*c == 58){
                    currentToken = ATRIBUICAO;
                } else if (*c == 42 || *c == 43 || *c == 45 || *c == 47 || *c == 60 || *c == 61 || *c == 62 || *c == 91 || *c == 93){
                    /// estes sao: * + - / < = > [ ]
                    currentToken = UNICO;
                }

                goto recomputaSwitch;

            case NUMERO:    /// este estado le ate o final do numero
                resp = (char*) malloc(7*sizeof(char));  /// numeros de 0 ate 999999
                char i;

                for(i = 0; i < 6; i ++){        /// le no maximo 6 digitos

                    resp[i] = *c;               /// adiciona o numero na resposta
                    printf("%c ", resp[i]);
                    c = getCharacter();         /// le o proximo

                    if(*c < 48 || *c > 57){     /// nao eh um digito
                        break;                  /// termina este for
                    }
                }

                finishToken = TRUE;     /// termina de ler
                resp[i+1] = '\0';       /// finaliza a representacao do numero no resposta
                currentCharacter = *c;  /// nao processa o caracter atual
                free(c);                /// libera a memoria do caracter

                token = (TokenRecord*) malloc(sizeof(TokenRecord)); /// cria o token
                token->tokenval = NUM;                              /// diz que ele eh numero
                token->attribute.numval = atoi(resp);               /// guarda o valor

                break;

            case IDENTIFICADOR:
                printf("identificador: %c, %d\n", *c, *c);
                currentToken = INICIAL;
                break;

            case ATRIBUICAO:
                printf("atribuicao: %c\n", *c);                currentToken = INICIAL;
                break;

            case UNICO:
                printf("unico: %c, %d\n", *c, *c);
                currentToken = INICIAL;
                break;

            case ESPACO:    /// este espaco ja foi lido, entao retorna para o estado inicial
                printf("espaco");
                currentToken = INICIAL;
                break;

            case FIM:    /// EOF
                finishToken = TRUE;
                break;
        }
    }

    return token;
}

int main(int argc, char *argv[]){
    if(argc != 2){
        fprintf(stderr, "Use: vcc file.tpp");
        return 1;
    }

    if(openFile(argv[1])){      /// erro ao abrir arquivo
        return 1;
    }

    TokenRecord *token = getToken();
    printf("Token encontrado: %d", token->attribute.numval);
    token = getToken();
    printf("Token encontrado: %d", token->attribute.numval);

    return 0;
}

