Nome: Henrique Fantato                                                     --                                              RA: 21053916   

Nome: Eduardo Maciel de Souza                                              --                                              RA: 11055516

Professor: Harlen Batagelo                                                 --                                      Turma: A2-Noturno SA

       
# Entrega 3 – Aplicativo 3D com Textura e Iluminação - Jumping Block V2

***Links:***

   •	Repositório GitHub: https://github.com/edumacsou/abcg/tree/Entrega-3
  
   •	Repositório WebAssembly: 
  
   •	Página WebAssembly: 
  


***Descrição:***

  Implementamos uma animação gráfica onde dois cubos interagem entre si, o cubo vermelho está inicialmente em queda livre e, ao atingir o cubo maior azul, este 
 é deformado tal qual uma mola, impulsionando o cubo vermelho para cima. A aplicação foi realizada utilizando C++ e a biblioteca ABCg, bem como foi utilizado como
base o código "lookat" disponibilizado em aula pelo professor. Os objetos utilizados em tela são carregados de um arquivo modelo, que contém todos os vértices 
necesários, na função "initializeGL()", bem como as texturas utilizadas nos modelos são carregadas nesta mesma função. Os cálculos de todas as variáveis de posição e
escala são feitos na função "update()". A deformação do cubo azul é feita utilizando a função "glm:scale()", onde são passadas as variáveis calculadas na 
função "update()" para realizar a distorção, com base na posição dos cubos e na existência de colisão ou não.



ABCg
======

Development framework accompanying the course MCTA008-17 Computer Graphics at [UFABC](https://www.ufabc.edu.br/).

----

### Credits

Developed by Harlen Batagelo.

### License

ABCg is licensed under the MIT License. See [LICENSE](https://github.com/hbatagelo/abcg/blob/main/LICENSE) for more information.
