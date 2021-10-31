Nome: Henrique Fantato                                                                                                   RA: 21053916         
Nome: Eduardo Maciel de Souza                                                                                            RA: 11055516
Professor: Harlen Batagelo                                                                                       Turma: A2-Noturno SA
       
Entrega – Aplicativo 2D Interativo - Pong

•	Links:
  o	Repositório GitHub: https://github.com/edumacsou/abcg/tree/main
  o	Repositório WebAssembly: https://github.com/edumacsou/abcg/tree/main/public
  o	Página WebAssembly: https://edumacsou.github.io/abcg/
  Observação: Criamos a página em WebAssembly utilizando o tutorial informado no site da disciplina, porém é gerado um erro no arquivo javascript e a página não funciona           corretamente. O nome dos arquivos contidos na página são derivações de hello world, pois o próprio script cria com essa nomenclatura, caso contrário ele falha.


•	Descrição:

  Implementamos o jogo clássico Pong, utilizando a biblioteca ABCg e C++. Foram criadas as geometrias dos objetos desenhados em tela e, 
 com base na variável de translação do shader utilizado, realizamos toda a movimentação do jogo. 
  Foram criadas funções para atualização de posição das raquetes dos jogadores, bem como uma função para controlar a velocidade delas com base num EventHandler, 
 onde as raquetes podem ser controladas com as teclas W e S(raquete esquerda), e as setas Up e Down(raquete direita). Também foi implementado uma função que 
 controla a bolinha, realizando atualização de sua posição em tela, bem como atualização de sua velocidade e uma decetção de colisão com os objetos em cena. 





ABCg
======

Development framework accompanying the course MCTA008-17 Computer Graphics at [UFABC](https://www.ufabc.edu.br/).

----

### Credits

Developed by Harlen Batagelo.

### License

ABCg is licensed under the MIT License. See [LICENSE](https://github.com/hbatagelo/abcg/blob/main/LICENSE) for more information.
