#include "trackball.hpp"

#include <algorithm>
#include <limits>

void TrackBall::mouseMove(glm::ivec2 const &position) {
  if (!m_mouseTracking)
    return;

  auto const currentPosition{project(position)};

  if (m_lastPosition == currentPosition) {
    // Scale velocity to zero if time since last event > 10ms
    m_velocity *= m_lastTime.elapsed() > 0.01 ? 0.0 : 1.0;
    return;
  }

  // Rotation axis
  m_axis = glm::cross(m_lastPosition, currentPosition);

  // Rotation angle
  auto const angle{glm::length(m_axis)};

  m_axis = glm::normalize(m_axis);

  // Compute an angle velocity that will be used as a constant rotation angle
  // when the mouse is not being tracked.
  m_velocity = angle / (gsl::narrow_cast<float>(m_lastTime.restart()) +
                        std::numeric_limits<float>::epsilon());
  m_velocity = glm::clamp(m_velocity, 0.0f, m_maxVelocity);

  // Concatenate rotation: R_old = R_new * R_old
  m_rotation = glm::rotate(glm::mat4(1.0f), angle, m_axis) * m_rotation;

  m_lastPosition = currentPosition;
}

void TrackBall::mousePress(glm::ivec2 const &position) {
  m_rotation = getRotation();
  m_mouseTracking = true;
  m_lastTime.restart();
  m_lastPosition = project(position);
  m_velocity = 0.0f;
}

void TrackBall::mouseRelease(glm::ivec2 const &position) {
  mouseMove(position);
  m_mouseTracking = false;
}

void TrackBall::resizeViewport(glm::ivec2 const &size) {
  m_viewportSize = size;
}

glm::mat4 TrackBall::getRotation() const {
  if (m_mouseTracking)
    return m_rotation;

  // Rotate by velocity when not tracking to simulate an inertia-free rotation
  auto const angle{m_velocity * gsl::narrow_cast<float>(m_lastTime.elapsed())};

  return glm::rotate(glm::mat4(1.0f), angle, m_axis) * m_rotation;
}

glm::vec3 TrackBall::project(glm::vec2 const &position) const {
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
