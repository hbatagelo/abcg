# Boid Simulator
## Simulação de boids

## Nome: Gabriel Jorge Menezes
## RA: 11201921315

## Nome: Rodrigo Cabrera
## RA: 21005515

## Introdução 

Esse projeto consiste em criar um programa de ***Boids*** 3D (do inglês, *bird-oid object*), criado por Craig Reynolds em 1986. Esse programa se enquadra na categoria de algoritmos de vida artificial, onde um conjunto de regras simples aplicadas em um sistema com vários objetos apresentam comportamentos complexos, como exemplo de tais programas temos o jogo da vida de John Conway. 

No caso especifico do ***Boids***, espera-se que apartir de 3 simples regras seja possível simular um comportamento de rebanho.

## Como foi feito

A classe ```Space``` representa a região no formato de um cubo onde os ***boids*** vão se mover. Essa classe implementam as seguintes  funções:

* **setup**: Responsável por criar os vértices e índices (especificados em código), carregar o vertex  e fragment shader, criar os buffers e fazer os *binds* necessários.

* **show**: Responsável por desenhar aresta por aresta utilizando o comando ***GL_LINES***. Essa lógica está implementada no arquivo [space.cpp](./space.cpp) e sua respectiva interface [space.hpp](./space.hpp).

<font color = "red"><b>TODO: CHECAR</b></font>

O vertex shader utilizado está no arquivo: [shader.vert](./assets/shader.vert) e contém somente as multiplicações matrizes que permitem transformar do espaço do objeto para o espaço da câmera com perspectiva. 

O fragment shader utilizado está no arquivo: [space.frag](./assets/space.frag) e contém somente o output de uma cor.

---
A classe ```Boid```, onde os principais métodos são:

* **checkEdge**: Checa se um boid está dentro do espaço permitido e o reposiciona caso necessário.

* **simulate**: Implementa as regras que permitem o comportamento de rebanho.

* **update**: Realiza o update da posição e velocidade dos boids.

* **show**: Envia as matrizes necessárias para os shaders e desenha os triângulos.

* **setup**: Responsável por criar os buffers.

* **showUI**: Responsável por desenhar a ImGui na tela.

O vertex shader utilizado está no arquivo: [shader.vert](./assets/shader.vert) e contém somente as multiplicações matrizes que permitem transformar do espaço do objeto para o espaço da câmera com perspectiva. 

<font color = "red"><b>TODO: CHECAR</b></font>

O fragment shader utilizado está no arquivo: [boid.frag](./assets/boid.frag) e contém somente o output de uma cor.

## Como funciona

Regras:

1. Os Boids devem desviar de outros boids.
2. Os Boids devem se mover na mesma direção de boids próximos.
3. Todo Boid deve se mover para o centro de boids próximos.


## Comandos

Os comandos de mouse e teclado foram implementados de forma a simular câmeras normalmente utilizadas em jogos de FPS (do inglês, *First-person shooter*):

* **W/S**: Representa o movimento ***dolly*** (jargão cinematográfico).
* **A/D**: Representa o movimento ***pan*** (jargão cinematográfico).
* **Mouse**: Muda a posição para qual a câmera está olhando.

Os comandos de ***dolly*** (**W/S**) e ***pan*** (**A/D**) são implementados alterando-se a posição da câmera (na coordenada do mundo), representada pela variável ```m_Eye_```.

O comando relacionado ao **Mouse** é implementado ao se alterar a posição na qual a câmera está olhando (na coordenada do mundo), representada pela variável ```m_Front_```.

A implementação pode ser encontrada no arquivo [camera.cpp](camera.cpp), e sua respectiva interface pode ser encontrada no arquivo [camera.hpp](camera.hpp).

## Parametros controlaveis

Boids:

* **Alignment**: Velocidade média dos boids vizinhos.
* **Cohesion**: Posição média dos boids vizinhos.
* **Separation**: Evita colisões (inversamente proporcional a distância).
* **Perception Radius**: Altera a esfera de percepção dos boids.
* **Maximum Force**: xxxx
* **Velocity**: Aumenta a velocidade dos boids.

* **Number of birds**: Muda a quantidade de boids no espaço.


## Observações

O projeto teve como inspiração esse [Vídeo](https://www.youtube.com/watch?v=bqtqltqcQhw).
