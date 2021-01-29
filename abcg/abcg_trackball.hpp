/**
 * @file abcg_trackball.hpp
 * @brief abcg::TrackBall header file.
 *
 * Declaration of abcg::TrackBall class.
 *
 * This project is released under the MIT License.
 */

#ifndef ABCG_TRACKBALL_HPP_
#define ABCG_TRACKBALL_HPP_

#include <glm/ext/vector_int2.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/vec3.hpp>

#include "abcg_elapsedtimer.hpp"

namespace abcg {
class TrackBall;
}  // namespace abcg

/**
 * @brief abcg::TrackBall class.
 *
 */
class abcg::TrackBall {
 public:
  void mouseMove(const glm::ivec2& mousePosition);
  void mousePress(const glm::ivec2& mousePosition);
  void mouseRelease(const glm::ivec2& mousePosition);
  void resizeViewport(int width, int height);

  [[nodiscard]] glm::quat getRotation();

 private:
  const float m_maxVelocity{glm::radians(720.0f / 1000.0f)};

  glm::vec3 m_axis{};
  float m_velocity{};
  glm::quat m_rotation{glm::quat(1.0, 0.0, 0.0, 0.0)};

  glm::vec3 m_lastPosition{};
  ElapsedTimer m_lastTime{};
  bool m_mouseTracking{};

  float m_viewportWidth{};
  float m_viewportHeight{};

  [[nodiscard]] glm::vec3 project(const glm::vec2& mousePosition) const;
};

#endif