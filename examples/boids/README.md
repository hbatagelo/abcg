# Boid Simulator
## Simulação de boids

## Nome: Gabriel Jorge Menezes
## RA: 11201921315

## Nome: Rodrigo Cabrera
## RA: 21005515

## Introdução 

Esse projeto consiste em criar um programa de ***Boids*** 3D (do inglês, *bird-oid object*), criado por Craig Reynolds em 1986. Esse programa se enquadra na categoria de algoritmos de vida artificial, onde um conjunto de regras simples aplicadas em um sistema com vários objetos apresentam comportamentos complexos, como exemplo de tais programas temos o jogo da vida de John Conway. 

No caso especifico do ***Boids***, espera-se que apartir de 3 simples regras seja possível simular um comportamento do rebanho.

## Comandos

Os comandos de mouse e teclado foram implementados de forma a simular câmeras normalmente utilizadas em jogos de FPS (do inglês, *First-person shooter*):

* **W/S**: Representa o movimento ***dolly*** (jargão cinematográfico).
* **A/D**: Representa o movimento ***pan*** (jargão cinematográfico).
* **Mouse**: Muda a posição para qual a câmera está olhando.

Os comandos de ***dolly*** (**W/S**) e ***pan*** (**A/D**) são implementados alterando-se a posição da câmera (na coordenada do mundo), representada pela variável ```m_Eye_```.

O comando relacionado ao **Mouse** é implementado ao se alterar a posição na qual a câmera está olhando (na coordenada do mundo), representada pela variável ```m_Front_```.

A implementação pode ser encontrada no arquivo [camera.cpp](camera.cpp), e sua respectiva interface pode ser encontrada no arquivo [camera.hpp](camera.hpp).
