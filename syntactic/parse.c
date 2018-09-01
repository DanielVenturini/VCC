#include "parse.h"
#include "../lexical/varredura.h"

TreeNode *parse(void) {

	TreeNode *raiz = novo_node();

}

int main(){
	TreeNode *arvore = novo_node('d');
	insere_filho(arvore, novo_node('a'));
	insere_filho(arvore, novo_node('n'));
	insere_filho(arvore, novo_node('i'));
	insere_filho(arvore, novo_node('e'));
	insere_filho(arvore, novo_node('l'));
	insere_filho(arvore->filhos[1], novo_node('5'));
	insere_filho(arvore->filhos[2], novo_node('6'));
	insere_filho(arvore->filhos[2], novo_node('8'));
	insere_filho(arvore->filhos[3], novo_node('7'));
	insere_filho(arvore->filhos[3], novo_node('9'));
	insere_filho(arvore->filhos[3], novo_node('4'));
	insere_filho(arvore->filhos[3]->filhos[2], novo_node('1'));
	insere_filho(arvore->filhos[4], novo_node('c'));

	printArvore(arvore);

	return 0;
}