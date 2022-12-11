# Boid Simulator
## Simulação de boids

## Nome: Gabriel Jorge Menezes
## RA: 11201921315

## Nome: Rodrigo Cabrera
## RA: 21005515

## **Introdução**

Esse projeto consiste na continuação do trabalho do ***Boids*** (Atividade 3) com a adição de shaders de texturização e iluminação.

## **Como foi feito**

### **Iluminação**

Inicialmente foi adicionado no struct do vértice um vetor normal (**normal (vec3)**) e um vetor de textura (**texCoord (vec2)**). Além disso, é necessário atualizarmos a função hash do vértice para levar em consideração essas novas componentes. Isso é importante porque agora vértices do objeto com a mesma posição podem ser diferentes dependendo das componentes do vetor normal e de textura.

Nos arquivos **light** (**light.hpp** e **light.cpp**) são implementados três tipos de luz diferentes com suas respectivas interfaces ImGui, conforme apresentado na Aula 9 secção 2 na parte *Fontes de luz*:

- **Luz direcional**: Ilumina todo mundo com a mesma intensidade. Como não tem posição no espaço não calcula-se a atenuação.

- **Luz pontual**: Ilumina em toda a direção e a intensidade diminui conforme a distância aumenta. 

- **Luz Spot**: Ilumina em formato de cone (parecido com uma lanterna) e a intensidade diminui conforme a distância aumenta.

No espaço temos uma luz direcional e cinco fontes de luz pontuais e spot. No arquivo **boid.frag** na função *CalcLight* é calculada a intensidade de Blinn–Phong (Aula 9 secção 3), a qual é utilizada pelos 3 tipos de fonte luz em suas respectivas funções (*DirectLightContribution*, *CalcPointLight*, *CalcSpotLight*). Por último na função *main* ocorre a soma da contribuição de cada fonte de luz no cenário, permitindo atribuir aos vértices a cor de maneira proporcional a iluminação que eles recebem criando os efeitos visuais.

Vale notar que calculamos a equação do modelo de iluminação para cada fragmento, logo estamos utilizando o sombreamento de Phong que obtém melhor resultado, mas é mais custoso quando comparado ao modelo Flat ou de Gouraud. 

### **Textura**

Na classe **Boid** foi implementada um struct denominado **Material** usado para implementar a textura dos boids. Nessa parte foi implementado uma textura difusa, conforme exposto na Aula 10. 

Esse processo foi realizado na função *CalcLight* do fragment shader **boid.frag**. Então é calculado a textura utilizando a função ***texture***, posteriormente utilizada para alterar as componentes de cor *ambient* e *diffuse*. 

## **Funcionlidades ImGui Adicionadas**

- Na ImGui é possível ativar ou desativar cada fonte de luz. Permitindo estudar como cada contribuição afeta o resultado final.
- Para as fontes de luz pontuais e spot é possível alterar:
    - Suas posições no espeço alterando as componentes x,y e z. 
    - Sua atenuação alterando a componente constante, linear ou quadrática da equação de atenuação utilizada.
    - As componentes: ambiente (Ia), difusa (Id) e especular (Is).
- Em especial para as fonte de luz spot podemos tambem alterar a posição apontada pela fonte de luz (lanterna), alterando as variáveis de rotação e o *cut off angle*.
- Para a fonte de luz direcional apenas podemos mudar sua intensidade (variável *strength*), posição (variáveis x,y e z) e as componentes ambiente (Ia), difusa (Id) e especular (Is).
