#include "triangle.hpp"
#include <vector>

template <>
GLuint Shape<Triangle>::m_VAO_ = 0;
template <>
GLuint Shape<Triangle>::m_VBO_ = 0;
template <>
GLuint Shape<Triangle>::m_EBO_ = 0;
template <>
unsigned int Shape<Triangle>::m_EBOSize_ = 0;
template <>
const Shader* Shape<Triangle>::m_Shader_ = nullptr;
