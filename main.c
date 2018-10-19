#include "lexical/varredura.h"
#include "syntactic/parse.h"
#include <string.h>

typedef enum {      // os tipos de flags reconhecidas
    TOKENS, AST_X, AST_T, ST_X, ST_T, HELP, DESCONHECIDA
} FlagType;

char **arquivos;        // este ponteiro conterá todos os arquivo passados por parâmetros
FlagType **flagsType;   // este ponteiro conterá todos os tipos de flags passadas por parâmetros
char **flags;           // este ponteiro conterá todas as flags passadas por parâmetros

FlagType *qualFlag(char *flag, char f) {
    FlagType *flagType = (FlagType*) malloc(sizeof(FlagType));

    if(strcmp(flag, "--tokens") == 0 || strcmp(flag, "-tk") == 0)       // --tokens
        *flagType = TOKENS;
    else if(strcmp(flag, "--ast-x") == 0 || strcmp(flag, "-ax") == 0)   // --abstract syntax tree-xdot
        *flagType = AST_X;
    else if(strcmp(flag, "--ast-t") == 0 || strcmp(flag, "-at") == 0)   // --abstract syntax tree-term
        *flagType = AST_T;
    else if(strcmp(flag, "--st-x") == 0 || strcmp(flag, "-sx") == 0)    // --syntax tree-xdot
        *flagType = ST_X;
    else if(strcmp(flag, "--st-t") == 0 || strcmp(flag, "-st") == 0)    // --syntax tree-term
        *flagType = ST_T;
    else if(strcmp(flag, "--help") == 0 || strcmp(flag, "-h") == 0)     // --help
        *flagType = HELP;
    else
        *flagType = DESCONHECIDA;

    flags[f] = flag;
    return flagType;
}

// para as flags, serão no máximo duas:
// -tokens  imprime os tokens do arquivo passado
// -tree    imprime a árvore do arquivo passado

void separaArquivosFlags(int argc, char *argv[]) {

    flagsType = (FlagType**) malloc((argc-1)*sizeof(FlagType*));// no máximo argc-1 flags
    arquivos = (char **) malloc((argc-1)*sizeof(char*));        // no máximo argc arquivo
    flags = (char **) malloc((argc-1)*sizeof(char*));           // no máximo argc de flag

    char i;
    char f = 0;
    char a = 0;
    for(i = 1; i < argc; i ++){
        if(*(argv[i]) == '-') {   // se começar com um '-', então é uma flag
            flagsType[f] = qualFlag(argv[i], f);
            f ++;
        } else
            arquivos[a ++] = argv[i];
    }
}

void executaParaFlags(FlagType tipo) {

    char i = 0;
    while(arquivos[i]) {

        if(openFile(arquivos[i]) == '1'){       /// erro ao abrir arquivo
            i ++;
            continue;
        }

        switch(tipo) {

            case ST_X:
            case ST_T:
                // em breve
                printf("VCC: não implementado.\n");
                break;

            // printa a árvore no XDOT
            case AST_X:
                printArvoreX(parse(arquivos[i]), arquivos[i]);  /// recupera a árvore chamando o getToken()
                break;

            // printa a árvore no TERMINAL
            case AST_T:
                printArvoreT(parse(arquivos[i]), 0);
                break;

            case TOKENS:
                printf("%s:\n", arquivos[i]);
                TokenRecord *token;
                do {
                    token = getToken();
                    printToken(token, 1, 1);           /// se 0, não printar número da linha/caracter; se 1, printar
                } while (token->tokenval != EOFU);
                break;
            }

        i ++;
    }
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

        TreeNode *programa = parse(arquivos[i]);/// recupera a árvore chamando o getToken()

        i ++;
    }

    // para todas as flags
    i = 0;
    while(flagsType[i]){
        switch(*flagsType[i]){
            case ST_X:
            case ST_T:
            case AST_X:
            case AST_T:
            case TOKENS:
                executaParaFlags(*flagsType[i]);
                break;

            case HELP:
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
                //printf("\u2502   -sx,   --st-x,     exibe as árvores sintáticas no xdot--------------------\u2502\n");
                //printf("\u2502   -st,   --st-t,     exibe as árvores sintáticas no terminal----------------\u2502\n");

                desenhaLinha(2);
                break;

            default:
                printf("Erro: opção não reconhecida: %s.\n", flags[i]);
                printf("Use: ./vcc -h para opções.\n");
        }

        i ++;
    }

    return 0;
}