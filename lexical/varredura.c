#ifndef STDIO_H
    #include <stdio.h>
#endif // STDIO_H

#ifndef STDLIB_H
    #include <stdlib.h>
#endif // STDLIB_H

#include "varredura.h"

char *currentCharacter = NULL;  /// salvar o caracter que nao deveria ter sido lido - verificacao a frente
FILE *fileReader;               /// ponteiro para leitura do arquivo
pthread_t *desalocador = NULL;  /// cria thread responsavel por desalocar a memoria

/********************************************************************
*                   IMPLEMENTACAO DAS FUNCOES                       *
********************************************************************/
/// apenas desaloca a memoria alocada neste ponteiro
void *desaloca(void *ptr){
    free(ptr);
}

/// para evitar realocar sempre, somente sera realocado
/// se o 'size_data' for maior que a metade do SIZE_[NUM|IDENT]
int realoca(char *c, char i, char size_max){

    printf("COMECANDO ALOCAR\n");
    char new_size = i + (int) size_max-1;            /// novo tamanho sera o tamanho atual mais o tamanho maximo inicial menos 1
    c = (char *) realloc(c, new_size*sizeof(char)); /// realoca
    printf("TERMINOU DE ALOCAR\n");
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
#define UNICO 4         /// '+' '-' '*' '/' '[' ']' '<' '=' '>' '[' ']' '(' ')' ',' estes sao unicos e seus valores sao seus significados
#define COMENTARIO 5

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

        char i;
        recomputaSwitch:
        switch(currentToken){

            case INICIAL:               /// primeira vez, entao ve qual sera o proximo a processar
                if (*c >= '0' && *c <= '9'){
                    currentToken = NUMERO;
                } else if ( (*c >= 'A' && *c <= 'Z') || (*c >= 'a' && *c <= 'z') ){
                    currentToken = IDENTIFICADOR;
                } else if (*c == '*' || *c == '+' || *c == '-' || *c == '/' || *c == ':' || *c == '<' || *c == '='
                        || *c == '>' || *c == '[' || *c == ']' || *c == '(' || *c == ')' || *c == ','){
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
                resp = (char*) malloc(SIZE_NUM*sizeof(char));   /// numeros inicialmente de 0 ate 9_999_999_999 sem virgula
                char size_num = SIZE_NUM;                       /// se precisar realocar mais espaco, sera incrementado o size_num
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

                /// transformacao do numero
                token->tokenval = isFloat?NUM_I:NUM_F;                  /// marca se eh numero inteiro ou float
                if(isFloat){
                    int *stringval = (int *) malloc(sizeof(int));
                    *stringval = atoi(resp);                            /// recupera o valor inteiro
                    token->val = (void *) stringval;                    /// guarda no string val
                } else {
                    float *stringval = (float *) malloc(sizeof(float));
                    *stringval = atof(resp);
                    token->val = (void *) stringval;
                }

                if(!desalocador){                                       /// se ainda nao tiver sido usado thread
                    desalocador = (pthread_t*) malloc(sizeof(pthread_t));
                }

                pthread_create(desalocador, NULL, desaloca, (void *)resp);   /// inicia desalocar com a thread
                break;

            case IDENTIFICADOR:
                token = (TokenRecord*) malloc(sizeof(TokenRecord));             /// cria o token
                token->tokenval = ID;                                           /// diz que ele eh identificador
                char size_ident = SIZE_IDENT;
                char *stringval = (char*) malloc(size_ident*sizeof(char));      /// letras de ate size_ident caracteres

                for(i = 0; i < size_ident; i ++){                               /// le no maximo size_ident caracteres

                    stringval[i] = *c;                                          /// adiciona o numero na resposta
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
                stringval[i+1] = '\0';                  /// finaliza a representacao do identificador
                token->val = (void *) stringval;        /// guarda o ponteiro do numero
                currentCharacter = c;                   /// nao processa o caracter atual

                break;

            case UNICO:     /// estes sao: * + - / : < = > [ ]
                token = (TokenRecord*) malloc(sizeof(TokenRecord));
                token->val = c;

                switch(*c){
                    case '+':
                        token->tokenval = MAIS;
                        break;
                    case '-':
                        token->tokenval = MENOS;
                        break;
                    case '*':
                        token->tokenval = MULTIPLICACAO;
                        break;
                    case '/':
                        token->tokenval = DIVISAO;
                        break;
                    case '<':
                        token->tokenval = MENOR;
                        break;
                    case '=':
                        token->tokenval = IGUAL;
                        break;
                    case '>':
                        token->tokenval = MAIOR;
                        break;
                    case '[':
                        token->tokenval = ABRE_COLCHETES;
                        break;
                    case ']':
                        token->tokenval = FECHA_COLCHETES;
                        break;
                    case '(':
                        token->tokenval = ABRE_PARENTESES;
                        break;
                    case ')':
                        token->tokenval = FECHA_PARENTESES;
                        break;
                    case ',':
                        token->tokenval = VIRGULA;
                        break;
                    case ':':   /// pode ser ': ou ':='
                        token->tokenval = DOIS_PONTOS;          /// a principio eh dois pontos
                        char *nextCharacter = getCharacter();   /// eh preciso saber se o proximo eh '='

                        if(*nextCharacter == '='){              /// eh um ':='
                            token->tokenval = ATRIBUICAO;
                            char *stringval = (char *) malloc(3*sizeof(char));  /// aloco o espaco necessario
                            stringval[0] = *c;                                  /// preenchenco com ':' '=' '\0'
                            stringval[1] = *nextCharacter;
                            stringval[2] = '\0';
                            token->val = (void *) stringval;    /// guardando o ponteiro no token
                        } else {                                /// entao eh somento ':'
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