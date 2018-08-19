#include "lexical/varredura.h"

int main(int argc, char *argv[]){
    if(argc != 2){
        fprintf(stderr, "Use: vcc file.tpp");
        return 1;
    }

    if(openFile(argv[1]) == '1'){      /// erro ao abrir arquivo
        return 1;
    }

    TokenRecord *token;
    do {

    	token = getToken();
        printToken(token);

    } while (token->tokenval != EOF);

    return 0;
}