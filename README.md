# First-Follow

Projeto com a implementação das funções First-Follow e table para um algoritmo preditivo (analisador sintático).

A entrada do programa é um arquivo de texto com uma grámatica para análise, seguindo o seguinte padrão:

*X-aXb|e*


  * Todos os símbolos devem ter somente **um** charactere
  * As letras **maiúsculas** representam estados **Não-Terminais**
  * As letras **minúsculas** representam estados **Terminais**
  * A letra **e** represeta nosso "ε"(*epsilon*), portando lembrar de **não** utilizá-la para representar um terminal
  * $ representa nosso símbolo final
  * O símbolo "|" (*pipe*) separa produções
      * Pode-se optar pela não utilização do "pipe", basta escrever a produção em uma linha separada
      

Para executar o código:
```sh
$ make
$ ./a.out -f <input file name>
```
Você pode também escrever o resultado em um arquivo:
```sh
$ make
$ ./a.out -f <input file name> -o <result file name>
```

Após os passos acima, será impresso na tela os conjuntos *First*, *Follow* e a tabela construida e que pode ser utilizada pelo algoritmo preditivo.
