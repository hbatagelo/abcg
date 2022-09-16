#include "window.hpp"

void Window::onCreate() {
  createBuffers();
  createShaders();
}

void Window::onPaint(abcg::VulkanFrame const &frame) {
  // Execute the commands to draw onto the acquired image
  frame.commandBuffer.begin(
      {.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit});

  std::array<vk::ClearValue, 2> clearValues{};
  clearValues.at(0).setColor(
      {{{m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a}}});
  clearValues.at(1).setDepthStencil({1.0f, 0});

  // Main render pass
  frame.commandBuffer.beginRenderPass(
      {.renderPass = getSwapchain().getMainRenderPass(),
       .framebuffer = frame.framebufferMain,
       .renderArea = {.offset{}, .extent{getSwapchain().getExtent()}},
       .clearValueCount = clearValues.size(),
       .pClearValues = clearValues.data()},
      vk::SubpassContents::eInline);

  // Execute the commands to draw onto the acquired image
  frame.commandBuffer.bindPipeline(
      vk::PipelineBindPoint::eGraphics,
      static_cast<vk::Pipeline>(m_graphicsPipeline));

  // Record triangle drawing
  frame.commandBuffer.bindVertexBuffers(
      0, {static_cast<vk::Buffer>(m_vertexBuffer)}, {vk::DeviceSize{}});
  frame.commandBuffer.draw(static_cast<uint32_t>(m_vertices.size()), 1, 0, 0);

  frame.commandBuffer.endRenderPass();

  frame.commandBuffer.end();
}

void Window::onResize() {
  destroyGraphicsPipeline();
  createGraphicsPipeline();
}

void Window::onPaintUI() {
  // Parent class will show fullscreen button and FPS meter
  abcg::VulkanWindow::onPaintUI();

  // Our own ImGui widgets go below
  {
    // If this is the first frame, set initial position of our window
    static auto firstTime{true};
    if (firstTime) {
      ImGui::SetNextWindowPos(ImVec2(5, 75));
      firstTime = false;
    }

    // Window begin
    ImGui::Begin("Hello! This is a Dear ImGui window");

    // Static text
    ImGui::Text("Some example widgets are given below.");

    // Combo box
    {
      static auto currentIndex{0U};
      std::vector<std::string> const comboItems{"First item", "Second item",
                                                "Third item", "Fourth item"};

      if (ImGui::BeginCombo("A combo box",
                            comboItems.at(currentIndex).c_str())) {
        for (auto const index : iter::range(comboItems.size())) {
          bool const isSelected{currentIndex == index};
          if (ImGui::Selectable(comboItems.at(index).c_str(), isSelected))
            currentIndex = index;

          // Set the initial focus when opening the combo (scrolling +
          // keyboard navigation focus)
          if (isSelected)
            ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
      }
    }

    // Edit bools storing our window open/close state
    ImGui::Checkbox("Show demo window", &m_showDemoWindow);
    ImGui::Checkbox("Show another window", &m_showAnotherWindow);

    // Slider from 0.0f to 1.0f
    static auto value{0.0f};
    ImGui::SliderFloat("A slider", &value, 0.0f, 1.0f);

    // Edit background color
    ImGui::ColorEdit3("Background", &m_clearColor.r);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0 / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    // End of window
    ImGui::End();

    // Show the big demo window (Most of the sample code is in
    // ImGui::ShowDemoWindow()! You can browse its code to learn more about
    // Dear ImGui!).
    if (m_showDemoWindow) {
      ImGui::ShowDemoWindow(&m_showDemoWindow);
    }

    // Show another simple window
    if (m_showAnotherWindow) {
      // Pass a pointer to our bool variable (the window will have a closing
      // button that will clear the bool when clicked)
      ImGui::Begin("Another window", &m_showAnotherWindow);
      ImGui::Text("Hello from another window!");
      if (ImGui::Button("Close Me"))
        m_showAnotherWindow = false;
      ImGui::End();
    }
  }
}

void Window::onDestroy() {
  destroyGraphicsPipeline();
  destroyShaders();
  destroyBuffers();
}

void Window::createBuffers() {
  // Create vertex buffer
  m_vertexBuffer.create(getDevice(),
                        {.size = sizeof(m_vertices.at(0)) * m_vertices.size(),
                         .usage = vk::BufferUsageFlagBits::eVertexBuffer,
                         .properties = vk::MemoryPropertyFlagBits::eDeviceLocal,
                         .data = m_vertices.data()});
}

void Window::destroyBuffers() { m_vertexBuffer.destroy(); }

void Window::createShaders() {
  auto const assetsPath{abcg::Application::getAssetsPath()};

  // Crete vertex shader
  m_vertexShader.create(getDevice(),
                        {.source = assetsPath + "UnlitVertexColor.vert",
                         .stage = abcg::ShaderStage::Vertex});

  // Create fragment shader
  m_fragmentShader.create(getDevice(),
                          {.source = assetsPath + "UnlitVertexColor.frag",
                           .stage = abcg::ShaderStage::Fragment});
}

void Window::destroyShaders() {
  m_vertexShader.destroy();
  m_fragmentShader.destroy();
}

void Window::createGraphicsPipeline() {
  m_graphicsPipeline.create(
      getSwapchain(),
      {.shaders = {m_vertexShader, m_fragmentShader},
       .bindingDescriptions = {{.stride = sizeof(Vertex)}},
       .attributeDescriptions = {{.location = 0,
                                  .format = vk::Format::eR32G32Sfloat,
                                  .offset = offsetof(Vertex, pos)},
                                 {.location = 1,
                                  .format = vk::Format::eR32G32B32A32Sfloat,
                                  .offset = offsetof(Vertex, color)}},
       .inputAssemblyState = {.topology =
                                  vk::PrimitiveTopology::eTriangleList}});
}

void Window::destroyGraphicsPipeline() { m_graphicsPipeline.destroy(); }
