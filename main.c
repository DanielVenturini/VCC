#include "lexical/varredura.h"
#include "syntactic/parse.h"

int main(int argc, char *argv[]){
    if(argc != 2){
        fprintf(stderr, "Use: vcc file.tpp");
        return 1;
    }

    if(openFile(argv[1]) == '1'){       /// erro ao abrir arquivo
        return 1;
    }

    TreeNode *raiz = parse();           /// recupera a árvore chamando o getToken()
    printArvore(raiz);

    return 0;
}