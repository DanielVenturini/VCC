#ifndef _GERACAO_H
#define _GERACAO_H

#include "../tree/tree.h"
#include "../semantic/tabsimb.h"

/*-----------------------------*
 * BIBLIOTECAS DO LLVM E CLANG *
 *-----------------------------*/
#include <llvm-c/BitWriter.h>
#include <llvm-c/Core.h>

/********************************************************************
*                   	DEFINICAO DAS FUNÇÕES                       *
********************************************************************/

void geraCodigo(TreeNode *, TabSimb *);

#endif