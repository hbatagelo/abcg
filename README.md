## Autores:

Matheus Augusto Correa de Oliveira, **RA:** 11201721710\
Henrique Jotten Mendonca de Souza, **RA:** 11045812

## Links:

[Código GitHub](https://github.com/Matheuzs/abcg)  
[GitHub Pages](https://matheuzs.github.io/abcg/pacmanRain/)

## Descrição do Projeto:

*Pac-Man Rain* é um jogo 2D que utiliza a biblioteca abcg e é baseado no projeto [Asteroids](https://hbatagelo.github.io/abcgapps/asteroids/index.html), o objetivo do jogo é que você fuja com o Fantasma dos Pac-Mans que estão caindo do topo da tela, toda a movimentação do fantasma é feita com as teclas <kbd>A</kbd> / <kbd>D</kbd> ou <kbd>←</kbd> / <kbd>→</kbd> se movimentando apenas no eixo X da tela.\
O jogo termina em Vitória se o fantasma sobreviver sem colidir com os Pac-Mans por 60 segundos (Timer na tela) e em Game Over em caso de colisão, em ambos o jogo resetará após 5 segundos da sua conclusão.

![alt text](https://github.com/Matheuzs/abcg/blob/master/images/pacmanrain.png?raw=true)

## Detalhamento do Código:

* **gamedata.hpp**: Arquivo de cabeçalho que implementa enums e struct para os estados do jogo e para a movimentacao.\
Os estados são "Playing", "GameOver" e "Win".\
A movimentação é "Right" e "Left".

* **ghost.cpp / ghost.hpp:** Classes que implementam os comportamentos/atributos do fantasma no jogo, ele é renderizado utilizando 17 vértices com GL_TRIANGLES, as coordenadas dos vértices são normalizadas no intervalo (15.5, 15.5).\
Na função update(), fazemos o contorno para que o fantasma não saia da visão da tela.

* **main.cpp:** Nela utilizamos o abcg e definimos algumas configurções como o tamanho da tela (600x600) e título.

* **openglwindow.cpp / openglwindow.hpp:** Classe principal onde definimos todos os comportamentos da tela opengl e chamamos as classes ghost e pacmans, nesta implementamos a utilizacao do SDL na função handleEvent() que define ocomportamento das teclasna movimentacao.
Em initializeGL(), carregamos as fontes que serão usadas nos textos e os shaders que estão na pasta assets.
É tembém nesta classe que implementamos a função de colisão e os casos de WIN / LOSE.

* **pacmans.cpp / pacmans.hpp:** Implementa os comportamentos/atributos dos pacmans que caem do topo da tela, a classe gera os pacmans na posicao (aleatória, 1.5) de 3 em 3 segundos através da função generatePacmans().
A geometria dos pacmans é gerada com 10 lados e utilizando GL_TRIANGLE_FAN.







