Nome: Henrique Fantato                                                     --                                              RA: 21053916   

Nome: Eduardo Maciel de Souza                                              --                                              RA: 11055516

Professor: Harlen Batagelo                                                 --                                      Turma: A2-Noturno SA

       
# Entrega 3 – Aplicativo 3D com Textura e Iluminação - Jumping Block V2

***Links:***

   •	Repositório GitHub: https://github.com/edumacsou/abcg/tree/Entrega-3
  
   •	Repositório WebAssembly: 
  
   •	Página WebAssembly: https://edumacsou.github.io/abcg/public/lookat.html
  


***Descrição:***

  Implementamos uma aplicação gráfica interativa onde dois cubos coloridos interagem entre si, o cubo vermelho está inicialmente em queda livre e, ao atingir o cubo maior, este é deformado tal qual uma mola, impulsionando o cubo vermelho para cima. A aplicação foi realizada utilizando C++ e a biblioteca ABCg, foi utilizado como base o código "lookat" disponibilizado em aula pelo professor.  
  Os objetos utilizados em tela são carregados de um arquivo modelo, que contém todos os vértices necesários, na função "initializeGL()", bem como as texturas utilizadas nos modelos são carregadas nesta mesma função. Os cálculos de todas as variáveis de posição e escala são feitos na função "update()". A deformação do cubo maior é feita utilizando a função "glm:scale()", onde são passadas as variáveis calculadas na função "update()" para realizar a distorção, com base na posição dos cubos e na existência de colisão ou não.

  A aplicação se baseia em um jogo, no qual o objetivo é coincidir a cor do bloco saltitante com a cor do bloco maior. Se as cores dos blocos forem distintas no momento que os dois blocos colidem, então será exibida uma tela de 'Game Over' na tela.
  Se o jogador acertar 14 pulos consecutivos a mensagem de vitória é exibida na tela.
  As mensagens permanecerão na tela por 5 segundos então o jogo reiniciará automáticamente.  
  Para alterar a cor do bloco saltitante use as teclas 'J', 'K', e 'L', conforme a seguir:  
  J -> Altera a cor para Vermelho  
  K -> Altera a cor para Verde  
  L -> Altera a cor para Azul  
 

ABCg
======

Development framework accompanying the course MCTA008-17 Computer Graphics at [UFABC](https://www.ufabc.edu.br/).

----

### Credits

Developed by Harlen Batagelo.

### License

ABCg is licensed under the MIT License. See [LICENSE](https://github.com/hbatagelo/abcg/blob/main/LICENSE) for more information.
