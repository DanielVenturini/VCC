#include "lexical/varredura.h"
#include "syntactic/parse.h"
#include "tree/tree.h"
#include "semantic/poda.h"
#include <string.h>

typedef TokenRecord TR; // apenas para economizar espaços na linha

char **arquivos;    // este ponteiro conterá todos os arquivo passados por parâmetros
char flags[6];      // cada posição se refere a um tipo de flag
// flags: [0] = -tk; [1] = -ax; [2] = -at; [3] = -sx; [4] = -st; [5] = -h;

void qualFlag(char *flag) {

    if(strcmp(flag, "-tk") == 0 || strcmp(flag, "--tokens") == 0)       // --tokens
        flags[0] = 1;
    else if(strcmp(flag, "-ax") == 0 || strcmp(flag, "--ast-x") == 0)   // --abstract syntax tree-xdot
        flags[1] = 1;
    else if(strcmp(flag, "-at") == 0 || strcmp(flag, "--ast-t") == 0)   // --abstract syntax tree-term
        flags[2] = 1;
    else if(strcmp(flag, "-sx") == 0 || strcmp(flag, "--st-x") == 0)    // --syntax tree-xdot
        flags[3] = 1;
    else if(strcmp(flag, "-st") == 0 || strcmp(flag, "--st-t") == 0)    // --syntax tree-term
        flags[4] = 1;
    else if(strcmp(flag, "-h") == 0 || strcmp(flag, "--help") == 0)     // --help
        flags[5] = 1;
    else    // flag desconhecida
        printf("\n\e[38;5;184mvcc:\e[38;5;196m erro: flag desconhecida \e[38;5;255m\'%s\'.\n\n", flag);
}

// esta função separa as flags dos arquivos no argv
void separaArquivosFlags(int argc, char *argv[]) {

    arquivos = (char **) malloc((argc-1)*sizeof(char*));        // no máximo argc arquivo

    char i;
    char a = 0;
    for(i = 1; i < argc; i ++){
        if(*(argv[i]) == '-') {   // se começar com um '-', então é uma flag
            qualFlag(argv[i]);
        } else
            arquivos[a ++] = argv[i];
    }

    arquivos[a] = 0;            // encerrar a lista de arquivos
}

// imprime a linha com os cantos formatados
void desenhaLinha(char linha) {

    if(linha == 0) {            // linha superior
        printf("\u250C");
    } else if (linha == 1) {    // linhas intermediárias
        printf("\u251C");
    } else {                    // linha inferior
        printf("\u2514");
    }

    char i;
    for(i = 0; i < 77; i ++)
        printf("\u2500");

    if(linha == 0) {            // linha superior
        printf("\u2510\n");
    } else if (linha == 1) {    // linhas intermediárias
        printf("\u2524\n");
    } else {                    // linha inferior
        printf("\u2518\n");
    }
}

void printHelp() {
    // https://en.wikipedia.org/wiki/Box-drawing_character
    desenhaLinha(0);
    printf("\u2502     V C C   -      Venturini Compiler  Compiler     -     B R A S I L       \u2502\n");
    desenhaLinha(1);
    printf("\u2502               vcc [flags] arquivo1.tpp arquivo2.tpp ...                     \u2502\n");
    desenhaLinha(1);

    printf("\u2502Flags:                                                                       \u2502\n");
    printf("\u2502   -h,    --help      exibe ajuda--------------------------------------------\u2502\n");
    printf("\u2502   -tk,   --tokens,   exibe os tokens----------------------------------------\u2502\n");
    printf("\u2502   -ax,   --ast-x,    exibe as árvores de análises sintáticas no xdot--------\u2502\n");
    printf("\u2502   -at,   --ast-t,    exibe as árvores de análises sintáticas no terminal----\u2502\n");
    printf("\u2502   -sx,   --st-x,     exibe as árvores sintáticas no xdot--------------------\u2502\n");
    printf("\u2502   -st,   --st-t,     exibe as árvores sintáticas no terminal----------------\u2502\n");
    //printf("\u2502   -ts,   --tab-s,    exibe a tabela de símbolos-----------------------------\u2502\n");

    desenhaLinha(2);
}

int main(int argc, char *argv[]) {
    if(argc < 2){
        fprintf(stderr, "Use: vcc [flags] [arquivo1.tpp arquivo2.tpp ...]\n");
        return 1;
    }

    // separa o nome dos arquivos das flags
    separaArquivosFlags(argc, argv);

    // permite compilar vários arquivos de uma só vez
    char i = 0;
    while(arquivos[i]) {

        if(openFile(arquivos[i]) == '1'){       /// erro ao abrir arquivo
            return 1;
        }

        /****************\
        * análise léxica *
        \****************/
        TR *primeiro = getToken();
        TR *token = primeiro;

        while (token->tokenval != EOFU) {       // enquanto não chegar no fim do arquivo
            token->proximo = (struct TokenRecord *) getToken(); // recupera o próximo token
            token = (TR *) token->proximo;      // substitui para verificar se não chegou no fim
        }

        printf("ARQUIVO: %s.\n", arquivos[i]);
        if(flags[0]) {                          // se a flag para tokens for setada: --tokens -tk
            token = primeiro;
            do {
                printToken(token, 1, 1);        // se 0, não printar número da linha/caracter; se 1, printar
                token = (TR *) token->proximo;  // avança para o próximo
            } while (token->tokenval != EOFU);
            printf("\n");
        }

        /*******************\
        * análise sintática *
        \*******************/
        TreeNode *ast = parse(arquivos[i], primeiro);   /// recupera a árvore chamando o getToken()

        if(flags[1])
            printArvoreX(ast, arquivos[i], 0);          // 0 indica que a árvore é da análise sintática
        if(flags[2])
            printArvoreT(ast);

        /*******************\
        * análise semântica *
        \*******************/
        TreeNode *st = podar(ast);                      // podar a árvore

        if(flags[3])
            printArvoreX(st, arquivos[i], 1);           // 0 indica que a árvore é da análise semântica
        if(flags[4])
            printArvoreT(st);

        i ++;
    }

    if(flags[5]) {
        printHelp();
    }

    return 0;
}