#include "varredura.h"

char *caracterAtual = NULL;     /// salvar o caracter que foi lido mas não processado - verificação a frente
FILE *leitorArquivo;            /// ponteiro para leitura do arquivo
pthread_t *desalocador = NULL;  /// ponteiro da thread responsável por desalocar a memória

/********************************************************************
*                   IMPLEMENTAÇÃO DAS FUNÇÕES                       *
********************************************************************/
/// apenas desaloca a memoria alocada neste ponteiro
/// como é usado por uma thread, tem que ser void *
void *desaloca(void *ptr){
    free(ptr);
}

/// função usada para realocar memória se a letra ou o número tiver muitos caracteres.
/// para evitar realocar sempre, somente sera realocado
/// c é o ponteiro para o vetor a alocar; i é o tamanho total; size_max é o tamanho total deste dado: SIZE_NUM|SIZE_ID
char *realoca(char *c, char *i, char *size_max) {
    *size_max = (char) *i + *size_max-1;            /// novo tamanho sera o tamanho atual mais o tamanho máximo inicial menos 1
    c = (char *) realloc(c, *size_max*sizeof(char));/// realoca
    return c;
}

/// chamado diretamente da main, esta função retorna 0 se existir o arquivo ou 1 se não existir
/// o ponteiro do arquivo é guardado para ser lido caracter por caracter na função getCaracter
char openFile(char *filename) {
    leitorArquivo = fopen(filename, "r");

    if(leitorArquivo == NULL){
        fprintf(stderr, "ERR: arquivo '%s' nao existe.\n", filename);
        return '1';
    }

    return '0';
}

/// retorna um caracter que ainda não tenha sido processado
/// ou retorna um caracter direto do arquivo
char* getCaracter(){

    char *c = malloc(sizeof(char));

    if(caracterAtual != NULL){    /// se ja tiver um caracter que nao foi processado pela getToken(void)
        c = caracterAtual;
        caracterAtual = NULL;

        return c;
    }

    *c = getc(leitorArquivo);

    return c;
}

/// lê toda a sequência de digitos e guarda em 'resp'.
/// como pode ser chamada de outras funções, por exemplo, 'getFlutuante',
/// irá começar a concatenar a partir de 'i'.
/// 'size_max' é o tamanho máximo que pode ser lido sem precisar de usar um 'realloc'
/// 'c' é o dígito que ainda não foi processado
char getDecimal(char *resp, char i, char size_max, char *c) {

    while(TRUE){                            /// fica lendo até parar de ler dígitos

        if(*c < '0' || *c > '9'){           /// não é um digito
            caracterAtual = c;              /// guarda o caracter que não é número
            return i;                       /// retorna a próxima posição vazia do array
        }

        if(i % size_max == size_max-1){     /// se precisar realocar
            realoca(resp, &i, &size_max);
        }

        resp[i] = *c;                       /// adiciona o digito na resposta
        c = getCaracter();                  /// le o proximo

        i ++;
    }
}

/// lê toda a sequência de números a e guarda em 'resp'.
/// 'c' deve ser o '.' que trousse a esta função.
char getFlutuante(char *resp, char i, char size_max) {

    resp[i] = *getCaracter();                   /// consome o caractere
    char *c = getCaracter();                    /// pega o próximo

    return getDecimal(resp, i+1, size_max, c);   /// lê todo o resto de número
}

/// estes são os possíveis tokens para processar
/// eles são usados no estado inicial para saber qual será o token a processar
#define INICIAL 0
#define ESPACO 1        /// ' '
#define NUMERO 2        /// '123.213'
#define IDENTIFICADOR 3 /// 'string' pode ser uma variavel ou palavra reservada
#define UNICO 4         /// '+' '-' '*' '/' '[' ']' '<' '=' '>' '[' ']' '(' ')' ',' estes sao unicos e seus valores sao seus significados
#define LOGICO 5        /// && e ||
#define COMENTARIO 6    /// { }
#define NI 7            /// caracter não identificado

TokenRecord* getToken(void){

    char *c;                    /// cada caracter lido
    char *resp;                 /// o resultado do token - este será usado para guardar somente dos tokens id ou numero.
    int finishToken = FALSE;    /// se terminou de ler o token
    int tokenAtual = INICIAL;   /// o token atual de processamento
    TokenRecord *token;         /// o token propriamente dito

    while(!finishToken){
        c = getCaracter();      /// lê o caracter

        if(*c == EOF){          /// se já chegou no fim do arquivo
            tokenAtual = EOF;   /// retorna um token EOF
        }

        char i;                 /// usado nos cases de tokens de números e identificadores
        recomputaSwitch:        /// label do goto para nao precisar ler caracter novamente
        switch(tokenAtual){

            case INICIAL:       /// primeira vez, entao ve qual sera o proximo a processar
                if (*c >= '0' && *c <= '9'){
                    tokenAtual = NUMERO;
                } else if ( (*c >= 'A' && *c <= 'Z') || (*c >= 'a' && *c <= 'z') ){
                    tokenAtual = IDENTIFICADOR;
                } else if (*c == '*' || *c == '+' || *c == '-' || *c == '/' || *c == ':' || *c == '<' || *c == '='
                        || *c == '>' || *c == '[' || *c == ']' || *c == '(' || *c == ')' || *c == ',' || *c == '!'){
                    tokenAtual = UNICO;
                } else if (*c == ' ' || *c == 13 || *c == 10 || *c == '\t') {
                            /// espaço, nova linha, line feed ou tabulação
                    break;  /// entao volta para o comeco do laco e le o proximo caracter
                } else if (*c == '{') {
                    tokenAtual = COMENTARIO;
                } else if (*c == '&' || *c == '|'){
                    tokenAtual = LOGICO;
                } else if (*c == EOF){
                    tokenAtual = EOF;
                } else {    /// algum caracter não válido
                    tokenAtual = NI;
                    printf("CARACTER INVALIDO: %d\n", *c);
                }

                /// quando sai do case inicial, volta para o 'switch(tokenAtual)' para processar o token identificado no case INICI
                goto recomputaSwitch;

            case NUMERO:    /// este estado le ate o final do numero

                resp = (char*) malloc(SIZE_NUM*sizeof(char));   /// váriavel para guardar cada dígito do número
                char size_num = SIZE_NUM;                       /// se precisar realocar mais espaco, sera incrementado o size_num
                char isFloat = FALSE;                           /// se ira transformar em numero com o atof() ou atoi()

                /// número decimal
                i = getDecimal(resp, (char) 0, size_num, c);    /// lê todo o número e retorna a última posição do array

                /// número flutuante
                if(*caracterAtual == '.'){                      /// se for igual há um ponto, lê o ponto e o número depois do ponto
                    i = getFlutuante(resp, i, size_num);        /// lê todo o restante de números depois da vírgula
                    isFloat = TRUE;
                }

                /// número com notação científica
                /*if(*characterAtual == 'e' || *characterAtual == 'E'){
                    /// chama a função que pega o caractere
                }*/

                finishToken = TRUE;                                 /// termina de ler
                resp[i] = '\0';                                     /// finaliza a representacao do numero no resposta
                token = (TokenRecord*) malloc(sizeof(TokenRecord)); /// cria o token

                /// transformacao do numero
                token->tokenval = isFloat ? NUM_F:NUM_I;            /// marca se é numero inteiro ou float
                if(isFloat) {
                    float *stringval = (float *) malloc(sizeof(float));
                    *stringval = atol(resp);
                    token->val = (void *) stringval;
                } else {
                    int *stringval = (int *) malloc(sizeof(int));
                    *stringval = atoi(resp);                            /// recupera o valor inteiro
                    token->val = (void *) stringval;                    /// guarda no string val
                }

                if(!desalocador){                                       /// se ainda nao tiver sido usado thread
                    desalocador = (pthread_t*) malloc(sizeof(pthread_t));
                }

                pthread_create(desalocador, NULL, desaloca, (void *)resp);   /// inicia desalocar com a thread*/
                //free(resp);
                break;

            case IDENTIFICADOR:
                token = (TokenRecord*) malloc(sizeof(TokenRecord));             /// cria o token
                token->tokenval = ID;                                           /// diz que ele eh identificador
                char size_ident = SIZE_IDENT;
                char *stringval = (char*) malloc(size_ident*sizeof(char));      /// letras de ate size_ident caracteres

                for(i = 0; TRUE; i ++){                                         /// lê enquanto tiver caracteres para ler

                    stringval[i] = *c;                                          /// adiciona o caracter na resposta
                    c = getCaracter();                                          /// le o proximo

                    if( ((*c < 'A' || *c > 'Z') && (*c < 'a' || *c > 'z')) &&   /// nao é um digito
                        (*c < '0' || *c > '9')  && *c != '_'){                  /// nao é um numero nem '_'
                        break;                                                  /// termina este for
                    }

                    if(i == size_ident-1){                                      /// se precisar realocar
                        stringval = realoca(stringval, &i, &size_ident);        /// realoca e devolve o novo ponteiro para stringval
                    }
                }

                finishToken = TRUE;                     /// termina de ler
                stringval[i+1] = '\0';                  /// finaliza a representação do identificador
                token->val = (void *) stringval;        /// guarda o ponteiro do identificador
                caracterAtual = c;                      /// não processa o caracter atual

                break;

            case UNICO:     /// estes são: * + - / : < = > [ ] , !
                token = (TokenRecord*) malloc(sizeof(TokenRecord));

                switch(*c){
                    case '+':
                        token->tokenval = SOMA;
                        break;
                    case '-':
                        token->tokenval = SUBTRACAO;
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
                        token->tokenval = IGUALDADE;
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
                    case '!':
                        token->tokenval = NEGACAO;
                        break;
                    case ':':   /// pode ser ': ou ':='
                        token->tokenval = DOIS_PONTOS;          /// a principio são dois pontos
                        char *nextCharacter = getCaracter();    /// é preciso saber se o próximo é '='

                        if(*nextCharacter == '='){              /// verifica se é um ':='
                            token->tokenval = ATRIBUICAO;
                        } else {                                /// entao é somente ':'
                            caracterAtual = nextCharacter;
                        }
                }

                finishToken = TRUE;                     /// termina de ler
                break;

            case LOGICO:
                    tokenAtual = NI;                    /// apenas para não deixar um statement vazio
                    char *nextCharacter = getCaracter();
                    if (*c != *nextCharacter){          /// não são os mesmos caracteres: '||' ou '&&'
                        caracterAtual = nextCharacter;
                        goto recomputaSwitch;           /// cria um token de EOF
                    }

                    finishToken = TRUE;
                    token = (TokenRecord*) malloc(sizeof(TokenRecord));
                    token->tokenval = (*c == '&' ? E_LOGICO : OU_LOGICO);

                break;

            case COMENTARIO:
                printf("COMENTARIO IGNORADO\n");
                char qtd = 1;                           /// quantidade de fechas - '}' - que faltam
                while(qtd){
                    c = getCaracter();
                    if (*c == '}')
                        qtd --;
                    else if (*c == '{')
                        qtd ++;
                }

                tokenAtual = INICIAL;                 /// volta para estado inicial
                break;

            case EOF:    /// EOF
                finishToken = TRUE;
                token = (TokenRecord*) malloc(sizeof(TokenRecord));
                token->tokenval = EOF;
                break;

            case NI:    /// NAO_IDENTIFICADO
                finishToken = TRUE;
                token = (TokenRecord*) malloc(sizeof(TokenRecord));
                token->tokenval = NAO_IDENTIFICADO;
                break;
        }
    }

    return token;
}

void print(TokenRecord *token){
        if (token->tokenval == ID)
            printf("(ID, %s)\n", (char *) token->val);
        else if (token->tokenval == NUM_I)
            printf("(NUM, %d)\n", *((int *) token->val));
        else if (token->tokenval == NUM_F)
            printf("(NUM, %f)\n", *((float *) token->val));
        else if (token->tokenval == SOMA)
            printf("( + )\n");
        else if (token->tokenval == SUBTRACAO)
            printf("( - )\n");
        else if (token->tokenval == MULTIPLICACAO)
            printf("( * )\n");
        else if (token->tokenval == DIVISAO)
            printf("( / )\n");
        else if (token->tokenval == MAIOR)
            printf("( > )\n");
        else if (token->tokenval == MENOR)
            printf("( < )\n");
        else if (token->tokenval == IGUALDADE)
            printf("( = )\n");
        else if (token->tokenval == ABRE_COLCHETES)
            printf("( [ )\n");
        else if (token->tokenval == FECHA_COLCHETES)
            printf("( ] )\n");
        else if (token->tokenval == ABRE_PARENTESES)
            printf("( ( )\n");
        else if (token->tokenval == FECHA_PARENTESES)
            printf("( ) )\n");
        else if (token->tokenval == VIRGULA)
            printf("( , )\n");
        else if (token->tokenval == DOIS_PONTOS)
            printf("( : )\n");
        else if (token->tokenval == ATRIBUICAO)
            printf("( := )\n");
        else if (token->tokenval == E_LOGICO)
            printf("( && )\n");
        else if (token->tokenval == OU_LOGICO)
            printf("( || )\n");
        /// falta o >= <=
}