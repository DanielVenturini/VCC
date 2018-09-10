#include "lexical/varredura.h"
#include "syntactic/parse.h"

int main(int argc, char *argv[]){
    if(argc < 2){
        fprintf(stderr, "Use: vcc file.tpp");
        return 1;
    }

    // permite compilar vários arquivos de uma só vez
    char i = 1;
    while(argv[i]) {

        if(openFile(argv[1]) == '1'){       /// erro ao abrir arquivo
            return 1;
        }

        TreeNode *programa = parse(argv[i]);/// recupera a árvore chamando o getToken()
        //printArvore(programa);

        i ++;
    }

    return 0;
}
