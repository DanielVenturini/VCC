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

        if (token->tokenval == ID)
            printf("(ID, %s)\n", (char *) token->val);
        else if (token->tokenval == NUM_I)
            printf("(NUM, %d)\n", *((int *) token->val));
        else if (token->tokenval == NUM_F)
            printf("(NUM, %f)\n", *((float *) token->val));
        else if (token->tokenval == SOMA || token->tokenval == SUBTRACAO ||token->tokenval == MULTIPLICACAO ||token->tokenval == DIVISAO)
            printf("( %c )\n", *((char *) token->val));
        else if (token->tokenval == MAIOR || token->tokenval == MENOR || token->tokenval == IGUALDADE)
            printf("( %c )\n", *((char *) token->val));
        else if (token->tokenval == ABRE_COLCHETES || token->tokenval == FECHA_COLCHETES
        	  || token->tokenval == ABRE_PARENTESES || token->tokenval == FECHA_PARENTESES)
            printf("( %c )\n", *((char *) token->val));
        else if (token->tokenval == VIRGULA || token->tokenval == DOIS_PONTOS)
        	printf("( %c )\n", *((char *) token->val));
        else if (token->tokenval == ATRIBUICAO)
            printf("( %s )\n", (char *) token->val);
        else if (token->tokenval == E_LOGICO || token->tokenval == OU_LOGICO)
        	printf("( %s )\n", (char *) token->val);

    } while (token->tokenval != EOF);

    return 0;
}