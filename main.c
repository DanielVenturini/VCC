#include "lexical/varredura.h"
#include "syntactic/parse.h"

TokenRecord *novo_token(TokenType tokenval, unsigned short int numline, unsigned char numcaracter, void *val){
    TokenRecord *token = (TokenRecord*) malloc(sizeof(TokenRecord));/// cria o token
    token->tokenval = tokenval;
    token->numline = numline;
    token->numcaracter = numcaracter;
    token->val = val;

    return token;
}

int main(int argc, char *argv[]){
    if(argc != 2){
        fprintf(stderr, "Use: vcc file.tpp");
        return 1;
    }

    if(openFile(argv[1]) == '1'){       /// erro ao abrir arquivo
        return 1;
    }

    //TreeNode *raiz = parse();           /// recupera a árvore chamando o getToken()

    int *n = (int *) malloc(sizeof(int));
    int *n1 = (int *) malloc(sizeof(int));
    *n = 10;
    *n1 = 4;

    TreeNode *id = novo_node(novo_token(ID, 2, 1, "a"));
    TreeNode *attr = novo_node(novo_token(ATRIBUICAO, 2, 3, NULL));
    TreeNode *num1 = novo_node(novo_token(NUM_I, 2, 6, (void *) n));
    TreeNode *soma = novo_node(novo_token(SOMA, 2, 9, NULL));
    TreeNode *num2 = novo_node(novo_token(NUM_I, 2, 11, (void *) n1));
    TreeNode *id2 = novo_node(novo_token(ID, 3, 1, "DAaNieL"));

    insere_filho(attr, id);
    insere_filho(attr, soma);
    //insere_filho(soma, id2);
    insere_filho(soma, num1);
    insere_filho(soma, num2);

    printArvore(attr);

    return 0;
}