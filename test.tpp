{inteiro: n

inteiro fatorial(inteiro: n)
	inteiro: string_de_tamanho_grande_para_teste_de_realocacao
	inteiro: test_float
	test_float: 402331909.123
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
}

inteiro: a, b
inteiro principal()
	inteiro: c
	a := 1
	b := 2.3
	c := a + b

	se(c > 2) entao
		escreva(c)
	senao
		escreva(b)
	fim
	retorna(0)
fim