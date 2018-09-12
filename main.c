#include "lexical/varredura.h"
#include "syntactic/parse.h"

typedef enum {      // os tipos de flags reconhecidas
    TOKENS, TREE, HELP, DESCONHECIDA
} FlagType;

char **arquivos;        // este ponteiro conterá todos os arquivo passados por parâmetros
FlagType **flagsType;   // este ponteiro conterá todos os tipos de flags passadas por parâmetros
char **flags;           // este ponteiro conterá todas as flags passadas por parâmetros

FlagType *qualFlag(char *flag, char f) {
    FlagType *flagType = (FlagType*) malloc(sizeof(FlagType));

    if(flag[1] == 't' && flag[2] == 'k' && flag[3] == '\0')
        *flagType = TOKENS;
    else if(flag[1] == 't' && flag[2] == 'r' && flag[3] == '\0')
        *flagType = TREE;
    else if(flag[1] == 'h' && flag[2] == '\0')
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

            case TREE:
                printArvore(parse(arquivos[i]));/// recupera a árvore chamando o getToken()
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

int main(int argc, char *argv[]) {
    if(argc < 2){
        fprintf(stderr, "Use: vcc file.tpp");
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
            case TREE:
            case TOKENS:
                executaParaFlags(*flagsType[i]);
                break;

            case HELP:
                printf("|___________________________________________________________________________|\n");
                printf("| V C C      -       Venturini Compiler  Compiler      -       B R A S I L  |\n");
                printf("|___________________________________________________________________________|\n");
                printf("|./vcc [flags] arquivo        ----------------------------------------------|\n");
                printf("|Flags:                       ----------------------------------------------|\n");
                printf("|    -h                       exibe ajuda-----------------------------------|\n");
                printf("|    -tk                      exibe as árvores sintáticas de cada arquivo---|\n");
                printf("|    -tr                      exibe os tokens de cada arquivo---------------|\n");
                printf("|___________________________________________________________________________|\n");
                break;

            default:
                printf("Erro: opção não reconhecida: %s.\n", flags[i]);
        }

        i ++;
    }

    return 0;
}
