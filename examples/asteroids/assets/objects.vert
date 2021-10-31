// Esse shader será usado para Nave, asteroids e tiros.

#version 410

layout(location = 0) in vec2 inPosition; // Só depende da posição do vértice (Já que todos os objetos serão brancos)

uniform vec4 color;
uniform float rotation;
uniform float scale;
uniform vec2 translation;

out vec4 fragColor;

void main() {
  float sinAngle = sin(rotation);
  float cosAngle = cos(rotation);
  // rotated é a nova posição do vértice depois de ser rotacionado no sentido anti-horário pelo angulo rotation
  vec2 rotated = vec2(inPosition.x * cosAngle - inPosition.y * sinAngle,
                      inPosition.x * sinAngle + inPosition.y * cosAngle);

  // O vértice é movimentado e redimensionado segundo os parâmetros scale e translation
  vec2 newPosition = rotated * scale + translation;
  gl_Position = vec4(newPosition, 0, 1);
  fragColor = color; // A cor passada para o fragmento será a cor Uniform, ou seja, a cor global
}