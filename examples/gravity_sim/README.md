# Gravity Sim
## Simulador de gravidade utilizando lei Gravitacional de Newton

---

## Comandos
* Botão esquerdo: Adiciona uma particula
* Arrastar botão esquerdo: Adiciona uma particula com velocidade inicial.

---

## Como foi feito
Foi utilizado a feature de [DrawList](https://github.com/ocornut/imgui/blob/master/imgui_demo.cpp) para criar a área, borda, particulas, linha de disparo.

A parte de criacao do canvas, borda e evento sobre o canvas foram retirados do [imgui_demo.cpp](https://github.com/ocornut/imgui/blob/master/imgui_demo.cpp#L7642).

## Como funciona
Todos os calculos matematicos foram baseados em geometria analitica utilizando funcionalidades do glm.

Cada particula é uma instancia de uma classe chamada [Particle](particle.hpp) que armazena informações pertinentes a particula (posição, velocidade, cor, massa, raio, momento, aceleração, força resultante, se foi absorvida por outra particula)

Para cada frame:
1. Desenhamos o canvas e caputramos eventos sobre o mesmo
2. Verificamos eventos sobre o canvas e caso o botão esquero seja clicado ou arrastado adicionamos uma particula com velocidade: pos mouse inicial - pos mouse final.
3. Reiniciamos a força resultante de todas as particulas (calculado no frame anterior)
4. Verifica se houve colisão entre duas particulas (iteramos sobre todos os possiveis pares de particulas), caso ocorra uma colisão marcamos uma das particulas como absorvida para evitar que seja verificada numa proxima iteração e absorvemos esta particula.
5. Removemos as particulas absorvidas
6. Desenhamos cada particula e verificamos a interação entre a particula atual e todas as outras particulas do sistema, sendo o resultado é acumulado no vetor de força resultante 
7. Atualizamos a posição da particula atual com base na aceleração gerada pela força resultante, utilizando o delta time entre o frame anterior o atual

## Parametros controlaveis
* Gravidade
* Limpar todas as particulas
* O exponte da massa da particula (2^n)
* Cor da particula (gerado aleatoriamente cada vez que uma particula é adicionada)


## Observações
Alguns propriedades das particulas são levemente alteradas para tornar a simulação mais utilizavel
* Velocidade inicial: Como o espaço do canvas é relativamente grande so calcular pos mouse inicial - pos mouse final resultaria numa velocidade inicial muito alta, com isto multiplicamos por uma contantes pare reduzir este valor
* Velocidade calculada a frame: Motivo é similar ao anterior
