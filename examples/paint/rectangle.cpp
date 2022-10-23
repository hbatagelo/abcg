#include "rectangle.hpp"

template <>
GLuint Shape<Rectangle>::m_VAO_ = 0;
template <>
GLuint Shape<Rectangle>::m_VBO_ = 0;
template <>
GLuint Shape<Rectangle>::m_EBO_ = 0;
template <>
unsigned int Shape<Rectangle>::m_EBOSize_ = 0;
template <>
const Shader* Shape<Rectangle>::m_Shader_ = nullptr;
