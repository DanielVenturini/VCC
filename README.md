# Venturini Compiler Compiler

Compilador em desenvolvimento para a disciplina de Compiladores. Este irá compilar para T++, uma linguagem desenvolvida propriamente para a disciplina, o gcc é melhor.

O VCC está sendo implementado para ser rápido e robusto. Não será usado funções prontas de bibliotecas, por exemplo, Flex. Assim, todas as funções e etapas serão desenvolvidas "não mão" para garantir que não há operações desnecessárias para obter o que precisa. Somente o que deve ser feito será, visando o máximo desempenho. Também a alocação de memória está sendo pensada para se usar o mínimo possível de alocação, tanto dos IDENTIFICADORES quanto das estruturas de dados. O principal motivo para a escolha da linguagem C foi o fato de esta ser muito flexivel quanto a alocação de memória, permitindo um gerenciamento de memória de maneira mais transparente.

![Alt Text](https://github.com/danielventurini/vcc/raw/master/syntactic/vcc.jpg)

Para instalar o vcc:

```
git clone https://github.com/danielventurini/vcc
cd vcc
sudo make install
```

Compilar um arquivo:
```
vcc [flags] [arquivo1.tpp arquivo2.tpp ...]
```
Visualizar as opções de saídas da compilação dos arquivos:
```
./vcc -h
```

Na instalação, o software ```Graphviz``` é instalado junto, pois é usado para exibir a árvore gerada na análise sintática. Porém, o ```vcc``` funcionará normalmente se este não for instalado. Para verificar se o ```Graphviz``` foi instalado, execute ```xdot```.