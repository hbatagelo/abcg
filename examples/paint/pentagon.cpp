#include "pentagon.hpp"

template <>
GLuint Shape<Pentagon>::m_VAO_ = 0;
template <>
GLuint Shape<Pentagon>::m_VBO_ = 0;
template <>
GLuint Shape<Pentagon>::m_EBO_ = 0;
template <>
unsigned int Shape<Pentagon>::m_EBOSize_ = 0;
template <>
const Shader* Shape<Pentagon>::m_Shader_ = nullptr;
