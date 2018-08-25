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
    TokenRecord **tokenvetor = (TokenRecord**) malloc(48*sizeof(TokenRecord*));
    int i = 0;
    do {

    	token = getToken();
        printToken(token, 0);           /// se 0, não printar linha; se 1, printar
        tokenvetor[i] = token;

        i ++;
    } while (token->tokenval != EOF);

    printf("------------------------------------------------");
    for(i = 0; i < 48; i ++){
        printToken(tokenvetor[i], 0);
    }

    return 0;
}