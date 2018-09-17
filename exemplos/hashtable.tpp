{fator é o fator do vetor da hash.
	{max_filhos é o tamanho máximo de mapeamento para cada chave.}
fator é a função hash.}

inteiro: fator, max_filhos, true
max_filhos := 200
fator := 200
true := 1

inteiro: hash[fator][max_filhos]

inicializaHash()
	inteiro: i, j
	i := 0
	j := 0

	repita
		repita
			hash[i][j] := -1
			j := j + 1
		até j < max_filhos

			j := 0
			i := i + 1
	até i < fator
fim

{função retorna o resto da divisão da chave pelo fator}
inteiro resto_da_divisao(inteiro: chave)
	repita
		se chave < fator então
			retorna(chave)
		senão
			chave := chave - fator
		fim
	até true
fim

{função retorna a posição da chave dentro da hash}
inteiro posicao(inteiro: chave)
	retorna(resto_da_divisao(chave))
fim

{função retorna os valores mapeados para a dada chave}
flutuante valor(inteiro: chave)
	retorna(hash[posicao(chave)])
fim

{função que adiciona o mapemento para a chave}
adiciona(inteiro: chave, flutuante: mapeamento)
	inteiro: pos
	pos := posicao(chave)

	{se o mapeamento for inválido}
	se chave < 0 || mapeamento = -1 então
		retorna(-1)
	fim

	{se for o primeiro mapemento}
	se hash[pos][0] = -1 então
		hash[pos][0] := mapeamento
		retorna(0)	{retorna a posição}
	senão

		inteiro: i
		i := 0

		repita		{repete até o próximo espaço vago}
			se hash[pos][i] = -1 então
				hash[pos][i] := mapeamento
				retorna(i)
			senão
				i := i + 1
			fim
		até i = max_filhos-1
	fim

	{se não conseguiu adicionar}
	retorna(-1)
fim

inteiro principal()
	inicializaHash()

	adiciona(-1, 187)
	adiciona(6, 763)
	adiciona(17, 0923)
	adiciona(27, 5)
	adiciona(37, 984)
	adiciona(6, 123)
	adiciona(2341, 123)
	adiciona(87, 98)
	adiciona(32, 54)
	adiciona(13, 55)
	adiciona(87, 1351)
	adiciona(13, 125)
	adiciona(56, 52)
	adiciona(179, 45)

	imprime(valor(1332))
	imprime(valor(142))
	imprime(valor(412))
	imprime(valor(81))
	imprime(valor(532))
	imprime(valor(4532))
	imprime(valor(26))
	imprime(valor(1765))
	imprime(valor(32))
	imprime(valor(131))
	imprime(valor(142))
	imprime(valor(42))
fim