#ifndef _DESACERTO_H
#define _DESACERTO_H

#include <stdio.h>
#include "lexical/varredura.h"

/********************************************************************
*                   	DEFINICAO DAS FUNÇÕES                       *
********************************************************************/
void erro(char *nomeArquivo, TokenRecord *token, char *msgErro, char sair, char erro);
void warnig(char *nomeArquivo, TokenRecord *token, char *msgErro);

#endif