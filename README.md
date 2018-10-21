# Venturini Compiler Compiler

Compilador em desenvolvimento para a disciplina de Compiladores. Este irá compilar para T++, uma linguagem desenvolvida propriamente para a disciplina, o gcc é melhor.

O VCC está sendo implementado para ser rápido e robusto. Não será usado funções prontas de bibliotecas, por exemplo, Flex. Assim, todas as funções e etapas serão desenvolvidas "não mão" para garantir que não há operações desnecessárias para obter o que precisa. Somente o que deve ser feito será, visando o máximo desempenho. Também a alocação de memória está sendo pensada para se usar o mínimo possível de alocação, tanto dos IDENTIFICADORES quanto das estruturas de dados. O principal motivo para a escolha da linguagem C foi o fato de esta ser muito flexivel quanto a alocação de memória, permitindo um gerenciamento de memória de maneira mais transparente.

![Alt Text](https://github.com/danielventurini/vcc/raw/master/syntactic/vcc.jpg)

## Instação
Para instalar o vcc:

```
git clone https://github.com/danielventurini/vcc.git
cd vcc
sudo make install
```

O software ```xdot``` é instalado junto, pois é usado para exibir a árvore gerada na análise sintática. Porém, o ```vcc``` funcionará normalmente se este não for instalado. Para verificar se o ```xdot``` foi instalado, apenas execute ```xdot```.

Também na instalação, é copiado os arquivos necessários para o ```hithlight Syntax``` no ```Sublime Text 3```. As seguintes etapas devem ser realizadas para finalizar a instalação:

1. Abra um arquivo ```tpp``` no ```Sublime```;
2. Nas abas, navege em ```View->Syntax->Open all with current extension as ...->TPP```; e
3. Novamente nas abas, navege em ```Preferences->Color Scheme...-> tpp (User)```.

Se o ```Sublime``` não estiver instalado na máquina, ignore estas etapas.

## Execução
Compilar arquivos:
```
vcc [flags] [arquivo1.tpp arquivo2.tpp ...]
```
Visualizar as opções de saídas da compilação dos arquivos:
```
vcc -h
```
As flags desconhecidas são ignoradas e a compilação procede normalmente.

### Flags
Para cada uma das três primeiras partes da compilação, é possível verificar os resultados por meio de flags, que apresentam as saídas da compilação.

#### Tokens
A flag ```-tk``` ou ```--tokens``` apresentam os tokens de cada arquivo de entrada. Para os arquivos de entrada ```exemplos/teste.tpp``` e ```exemplos/hashtable.tpp```, o ```vcc``` gera a seguinte saída:

![Alt Text](https://github.com/danielventurini/vcc/raw/master/lexical/tokens.jpeg)

#### Árvore de Análise Sintática
A flag ```-ax``` ou ```--ast-x``` gera a saída da árvore de análise sintática. Esta árvore é gerada utilizando o software ```xdot```, que é instalado junto com o ```vcc```. Usando esta flag para o arquivo de entrada ```exemplos/teste.tpp```, o ```vcc``` gera a seguinte saída:

![Alt Text](https://github.com/danielventurini/vcc/raw/master/syntactic/ast-x.jpeg)