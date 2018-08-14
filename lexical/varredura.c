#ifndef STDIO_H
    #include <stdio.h>
#endif // STDIO_H

#ifndef STDLIB_H
    #include <stdlib.h>
#endif // STDLIB_H

#include "varredura.h"

char *currentCharacter = NULL;  /// salvar o caracter que nao deveria ter sido lido - verificacao a frente
FILE *fileReader;               /// ponteiro para leitura do arquivo

/********************************************************************
*                   IMPLEMENTACAO DAS FUNCOES                       *
********************************************************************/
void emptyStatement(void){/*do nothing*/}

/// para evitar realocar sempre, somente sera realocado
/// se o 'size_data' for maior que a metade do SIZE_[NUM|IDENT]
int realoca(char *c, char i, char size_max){

    printf("\nREALOCANDO\n");
    printf("Tamanho maximo atual: %d.\n", size_max);
    printf("Tamanho atual: %d.\n", i);
    char new_size = i + (int) size_max-1;            /// novo tamanho sera o tamanho atual mais o tamanho maximo inicial menos 1
    printf("Tamanho maximo atualizado: %d.\n", new_size);
    c = (char *) realloc(c, new_size*sizeof(char)); /// realoca
    return new_size;                                /// retorna o novo tamanho
}

char openFile(char *filename){
    fileReader = fopen(filename, "r");

    if(fileReader == NULL){
        fprintf(stderr, "ERR: arquivo '%s' nao existe.\n", filename);
        return '1';
    }

    return '0';
}

char* getCharacter(){

    char *c = malloc(sizeof(char));

    if(currentCharacter != NULL){    /// se ja tiver um caracter que nao foi processado pela getToken(void)
        c = currentCharacter;
        currentCharacter = NULL;

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
#define UNICO 5         /// '+' '-' '*' '/' '[' ']' '<' '=' '>' '[' ']' '(' ')' estes sao unicos e seus valores sao seus significados
#define COMENTARIO 6

TokenRecord* getToken(void){

    char *c;                    /// cada caracter lido
    char *resp;                /// o resultado do token
    int finishToken = FALSE;    /// se terminou de ler o token
    int currentToken = INICIAL; /// o token atual de processamento
    TokenRecord *token;         /// o token propriamente dito

    while(!finishToken){
        c = getCharacter();

        if(*c == EOF){
            currentToken = EOF;
        }

        /// usando switch duplamente aninhado:
        /// o primeiro nivel diz respeito ao que esta processando
        /// o segundo nivel diz respeito ao estado de processamento do token atual

        char i;
        recomputaSwitch:
        switch(currentToken){

            case INICIAL:               /// primeira vez, entao ve qual sera o proximo a processar
                if (*c >= '0' && *c <= '9'){
                    currentToken = NUMERO;
                } else if ( (*c >= 'A' && *c <= 'Z') || (*c >= 'a' && *c <= 'z') ){
                    currentToken = IDENTIFICADOR;
                } else if (*c == '*' || *c == '+' || *c == '-' || *c == '/' || *c == ':' || *c == '<' || *c == '=' || *c == '>' || *c == '[' || *c == ']' || *c == '(' || *c == ')'){
                    currentToken = UNICO;
                } else if (*c == ' ' || *c == 13 || *c == 10 || *c == '\t') {
                    break;   /// entao volta para o comeco do laco e le o proximo caracter
                } else if (*c == '{') {
                    currentToken = COMENTARIO;
                } else if (*c == EOF){
                    currentToken = EOF;
                } else {
                    currentToken = EOF;
                    printf("CARACTER INVALIDO: %d\n", *c);
                }

                /// quando sai do case inicial, volta para o 'switch(currentToken)' e pega o proximo token
                goto recomputaSwitch;

            case NUMERO:    /// este estado le ate o final do numero
                emptyStatement();                               /// nao faz nada, apenas o 'int size_...' - declaracao - nao pode ser o primeiro
                char size_num = SIZE_NUM;                       /// se precisar realocar mais espaco, sera incrementado o size_num
                resp = (char*) malloc(size_num*sizeof(char));   /// numeros inicialmente de 0 ate 9_999_999_999 sem virgula
                char isFloat = FALSE;                           /// se ira transformar em numero com o atof() ou atoi()

                for(i = 0; i < size_num; i ++){                 /// le no maximo 10 digitos

                    resp[i] = *c;                               /// adiciona o numero na resposta
                    c = getCharacter();                         /// le o proximo

                    if(i == size_num-1){                        /// se precisar realocar
                        size_num = realoca(resp, i, size_num);
                    }

                    if(*c == '.'){                              /// eh float
                        isFloat = TRUE;                         /// marca que eh float
                        resp[++i] = *c;                         /// adiciona o ponto
                        break;

                    } else if(*c < '0' || *c > '9'){            /// nao eh um digito
                        break;                                  /// termina este for
                    }
                }

                if(isFloat){

                    do{

                        c = getCharacter();
                        if(*c < '0' || *c > '9'){               /// nao eh um digito
                            break;                              /// termina este for
                        }

                        if(i++ == size_num-1){                  /// se precisar realocar
                            size_num = realoca(resp, i, size_num);
                        }

                        resp[i] = *c;
                    } while(i < size_num);
                }

                finishToken = TRUE;                                     /// termina de ler
                resp[i+1] = '\0';                                       /// finaliza a representacao do numero no resposta
                currentCharacter = c;                                   /// nao processa o caracter atual
                token = (TokenRecord*) malloc(sizeof(TokenRecord));     /// cria o token
                token->tokenval = NUM;                                  /// diz que ele eh numero
                token->attribute.numval = isFloat?atof(resp):atoi(resp);/// se tiver sido guardado um valor float, chama a atof(); senao, a atoi()
                free(resp);                                             /// desaloca a memoria

                break;

            case IDENTIFICADOR:
                emptyStatement();
                char size_ident = SIZE_IDENT;
                token = (TokenRecord*) malloc(sizeof(TokenRecord));                     /// cria o token
                token->tokenval = ID;                                                   /// diz que ele eh identificador
                token->attribute.stringval = (char*) malloc(size_ident*sizeof(char));   /// letras de ate size_ident caracteres

                for(i = 0; i < size_ident; i ++){                               /// le no maximo size_ident caracteres

                    token->attribute.stringval[i] = *c;                         /// adiciona o numero na resposta
                    c = getCharacter();                                         /// le o proximo

                    if( ((*c < 'A' || *c > 'Z') && (*c < 'a' || *c > 'z')) &&   /// nao eh um digito
                        (*c < '0' || *c > '9')  && *c != '_'){                  /// nao eh um numero nem '_'
                        break;                                                  /// termina este for
                    }

                    if(i == size_ident-1){                                      /// se precisar realocar
                        size_ident = realoca(resp, i, size_ident/1.5);
                    }
                }

                finishToken = TRUE;                     /// termina de ler
                token->attribute.stringval[i+1] = '\0'; /// finaliza a representacao do numero no resposta
                currentCharacter = c;                   /// nao processa o caracter atual

                break;

            case UNICO:     /// estes sao: * + - / : < = > [ ]
                token = (TokenRecord*) malloc(sizeof(TokenRecord));
                switch(*c){
                    case '+':
                        token->tokenval = SOMA;
                        token->attribute.stringval = c;
                        break;
                    case '-':
                        token->tokenval = SUB;
                        token->attribute.stringval = c;
                        break;
                    case '*':
                        token->tokenval = MULT;
                        token->attribute.stringval = c;
                        break;
                    case '/':
                        token->tokenval = DIV;
                        token->attribute.stringval = c;
                        break;
                    case '<':
                        token->tokenval = MENOR;
                        token->attribute.stringval = c;
                        break;
                    case '=':
                        token->tokenval = IGUAL;
                        token->attribute.stringval = c;
                        break;
                    case '>':
                        token->tokenval = MAIOR;
                        token->attribute.stringval = c;
                        break;
                    case '[':
                        token->tokenval = A_COL;
                        token->attribute.stringval = c;
                        break;
                    case ']':
                        token->tokenval = F_COL;
                        token->attribute.stringval = c;
                        break;
                    case '(':
                        token->tokenval = A_PAR;
                        token->attribute.stringval = c;
                        break;
                    case ')':
                        token->tokenval = F_PAR;
                        token->attribute.stringval = c;
                    case ':':   /// pode ser ': ou ':='
                        token->tokenval = ATTR;
                        char *nextCharacter = getCharacter();   /// eh preciso saber se o proximo eh '='

                        if(*nextCharacter == '='){
                            token->attribute.stringval = (char *) malloc(3*sizeof(char));
                            token->attribute.stringval[0] = *c;
                            token->attribute.stringval[1] = *nextCharacter;
                            token->attribute.stringval[2] = '\0';
                        } else {                                /// entao eh somento ':'
                            token->attribute.stringval = c;
                            currentCharacter = nextCharacter;
                        }
                        break;

                    default:
                        printf("OPERANDO UNICO NAO IMPLEMENTADO: %c\n", *c);
                        break;
                }

                finishToken = TRUE;                     /// termina de ler
                break;

            case COMENTARIO:
                printf("COMENTARIO IGNORADO\n");
                while( *(c = getCharacter()) != '}');   /// consumindo tudo o que esta entre o '{' e o '}'
                currentToken = INICIAL;                 /// volta para estado inicial
                break;
            case EOF:    /// EOF
                finishToken = TRUE;
                token = token = (TokenRecord*) malloc(sizeof(TokenRecord));
                token->tokenval = EOF;
                break;
        }
    }

    return token;
}