#include "tree.h"

void printLabel1(TreeNode *node, FILE *treedot);
void printLabel2(TreeNode *node, FILE *treedot);
void printAresta(TreeNode *pai, TreeNode *filho, FILE *treedot);

char* criaComando(char cid, char *nomeArquivo, char analise) {

    unsigned char i;
    char prog;
    for(i = 0; nomeArquivo[i] != '\0'; i ++);           // apenas procura o '\0'

    i ++;
    if(cid == 1) {          // '1' == 'nomearquivo.tpp.dot'
        i += 4;
    } else if(cid == 2) {   // '2' == 'xdot'
        i += 23;            // 'xdot file.tpp.dot 2>.xdotwarning'
    } else {                // '3' == 'rm'
        i += 23;             // 'rm file.tpp.dot .xdotwarning -f'
    }

    i += 3;                 // mais três posições para guardar o '.ax' ou '.sx'
    char* comando = (char *) malloc(i*sizeof(char));    // aloca a quantidade necessária

    if(cid == 1) {
        sprintf(comando, "%s.dot.%s", nomeArquivo, analise==0?"ax":"sx");
    } else if(cid == 2) {
        sprintf(comando, "xdot %s.dot.%s 2>.xdotwarning", nomeArquivo, analise==0?"ax":"sx");
    } else {
        sprintf(comando, "rm %s.dot.%s .xdotwarning -f", nomeArquivo, analise==0?"ax":"sx");
    }

    return comando;
}

void nullPosicoes(TreeNode *node, char inicial, char final){
	char i;
	for(i = inicial; i < final; i ++)
		node->filhos[i] = NULL;
}

TreeNode *novo_node(TokenRecord *token, EBNFType bnfval){
    // aloca os devidos ponteiros
    TreeNode *node = (TreeNode *) malloc(sizeof(TreeNode));
    node->filhos = (TreeNode **) malloc(NUM_INICIAL_FILHOS*sizeof(TreeNode*));

    // seta como NULL, pois, inicialmente, não terão nada
    node->token = token;
    node->bnfval = bnfval;
    node->max_filhos = NUM_INICIAL_FILHOS;
    node->tipoExpressao = 0;    // 0 é VAZIO. Também pode ser INTEIRO ou FLUTUANTE

    // usado somente no gerador de código
    node->escopo = NULL;
    node->llvmValueRef = NULL;

    nullPosicoes(node, 0, node->max_filhos);

    return node;
}

// realoca a quantidade de filhos que este pai tem
void realocaFilhos(TreeNode *node) {
	char novo_tamanho = node->max_filhos + NUM_INICIAL_FILHOS;
	node->max_filhos = novo_tamanho;

	node->filhos = (TreeNode **) realloc(node->filhos, novo_tamanho*sizeof(TreeNode *));

	nullPosicoes(node, node->max_filhos-NUM_INICIAL_FILHOS, node->max_filhos);
}

void insere_filho(TreeNode *pai, TreeNode *filho){
	if(!pai || !filho)
		return;

	unsigned char i = 0;

	while (pai->filhos[i]) {		// enquanto tiver filhos já alocado

		i ++;

		if(pai->max_filhos == i)	// se o pai não tiver mais espaço para filhos
			realocaFilhos(pai);		// realoca o número de filhos
	}

	pai->filhos[i] = filho;		// insere o filho
}

// remove o último filho e o devolve
// esta função será usada somente para garantir a precedencia na regra da expressao_logica
TreeNode *remove_filho(TreeNode *pai) {
    if(!pai)
        return NULL;

    unsigned char i = 1;

    while (pai->filhos[i]) {            // enquanto não encontrar o último filho
        i ++;
    }

    i --;                               // decrementa para a posição que o último filho está
    TreeNode *filho = pai->filhos[i];   // recupera o filho que vai remover
    pai->filhos[i] = NULL;              // remove o filho

    return filho;
}

// printa a string direto no arquivo
// o arquivo pode ser o stdout, no caso para printar no terminal
// ou o arquivo pode ser o tree.dot
void printArquivo(FILE *arquivo, char *string) {
    fprintf(arquivo, "%s", string);
}

void getArvoreRecursiva(TreeNode *pai, FILE *treedot) {
	if(!pai)
		return;

	printLabel1(pai, treedot);                       // printa o pai

	char i;
	for(i = 0; pai->filhos[i]; i ++){	             // adiciona 'pai -- filho' para cada filho
		printLabel1(pai->filhos[i], treedot);        // printa o filho no arquivo
        printAresta(pai, pai->filhos[i], treedot);   // printa a aresta 'pai -- filho'
	}

	for(i = 0; pai->filhos[i]; i ++)			// chama para cada filho também
		getArvoreRecursiva(pai->filhos[i], treedot);
}

// analise diz respeito ao tipo da arvore da analise, no caso, sintática ou semântica
void printArvoreX(TreeNode *raiz, char *nomeArquivo, char analise) {
    if(!raiz)
        return;

    char* comando = criaComando(1, nomeArquivo, analise);   // retorna uma string com o nome do arquivo.dot

	FILE *treedot = fopen(comando, "w");    // abrindo o arquivo
	fprintf(treedot, "strict graph G {\n"); // imprime cabeçalho

	getArvoreRecursiva(raiz, treedot);		// recursivamente vai adicionando no arquivo

    fprintf(treedot, "}");                  // finaliza o digrafo com o '}'
	fclose(treedot);						// fecha o arquivo

    pid_t pid = fork();                     // cria o processo filho

    if (pid < 0 || pid == 0) {              // se deu erro, ou se é o processo filho
        comando = criaComando(2, nomeArquivo, analise); // cria o comando xdot file.dot ou rm file.dot
        system(comando);                        // printa a arvore com o xdot - graphviz
        comando = criaComando(3, nomeArquivo, analise); // cria o comando rm file.dot
        system(comando);                        // apaga o arquivo do xdot
        exit(0);                            // não deve retornar, e sim sair
    }
}

void printAresta(TreeNode *pai, TreeNode *filho, FILE *treedot){
    fprintf(treedot, "    %i -- %i [color=green];\n", pai, filho);
}

// printa no arquivos os nós intermediários
void printLabel1(TreeNode *node, FILE *treedot){

    if(treedot != stdout)                           // se for diferente do stdout, então printa no arquivo
        fprintf(treedot, "    %i [color=yellow, label=\"", node); // printa o id do nó e o 'label="'

    if(node->token) {      // se tiver um token, então é uma folha
        printLabel2(node, treedot);
        return;
    }

    if (node->bnfval == PROGRAMA)
        printArquivo(treedot, "PROGRAMA");
    else if (node->bnfval == LISTA_DECLARACOES)
        printArquivo(treedot, "LISTA_DECLARACOES");
    else if (node->bnfval == DECLARACAO)
        printArquivo(treedot, "DECLARACAO");
    else if (node->bnfval == DECLARACAO_VARIAVEIS)
        printArquivo(treedot, "DECLARACAO_VARIAVEIS");
    else if (node->bnfval == INICIALIZACAO_VARIAVEIS)
        printArquivo(treedot, "INICIALIZACAO_VARIAVEIS");
    else if (node->bnfval == LISTA_VARIAVEIS)
        printArquivo(treedot, "LISTA_VARIAVEIS");
    else if (node->bnfval == VAR)
        printArquivo(treedot, "VAR");
    else if (node->bnfval == INDICE)
        printArquivo(treedot, "INDICE");
    else if (node->bnfval == TIPO)
        printArquivo(treedot, "TIPO");
    else if (node->bnfval == DECLARACAO_FUNCAO)
        printArquivo(treedot, "DECLARACAO_FUNCAO");
    else if (node->bnfval == CABECALHO)
        printArquivo(treedot, "CABECALHO");
    else if (node->bnfval == LISTA_PARAMETROS)
        printArquivo(treedot, "LISTA_PARAMETROS");
    else if (node->bnfval == PARAMETRO)
        printArquivo(treedot, "PARAMETRO");
    else if (node->bnfval == CORPO)
        printArquivo(treedot, "CORPO");
    else if (node->bnfval == ACAO)
        printArquivo(treedot, "ACAO");
    else if (node->bnfval == B_SE)
        printArquivo(treedot, "SE");
    else if (node->bnfval == B_REPITA)
        printArquivo(treedot, "REPITA");
    else if (node->bnfval == B_LEIA)
        printArquivo(treedot, "LEIA");
    else if (node->bnfval == B_ESCREVA)
        printArquivo(treedot, "ESCREVA");
    else if (node->bnfval == B_RETORNA)
        printArquivo(treedot, "RETORNA");
    else if (node->bnfval == B_ATRIBUICAO)
        printArquivo(treedot, "ATRIBUICAO");
    else if (node->bnfval == EXPRESSAO)
        printArquivo(treedot, "EXPRESSAO");
    else if (node->bnfval == EXPRESSAO_LOGICA)
        printArquivo(treedot, "EXPRESSAO_LOGICA");
    else if (node->bnfval == EXPRESSAO_SIMPLES)
        printArquivo(treedot, "EXPRESSAO_SIMPLES");
    else if (node->bnfval == EXPRESSAO_ADITIVA)
        printArquivo(treedot, "EXPRESSAO_ADITIVA");
    else if (node->bnfval == EXPRESSAO_MULTIPLICATIVA)
        printArquivo(treedot, "EXPRESSAO_MULTIPLICATIVA");
    else if (node->bnfval == EXPRESSAO_UNARIA)
        printArquivo(treedot, "EXPRESSAO_UNARIA");
    else if (node->bnfval == OPERADOR_RELACIONAL)
        printArquivo(treedot, "OPERADOR_RELACIONAL");
    else if (node->bnfval == OPERADOR_SOMA)
        printArquivo(treedot, "OPERADOR_SOMA");
    else if (node->bnfval == OPERADOR_LOGICO)
        printArquivo(treedot, "OPERADOR_LOGICO");
    else if (node->bnfval == OPERADOR_MULTIPLICACAO)
        printArquivo(treedot, "OPERADOR_MULTIPLICACAO");
    else if (node->bnfval == OPERADOR_NEGACAO)
        printArquivo(treedot, "OPERADOR_NEGACAO");
    else if (node->bnfval == FATOR)
        printArquivo(treedot, "FATOR");
    else if (node->bnfval == NUMERO)
        printArquivo(treedot, "NUMERO");
    else if (node->bnfval == CHAMADA_FUNCAO)
        printArquivo(treedot, "CHAMADA_FUNCAO");
    else if (node->bnfval == LISTA_ARGUMENTOS)
        printArquivo(treedot, "LISTA_ARGUMENTOS");
    else if (node->bnfval == VAZIO)
        printArquivo(treedot, "VAZIO");

    if(treedot != stdout)                    // se for diferente do stdout, então printa no arquivo
        printArquivo(treedot, "\"];\n");     // finalizando a linha
}

// printa no arquivos as folhas
void printLabel2(TreeNode *node, FILE *treedot) {

	if (node->token->tokenval == ID)
        fprintf(treedot, "ID: %s", (char *) node->token->val);
    else if (node->token->tokenval == NUM_I)
        fprintf(treedot, "NUM_INTEIRO: %d", *(int *) node->token->val);
    else if (node->token->tokenval == NUM_F)
        fprintf(treedot, "NUM_PONTO_FLUTUANTE: %f", *((float *) node->token->val));
    else if (node->token->tokenval == ATE)
        printArquivo(treedot, "ATÉ");
    else if (node->token->tokenval == ENTAO)
        printArquivo(treedot, "ENTÃO");
    else if (node->token->tokenval == ESCREVA)
        printArquivo(treedot, "ESCREVA");
    else if (node->token->tokenval == FIM)
        printArquivo(treedot, "FIM");
    else if (node->token->tokenval == FLUTUANTE)
        printArquivo(treedot, "FLUTUANTE");
    else if (node->token->tokenval == INTEIRO)
        printArquivo(treedot, "INTEIRO");
    else if (node->token->tokenval == LEIA)
        printArquivo(treedot, "LEIA");
    else if (node->token->tokenval == REPITA)
        printArquivo(treedot, "REPITA");
    else if (node->token->tokenval == RETORNA)
        printArquivo(treedot, "RETORNA");
    else if (node->token->tokenval == SE)
        printArquivo(treedot, "SE");
    else if (node->token->tokenval == SENAO)
        printArquivo(treedot, "SENÃO");
    else if (node->token->tokenval == SOMA)
        printArquivo(treedot, "+");
    else if (node->token->tokenval == SUBTRACAO)
        printArquivo(treedot, "-");
    else if (node->token->tokenval == MULTIPLICACAO)
        printArquivo(treedot, "*");
    else if (node->token->tokenval == DIVISAO)
        printArquivo(treedot, "/");
    else if (node->token->tokenval == MAIOR)
        printArquivo(treedot, ">");
    else if (node->token->tokenval == MENOR)
        printArquivo(treedot, "<");
    else if (node->token->tokenval == MAIOR_IGUAL)
        printArquivo(treedot, ">=");
    else if (node->token->tokenval == MENOR_IGUAL)
        printArquivo(treedot, "<=");
    else if (node->token->tokenval == IGUALDADE)
        printArquivo(treedot, "=");
    else if (node->token->tokenval == ABRE_COLCHETES)
        printArquivo(treedot, "[");
    else if (node->token->tokenval == FECHA_COLCHETES)
        printArquivo(treedot, "]");
    else if (node->token->tokenval == ABRE_PARENTESES)
        printArquivo(treedot, "(");
    else if (node->token->tokenval == FECHA_PARENTESES)
        printArquivo(treedot, ")");
    else if (node->token->tokenval == VIRGULA)
        printArquivo(treedot, ",");
    else if (node->token->tokenval == DOIS_PONTOS)
        printArquivo(treedot, ":");
    else if (node->token->tokenval == ATRIBUICAO)
        printArquivo(treedot, ":=");
    else if (node->token->tokenval == E_LOGICO)
        printArquivo(treedot, "&&");
    else if (node->token->tokenval == NEGACAO)
        printArquivo(treedot, "!");
    else if (node->token->tokenval == OU_LOGICO)
        printArquivo(treedot, "||");
    else if (node->token->tokenval == DIFERENTE)
        printArquivo(treedot, "<>");
    else if (node->token->tokenval == NAO_IDENTIFICADO)
        fprintf(treedot, "NÃO_IDENTIFICADO-%s", (char *) node->token->val);

    if(treedot != stdout)                    // se for diferente do stdout, então printa no arquivo
        printArquivo(treedot, "\"];\n");     // finalizando a linha
}

void printIdentacao(Stack *pilha) {
    unsigned short i;

    for(i = 0; i <= pilha->top; i ++) {
        if(pilha->array[i]) {
            printf("  \u2502"); // │
        } else {
            printf("  ");
        }
    }
}

void printArvoreT1(TreeNode *raiz, Stack *pilha) {
  // https://en.wikipedia.org/wiki/Box-drawing_character

    if (!raiz) {
        printf(" \u25cb\n");      // ○
        return;
    }

    printf("[");
    printLabel1(raiz, stdout);
    printf("]\n");

    char i;
    for(i = 0; 1; i ++) {

        if(!raiz->filhos[i+1]){             // se for o último filho

            printIdentacao(pilha);
            printf("  \u2514");     // └
            push(pilha, 0);
            printArvoreT1(raiz->filhos[i], pilha);
            pop(pilha);
            break;
        } else {
            printIdentacao(pilha);
            printf("  \u251c");     // ├
            push(pilha, 1);
            printArvoreT1(raiz->filhos[i], pilha);
            pop(pilha);
        }
    }
}

void printArvoreT(TreeNode *raiz) {
    Stack *pilha = createStack(1000);       // inicialmente com 1000
    printArvoreT1(raiz, pilha);
    printf("\n");
}
