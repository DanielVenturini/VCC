inteiro: n

inteiro fatorial(inteiro: n)
	inteiro: fat
	se n > 0 entao {não calcula se n > 0}
		fat := 1
		repita
			fat := fat * n
			n := n - 1
		ate n = 0
		retorna(fat) {retorna o valor do fatorial de n}
	senao
		retorna(0)
	fim
fim

inteiro principal()
	leia(n)
	escreva(fatorial(n))
	retorna(0)
fim