#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include "abcgOpenGL.hpp"

class Window : public abcg::OpenGLWindow {
protected:
  void onCreate() override;
  void onPaint() override;
  void onPaintUI() override;

private:
  std::array<float, 4> m_clearColor{0.906f, 0.910f, 0.918f, 1.0f};
};

#endif