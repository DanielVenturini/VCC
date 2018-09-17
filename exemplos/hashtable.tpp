{fator é o fator do vetor da hash.}
{fator é a função hash}
inteiro: fator, true
fator := 200
true := 1

inteiro: hash[fator]

inicializaHash()
	inteiro: i
	i := 0

	repita
		hash[i] := 0
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

inteiro posicao(inteiro: chave)
	retorna(resto_da_divisao(chave))
fim

flutuante valor(inteiro: chave)
	retorna(hash[posicao(chave)])
fim

adiciona(inteiro: chave, flutuante: mapeamento)
	se hash[posicao(chave)] = 0 então
		hash[posicao(chave)] := mapeamento
		retorna(0)
	senão
		hash[posicao(chave)] := mapeamento
		retorna(1)
	fim
fim

inteiro principal()
fim