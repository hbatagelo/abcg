#ifndef TRACKBALL_HPP_
#define TRACKBALL_HPP_

#include "abcg.hpp"

class TrackBall {
public:
  void mouseMove(glm::ivec2 const &position);
  void mousePress(glm::ivec2 const &position);
  void mouseRelease(glm::ivec2 const &position);
  void resizeViewport(glm::ivec2 const &size);

  [[nodiscard]] glm::mat4 getRotation() const;

  void setAxis(glm::vec3 const axis) noexcept { m_axis = axis; }
  void setVelocity(float const velocity) noexcept { m_velocity = velocity; }

private:
  constexpr static float m_maxVelocity{glm::radians(720.0f)};

  glm::vec3 m_axis{1.0f};
  glm::mat4 m_rotation{1.0f};

  glm::vec3 m_lastPosition{};
  abcg::Timer m_lastTime{};

  float m_velocity{};

  bool m_mouseTracking{};

  glm::ivec2 m_viewportSize{};

  [[nodiscard]] glm::vec3 project(glm::vec2 const &mousePosition) const;
};

#endif
