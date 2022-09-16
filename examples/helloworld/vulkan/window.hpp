#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include "abcgVulkan.hpp"

class Window : public abcg::VulkanWindow {
protected:
  void onCreate() override;
  void onPaint(abcg::VulkanFrame const &frame) override;
  void onPaintUI() override;
  void onResize() override;
  void onDestroy() override;

private:
  struct Vertex {
    glm::vec2 pos{};
    glm::vec4 color{};
  };

  std::array<Vertex, 3> const m_vertices = {
      Vertex{.pos = {0.0f, -0.5f}, .color = {1.0f, 0.0f, 0.0f, 1.0f}},
      Vertex{.pos = {0.5f, 0.5f}, .color = {0.0f, 1.0f, 0.0f, 1.0f}},
      Vertex{.pos = {-0.5f, 0.5f}, .color = {0.0f, 0.0f, 1.0f, 1.0f}}};

  abcg::VulkanPipeline m_graphicsPipeline{};
  abcg::VulkanShader m_vertexShader{};
  abcg::VulkanShader m_fragmentShader{};
  abcg::VulkanBuffer m_vertexBuffer{};

  bool m_showDemoWindow{false};
  bool m_showAnotherWindow{false};
  glm::vec4 m_clearColor{0.906f, 0.910f, 0.918f, 1.00f};

  void createBuffers();
  void destroyBuffers();
  void createShaders();
  void destroyShaders();
  void createGraphicsPipeline();
  void destroyGraphicsPipeline();
};

#endif