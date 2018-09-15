# Venturini Compiler Compiler

Compilador em desenvolvimento para a disciplina de Compiladores. Este irá compilar para T++, uma linguagem desenvolvida propriamente para a disciplina, o gcc é melhor.

O VCC está sendo implementado para ser rápido e robusto. Não será usado funções prontas de bibliotecas, por exemplo, Flex. Assim, todas as funções e etapas serão desenvolvidas "não mão" para garantir que não há operações desnecessárias para obter o que precisa. Somente o que deve ser feito será, visando o máximo desempenho. Também a alocação de memória está sendo pensada para se usar o mínimo possível de alocação, tanto dos IDENTIFICADORES quanto das estruturas de dados. O principal motivo para a escolha da linguagem C foi o fato de esta ser muito flexivel quanto a alocação de memória, permitindo um gerenciamento de memória de maneira mais transparente.

![Alt Text](https://github.com/danielventurini/vcc/raw/master/syntactic/vcc.jpg)

Compilar um arquivo:
```
./vcc [flags] arquivo1.tpp arquivo2.tpp
```
Visualizar as opções de saídas da compilação dos arquivos:
```
./vcc -h
```
Para compilar com MAKE:
```
make
```
Para executar o arquivo 'teste.cpp':
```
make teste
```
ou
```
./vcc teste.cpp
```

A árvore é gerada gráficamente usando o software Graphviz. Para isso, é realizado a chamada de sistema para ```xdot```, passando o arquivo que é gerado navegando a árvore. O arquivo em questão é o ```tree.dot```