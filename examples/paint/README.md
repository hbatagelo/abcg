# Paint
## Aplicação que imita Microsoft Paint

## Nome: Gabriel Jorge Menezes
## RA: 11201921315

## Comandos
* Botão esquerdo: Adiciona uma forma
* Esc: Limpa todas as formas
* Up: Forma anterior
* Down: Proxima forma

## Como foi feito
Foi utilizado polimorfismo mais especificamente [CRTP](https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern) para permitir que cada classe derivada tivesse membros estáticos que represantam os objetos do OpenGL, com isso não precisamos recriar estes objetos a cada nova instancia.

Os objetos são:
* [Circle](circle.hpp), este usa um [shader específico](assets/circle.frag), pois gerar circulos em shader é mais eficiente
* [Pentagon](pentagon.hpp)
* [Rectangle](rectangle.hpp)
* [Star](star.hpp)
* [Triangle](triangle.hpp)

## Como funciona
Cada um destes objetos são contruidos em um `vector` de [BaseShape](shape.hpp) seus respectivos métodos são chamados.

Cada instancia de uma forma tem uma matriz de transformação que matém a escala, rotação e translação.

**onCreate**:
1. Cada forma tem uma função estatica chamada `setup` que configura os buffers necessarios. Esta função deve ser chamado no `onCreate`.
2. Os shaders são compilados para serem reutilizados no futuro por cada instancia de uma forma.

**Para cada frame**:
1. Verificamos se o mouse esquerdo foi cliado. Se sim calculamos o `NDC` a partir da posição atual do mouse na tela e criamos a forma requisitada.
2. Desehamos a `GUI` de cada forma
3. Desenhamos cada forma

## Parametros controlaveis
* Cor da proxima forma
* Cor de fundo
* Para as formas já criadas
    1. Cor
    2. Aplição de um efeito especial no shader (se ativado permite controlar alguns outros parametros)
    3. Translação
    4. Escala
    5. Rotação
    6. Excluir

## Observações
Toda parte de polimorfismo foi feita para permitir criar novas formas de forma simples, pois todo o trabalho é feito pela classe pai ou seja as classes filhas so precisam definir os pontos do VBO e os indices do EBO
