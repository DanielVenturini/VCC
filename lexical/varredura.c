#include "varredura.h"

char respVal = 0;               /// se respVal for 0, resp não está apontando para um endereço alocado. Se for 1, não precisa realocar em resp
char *resp = NULL;              /// o resultado do token - este será usado para guardar somente dos tokens id ou numero.
unsigned short int posFile = 0; /// posição do ponteiro do arquivo. Usamos quando precisamos 'voltar' um ou dois caracteres no arquivo, então mudamos o ponteiro para trás. Permite arquivos com até 65.535 caracteres
FILE *leitorArquivo;            /// ponteiro para leitura do arquivo
pthread_t *desalocador = NULL;  /// ponteiro da thread responsável por desalocar a memória

/********************************************************************
*                   IMPLEMENTAÇÃO DAS FUNÇÕES                       *
********************************************************************/
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

    char *c = malloc(sizeof(char));     /// aloca um espaço
    *c = getc(leitorArquivo);           /// lê do arquivo
    posFile ++;                         /// incrementa a posição do arquivo

    return c;
}

void voltaCaracter(){
    posFile --;                                     /// decrementa a quantidade de caracteres lido
    char status = fseek(leitorArquivo, posFile, 0); /// desloca o ponteiro para, a partir do zero, uma posição atrás da atual

    if(status == -1) {                              /// se deu erro
        printf("ERRO AO VOLTAR CARACTER NO ARQUIVO\n");
    }
}

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

/// verifica se um identificador é igual á uma palavra reservada.
/// se o for chegar até o final, quer dizer que cada caracter são iguais.
/// vale também para identificadores acentuadpos: SENÃO, ENTÃO e ATÉ
char iguais(char* identificador, char palavraReservada[]){
    char i;
    for(i = 0; (identificador[i] == palavraReservada[i]) && identificador[i] != '\0' && palavraReservada[i] != '\0'; i ++);

    return (identificador[i]==palavraReservada[i] && palavraReservada[i]=='\0')?'1':'0';
}

void palavrasReservadas(TokenRecord *token){

    /// descobre qual é o caracter inicial do identificador, pois elimina mais da metade das palavras reservadas
    switch(((char *) token->val)[0]){
        case 'a':   /// pode ser o 'até'
            if(iguais(token->val, "até\0") == '0') {            /// se forem diferentes
                respVal = 0;                                    /// indica que na próxima vez precisa alocar novamente
                return;                                         /// não faz nada
            }

            token->tokenval = ATE;
            break;

        case 'e':   /// pode ser o 'então' ou 'escreva'
            if(iguais(token->val, "então\0") == '1')            /// é o então
                token->tokenval = ENTAO;                        /// atualiza o token
            else if (iguais(token->val, "escreva\0") == '1')
                token->tokenval = ESCREVA;
            else {
                respVal = 0;                                    /// indica que na próxima vez precisa alocar novamente
                return;                                         /// retorna pra não desalocar o token->val
            }

            break;

        case 'i':   /// pode ser o 'inteiro'
            if(iguais(token->val, "inteiro\0") == '0'){         /// se forem diferentes
                respVal = 0;                                    /// indica que na próxima vez precisa alocar novamente
                return;                                         /// não faz nada
            }

            token->tokenval = INTEIRO;
            break;

        case 'f':   /// pode ser 'fim' ou 'flutuante'
            if(iguais(token->val, "fim\0") == '1')              /// é o fim
                token->tokenval = FIM;                          /// atualiza o token
            else if (iguais(token->val, "flutuante\0") == '1')
                token->tokenval = FLUTUANTE;
            else {
                respVal = 0;                                    /// indica que na próxima vez precisa alocar novamente
                return;                                         /// retorna pra não desalocar o token->val
            }

            break;

        case 'l':   /// pode ser o 'leia'
            if(iguais(token->val, "leia\0") == '0') {           /// se forem diferentes
                respVal = 0;                                    /// indica que na próxima vez precisa alocar novamente
                return;                                         /// não faz nada
            }

            token->tokenval = LEIA;
            break;

        case 'r':   /// pode ser o 'repita' ou 'retorna'
            if(iguais(token->val, "repita\0") == '1')           /// é o repita
                token->tokenval = REPITA;                       /// atualiza o token
            else if (iguais(token->val, "retorna\0") == '1')
                token->tokenval = RETORNA;
            else {
                respVal = 0;                                    /// indica que na próxima vez precisa alocar novamente
                return;                                         /// retorna pra não desalocar o token->val
            }

            break;

        case 's':   /// pode ser o 'se' ou 'senão'
            if(iguais(token->val, "se\0") == '1')               /// é o se
                token->tokenval = SE;                           /// atualiza o token
            else if (iguais(token->val, "senão\0") == '1')
                token->tokenval = SENAO;
            else {
                respVal = 0;                                    /// indica que na próxima vez precisa alocar novamente
                return;                                         /// retorna pra não desalocar o token->val
            }

            break;

        default:            /// outra palavra que não começa com estas letras
            respVal = 0;    /// indica que na próxima vez precisa alocar novamente
            return;         /// então retorna para não desalocar
    }

    /// não precisa desalocar, apenas precisa setar a variavel 'respVal' como 1
    /// pois indicamos que ela tem espaço que pode ser sobrescrito
    respVal = 1;
}

/// lê toda a sequência de digitos e guarda em 'resp'.
/// como pode ser chamada de outras funções, por exemplo, 'getFlutuante',
/// irá começar a concatenar a partir de 'i'.
/// 'size_max' é o tamanho máximo que pode ser lido sem precisar de usar um 'realloc'
/// 'c' é o dígito que ainda não foi processado
char getDecimal(char *resp, char i, char size_max, char *c) {

    while(TRUE){                            /// fica lendo até parar de ler dígitos

        if(*c < '0' || *c > '9'){           /// não é um digito
            voltaCaracter();                /// volta o caracter no arquivo
            return i;                       /// retorna a próxima posição vazia do array
        }

        if(i % size_max == size_max-1){     /// se precisar realocar
            resp = realoca(resp, &i, &size_max);
        }

        resp[i] = *c;                       /// adiciona o digito na resposta
        c = getCaracter();                  /// le o proximo

        i ++;
    }
}

/// lê toda a sequência de números a e guarda em 'resp'.
/// 'c' deve ser o '.' que trousse a esta função.
char getFlutuante(char *resp, char i, char size_max) {

    resp[i] = '.';              /// consome o caractere
    char *c = getCaracter();    /// pega o próximo

    return getDecimal(resp, i+1, size_max, c);   /// lê todo o resto de número
}

/// o caracter atual é 'e' ou 'E'.
/// Então lê um '-' ou '+' seguido de um número
char getNotacaoCientifica(char *resp, char i, char size_max, char *isFloat) {

    //char *e = getCaracter();          /// este é 'e' ou 'E'
    char *sinal_numero = getCaracter(); /// pode ser '-' ou '+' ou número
    char *numero = NULL;                /// este apontará para o número

    if(*sinal_numero != '-' && *sinal_numero != '+') {  /// se não for sinal
        if(*sinal_numero < '0' || *sinal_numero > '9') {/// e não for letra
            voltaCaracter();                            /// volta este que não é sinal nem número
            voltaCaracter();                            /// volta o 'e'

            return i;
        }
        /// então é número
    } else {                                /// 'sinal_numero' realmente tem um sinal
        numero = getCaracter();             /// este DEVE ser número
        if(*numero < '0' || *numero > '9') {/// se não for número
            voltaCaracter();                /// volta este que não é número
            voltaCaracter();                /// volta o que é sinal
            voltaCaracter();                /// volta o 'e'

            return i;
        } else {                            /// se for número
            voltaCaracter();                /// volta o número para depois re-ler
        }
    }

    if(i % size_max == size_max-2){     /// se precisar realocar, realoca duas posições
        resp = realoca(resp, &i, &size_max);
    }

    resp[i++] = 'e';            /// 'e' ou 'E'
    resp[i++] = *sinal_numero;  /// numero ou sinal
    *isFloat = TRUE;            /// marca como float

    return getDecimal(resp, i, size_max, getCaracter());/// lê todo o resto de número
}

/// alguns tokens podem mudar dependendo do próximo caracter: >, >=
/// esta função verifica se o próximo caracter é igual ao 'proximoCaracter'.
/// se for, troca o tipo atual do token para o 'tipo'
char verificaAFrente(TokenRecord *token, char proximoCaracter, TokenType tipo){
    char *caracter = getCaracter();

    if(*caracter != proximoCaracter){   /// se o caracter lido for diferente do esperado
        voltaCaracter();                /// guarda o caracter não processado
        return '0';                     /// retorna que não foi trocado, pois o '<' pode ser '<=' e '<>'
    }

    token->tokenval = tipo;             /// troca o tipo;
    return '1';
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
    char *nextCharacter;        /// alguns tokens precisam ver o próximo para saber quem são: '>' '>='
    int finishToken = FALSE;    /// se terminou de ler o token
    int tokenAtual;             /// o token atual de processamento
    TokenRecord *token;         /// o token propriamente dito

    while(!finishToken){
        inicial:                /// label para depois do comentário, voltar e recomputar o novo token

        c = getCaracter();      /// lê o caracter

        if(*c == EOF){          /// se já chegou no fim do arquivo
            tokenAtual = EOF;   /// retorna um token EOF
        }

        /// computa o token atual
        if (*c >= '0' && *c <= '9'){
            tokenAtual = NUMERO;
        } else if ( (*c >= 'A' && *c <= 'Z') || (*c >= 'a' && *c <= 'z') ){
            tokenAtual = IDENTIFICADOR;
        } else if (*c == '*' || *c == '+' || *c == '-' || *c == '/' || *c == ':' || *c == '<' || *c == '='
                || *c == '>' || *c == '[' || *c == ']' || *c == '(' || *c == ')' || *c == ',' || *c == '!'){
            tokenAtual = UNICO;
        } else if (*c == ' ' || *c == 13 || *c == 10 || *c == '\t') {
                            /// espaço, nova linha, line feed ou tabulação
            goto inicial;   /// entao volta para o comeco do laco e le o proximo caracter
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

        char i;                 /// usado nos cases de tokens de números e identificadores
        recomputaSwitch:        /// label do goto para nao precisar ler caracter novamente
        switch(tokenAtual){

            case NUMERO:    /// este estado le ate o final do numero

                if(!respVal)/// se o valor for 0
                    resp = (char*) malloc(SIZE_NUM*sizeof(char)); /// aloca para guardar cada dígito do número

                char size_num = SIZE_NUM;                       /// se precisar realocar mais espaco, sera incrementado o size_num
                char isFloat = FALSE;                           /// se ira transformar em numero com o atof() ou atoi()

                i = getDecimal(resp, (char) 0, size_num, c);    /// lê todo o número e retorna a última posição do array

                if(*getCaracter() == '.'){                      /// se for igual há um ponto, lê o ponto e o número depois do ponto
                    i = getFlutuante(resp, i, size_num);        /// lê todo o restante de números depois da vírgula
                    isFloat = TRUE;
                } else {
                    voltaCaracter();                            /// se não for o '.', volta o caracter
                }

                c = getCaracter();
                /// número com notação científica
                if(*c == 'e' || *c == 'E'){
                    i = getNotacaoCientifica(resp, i, size_num, &isFloat);
                } else {
                    voltaCaracter();                            /// volta o caracter
                }

                finishToken = TRUE;                                 /// termina de ler
                resp[i] = '\0';                                     /// finaliza a representacao do numero no resposta
                token = (TokenRecord*) malloc(sizeof(TokenRecord)); /// cria o token

                /// transformacao do numero
                token->tokenval = isFloat ? NUM_F:NUM_I;            /// marca se é numero inteiro ou float
                if(isFloat) {
                    float *numval = (float *) malloc(sizeof(float));
                    *numval = atof(resp);
                    token->val = (void *) numval;
                } else {
                    int *numval = (int *) malloc(sizeof(int));
                    *numval = atoi(resp);                            /// recupera o valor inteiro
                    token->val = (void *) numval;                    /// guarda no string val
                }

                respVal = 1;
                break;

            case IDENTIFICADOR:
                if(!respVal)                                                    /// se o valor for 0
                    resp = (char*) malloc(SIZE_IDENT*sizeof(char));             /// aloca para guardar letras até size_ident caracteres

                token = (TokenRecord*) malloc(sizeof(TokenRecord));             /// cria o token
                token->tokenval = ID;                                           /// diz que ele é identificador, PORÉM PODE SER ALTERADO SE FOR UMA PALAVRA RESERVADA
                char size_ident = SIZE_IDENT;

                for(i = 0; TRUE; i ++){                                         /// lê enquanto tiver caracteres para ler

                    resp[i] = *c;                                               /// adiciona o caracter na resposta
                    c = getCaracter();                                          /// le o proximo

                    if( ((*c < 'A' || *c > 'Z') && (*c < 'a' || *c > 'z')) &&   /// nao é um digito
                        (*c < '0' || *c > '9') &&                               /// não é número
                        (*c != '_' && *c != 'ã' && *c != 'é')){                 /// não é '_' nem 'ã' nem 'é'
                        break;                                                  /// termina este for
                    }

                    if(i == size_ident-1){                                      /// se precisar realocar
                        resp = realoca(resp, &i, &size_ident);        /// realoca e devolve o novo ponteiro para resp
                    }
                }

                finishToken = TRUE;         /// termina de ler
                resp[i+1] = '\0';           /// finaliza a representação do identificador
                token->val = (void *) resp; /// guarda o ponteiro do identificador
                voltaCaracter();            /// não processa o caracter atual
                palavrasReservadas(token);  /// verifica se o valor do token não é uma palavra reservada e troca o seu tipo

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
                    case '=':
                        token->tokenval = IGUALDADE;
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
                    case '>':   /// pode ser '>' ou '>='
                        token->tokenval = MAIOR;
                        verificaAFrente(token, '=', MAIOR_IGUAL);
                        break;
                    case '<':   /// pode ser '<' ou '<=' ou '<>'
                        token->tokenval = MENOR;
                        if(verificaAFrente(token, '=', MENOR_IGUAL) != '1') /// se não for '<='
                            verificaAFrente(token, '>', DIFERENTE);         /// teste se é '<>'
                        break;
                    case ':':   /// pode ser ': ou ':='
                        token->tokenval = DOIS_PONTOS;          /// a principio são dois pontos
                        verificaAFrente(token, '=', ATRIBUICAO);
                        break;
                }

                finishToken = TRUE;                     /// termina de ler
                break;

            case LOGICO:
                    tokenAtual = NI;                    /// o token ainda não foi identificado
                    char *nextCharacter = getCaracter();
                    if (*c != *nextCharacter){          /// não são os mesmos caracteres: '||' ou '&&'
                        voltaCaracter();                /// guarda o último caractere lido, o 'nextCharacter', pois o 'c' será processado pelo case 'NI'
                        tokenAtual = NI;                /// token não identificado
                        goto recomputaSwitch;           /// cria um token de NI
                    }

                    finishToken = TRUE;
                    token = (TokenRecord*) malloc(sizeof(TokenRecord));
                    token->tokenval = (*c == '&' ? E_LOGICO : OU_LOGICO);

                break;

            case COMENTARIO:
                1;
                char qtd = 1;           /// quantidade de fechas - '}' - que faltam
                while(qtd){             /// só termina de processar o comentário quando tiver fechado todos os abre
                    c = getCaracter();
                    if (*c == '}')
                        qtd --;
                    else if (*c == '{')
                        qtd ++;
                }

                goto inicial;           /// volta para estado inicial quando sair do while

            case EOF:    /// EOF
                finishToken = TRUE;
                token = (TokenRecord*) malloc(sizeof(TokenRecord));
                token->tokenval = EOF;
                break;

            case NI:    /// NAO_IDENTIFICADO
                finishToken = TRUE;
                token = (TokenRecord*) malloc(sizeof(TokenRecord));
                token->tokenval = NAO_IDENTIFICADO;
                token->val = (void *) c;
                respVal = 1;            /// este endereço não pode ser reaproveitado
                break;
        }
    }

    return token;
}

void printToken(TokenRecord *token){
        if (token->tokenval == ID)
            printf("(ID, %s)\n", (char *) token->val);
        else if (token->tokenval == NUM_I)
            printf("(NUM, %d)\n", *((int *) token->val));
        else if (token->tokenval == NUM_F)
            printf("(NUM, %f)\n", *((float *) token->val));
        else if (token->tokenval == ATE)
            printf("ATE\n");
        else if (token->tokenval == ENTAO)
            printf("ENTAO\n");
        else if (token->tokenval == ESCREVA)
            printf("ESCREVA\n");
        else if (token->tokenval == FIM)
            printf("FIM\n");
        else if (token->tokenval == FLUTUANTE)
            printf("FLUTUANTE\n");
        else if (token->tokenval == INTEIRO)
            printf("INTEIRO\n");
        else if (token->tokenval == LEIA)
            printf("LEIA\n");
        else if (token->tokenval == REPITA)
            printf("REPITA\n");
        else if (token->tokenval == RETORNA)
            printf("RETORNA\n");
        else if (token->tokenval == SE)
            printf("SE\n");
        else if (token->tokenval == SENAO)
            printf("SENAO\n");
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
        else if (token->tokenval == MAIOR_IGUAL)
            printf("( >= )\n");
        else if (token->tokenval == MENOR_IGUAL)
            printf("( <= )\n");
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
        else if (token->tokenval == DIFERENTE)
            printf("( <> )\n");
        else if (token->tokenval == NAO_IDENTIFICADO)
            printf("(NAO_IDENTIFICADO, %s)\n", (char *) token->val);
}
