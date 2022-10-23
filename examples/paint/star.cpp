#include "star.hpp"

template <>
GLuint Shape<Star>::m_VAO_ = 0;
template <>
GLuint Shape<Star>::m_VBO_ = 0;
template <>
GLuint Shape<Star>::m_EBO_ = 0;
template <>
unsigned int Shape<Star>::m_EBOSize_ = 0;
template <>
const Shader* Shape<Star>::m_Shader_ = nullptr;
