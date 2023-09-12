/**
 * @file abcgTrackball.cpp
 * @brief Definition of abcg::TrackBall members.
 *
 * This file is part of ABCg (https://github.com/hbatagelo/abcg).
 *
 * @copyright (c) 2021--2023 Harlen Batagelo. All rights reserved.
 * This project is released under the MIT License.
 */

#include "abcgTrackball.hpp"

#include <algorithm>
#include <limits>

constexpr static auto epsilon{std::numeric_limits<float>::epsilon()};

/**
 * @brief Function to be called on a mouse move event.
 *
 * @param position Position of the mouse cursor in window coordinates.
 */
void abcg::TrackBall::mouseMove(glm::ivec2 const &position) {
  if (!m_mouseTracking)
    return;

  m_durationSinceLastEvent =
      gsl::narrow_cast<float>(m_lastTime.restart()) * 1000.0f;

  // Return if mouse cursor hasn't moved wrt last position
  auto const currentPosition{project(position)};
  if (glm::all(glm::epsilonEqual(m_lastPosition, currentPosition, epsilon)))
    return;

  // Rotation axis
  m_axis = glm::cross(m_lastPosition, currentPosition);

  // Rotation angle
  auto const angle{glm::length(m_axis)};

  m_axis = glm::normalize(m_axis);

  // Compute an angle velocity that will be used as a constant rotation angle
  // when the mouse is not being tracked.
  m_velocity = angle / (m_durationSinceLastEvent + epsilon);
  m_velocity = glm::clamp(m_velocity, 0.0f, m_maxVelocity);

  // Concatenate the rotation: R_old = R_new * R_old
  m_rotation = glm::angleAxis(angle, m_axis) * m_rotation;

  m_lastPosition = currentPosition;
}

/**
 * @brief Function to be called on a mouse press event.
 *
 * @param position Position of the mouse cursor in window coordinates.
 */
void abcg::TrackBall::mousePress(glm::ivec2 const &position) {
  m_rotation = getRotation();
  m_mouseTracking = true;

  m_lastTime.restart();

  m_lastPosition = project(position);

  m_velocity = 0.0f;
}

/**
 * @brief Function to be called on a mouse release event.
 *
 * @param position Position of the mouse cursor in window coordinates.
 */
void abcg::TrackBall::mouseRelease(glm::ivec2 const &position) {
  mouseMove(position);

  // The longer the duration since last event, the slower the velocity
  m_velocity /= std::clamp(m_durationSinceLastEvent, 1.0f,
                           std::max(m_durationSinceLastEvent, 1.0f));
  m_mouseTracking = false;
}

/**
 * @brief Function to be called when the window is resized.
 *
 * @param size New size of the window, in pixels.
 */
void abcg::TrackBall::resizeViewport(glm::ivec2 const &size) noexcept {
  m_viewportSize = size;
}

/**
 * @brief Returns the current trackball rotation as a quaternion.
 *
 * @return Trackball rotation represented as a quaternion.
 */
glm::quat abcg::TrackBall::getRotation() const {
  if (m_mouseTracking)
    return m_rotation;

  auto const angle{m_velocity * gsl::narrow_cast<float>(m_lastTime.elapsed()) *
                   1000.0f};

  return glm::angleAxis(angle, m_axis) * m_rotation;
}

/**
 * @brief Sets the trackball's axis of rotation.
 *
 * @param axis Axis of rotation.
 *
 * The axis is expected to be a normalized vector.
 */
void abcg::TrackBall::setAxis(glm::vec3 axis) noexcept { m_axis = axis; }

/**
 * @brief Sets the trackball's rotation velocity.
 *
 * @param velocity Velocity in radians per second.
 *
 * The velocity is not bound checked.
 */
void abcg::TrackBall::setVelocity(float velocity) noexcept {
  m_velocity = velocity;
}

glm::vec3 abcg::TrackBall::project(const glm::vec2 &position) const {
  // Convert from window coordinates to NDC
  auto projected{glm::vec3(
      2.0f * position.x / gsl::narrow<float>(m_viewportSize.x) - 1.0f,
      1.0f - 2.0f * position.y / gsl::narrow<float>(m_viewportSize.y), 0.0f)};

  // Project to centered unit hemisphere
  if (auto const squaredLength{glm::length2(projected)};
      squaredLength >= 1.0f) {
    // Outside the sphere
    projected = glm::normalize(projected);
  } else {
    // Inside the sphere
    projected.z = std::sqrt(1.0f - squaredLength);
  }

  return projected;
}
