# Atividade 01 
Lucas Colonna Romano da Silva - 11202020710
Caique Rodrigues Do Nascimento - 11201722070

Para essa atividade optamos por desenvolver uma aplicação onde o usuário possa selecionar a quantidade máxima e mínima de vértices, inclusive para valores altos >30 a forma se aparenta muito com um círculo.  
Além disso, o usuário pode controlar o intervalo de geração e ao pressionar a barra de espaço troca-se a cor das formas para uma nova aleatória. 
A atividade fez uso da biblioteca ABCg e de parte do código desenvolvido na aula 5.1 (https://hbatagelo.github.io/cg/regularpolygons.html). Foram adicionados slides de controle, mudança de cor via barra espaço e a possibilidade de desenhar círculos quando a quantidade de vértices for suficientemente grande.

Para a captura do evento temos: 
```void Window::onEvent(SDL_Event const &event) {
  // Keyboard events
  if (event.type == SDL_KEYDOWN) {
    if (event.key.keysym.sym == SDLK_SPACE){
      std::uniform_real_distribution rd(0.0f, 1.0f);
      color3 = {rd(m_randomEngine), rd(m_randomEngine),
                              rd(m_randomEngine)};
      //std::cout << "Space key pressed. New color: " << color3.x << ", " << color3.y << ", " << color3.z << std::endl;
    }
  } 
```
E uma variável declarada em window.hpp para fazer o rastreio da cor. 

## License

ABCg is licensed under the MIT License. See [LICENSE](https://github.com/hbatagelo/abcg/blob/main/LICENSE) for more information.


