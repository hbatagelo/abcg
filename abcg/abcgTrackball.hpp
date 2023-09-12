/**
 * @file abcgTrackball.hpp
 * @brief Header file of abcg::TrackBall.
 *
 * Declaration of abcg::TrackBall class.
 *
 * This file is part of ABCg (https://github.com/hbatagelo/abcg).
 *
 * @copyright (c) 2021--2023 Harlen Batagelo. All rights reserved.
 * This project is released under the MIT License.
 */

#ifndef ABCG_TRACKBALL_HPP_
#define ABCG_TRACKBALL_HPP_

#include "abcgExternal.hpp"
#include "abcgTimer.hpp"

namespace abcg {
class TrackBall;
} // namespace abcg

/**
 * @brief Manages a virtual trackball.
 *
 */
class abcg::TrackBall {
public:
  void mouseMove(glm::ivec2 const &position);
  void mousePress(glm::ivec2 const &position);
  void mouseRelease(glm::ivec2 const &position);
  void resizeViewport(glm::ivec2 const &size) noexcept;

  [[nodiscard]] glm::quat getRotation() const;

  void setAxis(glm::vec3 axis) noexcept;
  void setVelocity(float velocity) noexcept;

private:
  constexpr static float m_maxVelocity{glm::radians(720.0f / 1000.0f)};

  glm::vec3 m_axis{1.0f};
  float m_velocity{};
  glm::quat m_rotation{glm::quat(1.0, 0.0, 0.0, 0.0)};

  float m_durationSinceLastEvent{};

  glm::vec3 m_lastPosition{};
  Timer m_lastTime;
  bool m_mouseTracking{};

  glm::ivec2 m_viewportSize{0};

  [[nodiscard]] glm::vec3 project(glm::vec2 const &mousePosition) const;
};

#endif
