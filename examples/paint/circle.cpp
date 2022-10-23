#include "circle.hpp"

template <>
GLuint Shape<Circle>::m_VAO_ = 0;
template <>
GLuint Shape<Circle>::m_VBO_ = 0;
template <>
GLuint Shape<Circle>::m_EBO_ = 0;
template <>
unsigned int Shape<Circle>::m_EBOSize_ = 0;
template <>
const Shader* Shape<Circle>::m_Shader_ = nullptr;
