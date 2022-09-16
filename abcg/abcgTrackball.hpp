/**
 * @file abcgTrackball.hpp
 * @brief Header file of abcg::TrackBall.
 *
 * Declaration of abcg::TrackBall class.
 *
 * This file is part of ABCg (https://github.com/hbatagelo/abcg).
 *
 * @copyright (c) 2021--2022 Harlen Batagelo. All rights reserved.
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

  /**
   * @brief Sets the trackball's axis of rotation.
   *
   * @param axis Axis of rotation.
   *
   * The axis is expected to be a normalized vector.
   */
  void setAxis(glm::vec3 const axis) noexcept { m_axis = axis; }

  /**
   * @brief Sets the trackball's rotation velocity.
   *
   * @param velocity Velocity in radians per second.
   *
   * The velocity is not bound checked.
   */
  void setVelocity(float const velocity) noexcept { m_velocity = velocity; }

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
