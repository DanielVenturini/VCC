#include "lexical/varredura.h"

int main(int argc, char *argv[]){
    if(argc != 2){
        fprintf(stderr, "Use: vcc file.tpp");
        return 1;
    }

    if(openFile(argv[1]) == '1'){      /// erro ao abrir arquivo
        return 1;
    }

    TokenRecord *token = getToken();
    while(token->tokenval != EOF){

        if (token->tokenval == ID)
            printf("(ID, %s)\n", token->attribute.stringval);
        else if (token->tokenval == NUM)
            printf("(NUM, %f)\n", token->attribute.numval);
        else if (token->tokenval == SOMA || token->tokenval == SUB ||token->tokenval == MULT ||token->tokenval == DIV)
            printf("( %c )\n", *token->attribute.stringval);
        else if (token->tokenval == MAIOR || token->tokenval == MENOR || token->tokenval == IGUAL)
            printf("( %c )\n", *token->attribute.stringval);
        else if (token->tokenval == A_COL || token->tokenval == F_COL || token->tokenval == A_PAR || token->tokenval == F_PAR)
            printf("( %c )\n", *token->attribute.stringval);
        else if (token->tokenval == ATTR)
            printf("( %s )\n", token->attribute.stringval);

        free(token);
        token = getToken();
    }

    return 0;
}