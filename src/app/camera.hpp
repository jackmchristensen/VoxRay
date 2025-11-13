// camera.hpp
#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/common.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>

namespace cam {

struct Camera{
  // Variables for transformations
  glm::vec3 position  { 0.f, 0.f,  3.f };
  glm::vec3 forward   { 0.f, 0.f, -1.f };
  glm::vec3 up        { 0.f, 1.f,  0.f };

  // Orbit specifics
  glm::vec3 target { 0.f, 0.f, 0.f };

  // Projection variables
  float fov_deg   = 30.f;
  float aspect    = 16.f/9.f;
  float near_clip = 0.1f;
  float far_clip  = 1000.f;

  mutable glm::mat4 view{1.f}, proj{1.f};
};


// --- Simple helper functions ---

inline glm::vec3 getRight(const glm::vec3& forward, const glm::vec3& up) {
  return glm::normalize(glm::cross(forward, up));
}

inline void orthonormalize(Camera& c) {
  c.forward  = glm::normalize(c.forward);
  glm::vec3 right = glm::normalize(glm::cross(c.forward, c.up));
  c.up       = glm::normalize(glm::cross(right, c.forward));
}


// --- Mutators ---

inline void translateWorld(Camera& c, const glm::vec3& delta) {
  c.position += delta;
}

inline void translateLocal(Camera& c, const glm::vec3& delta) {
  glm::vec3 right = getRight(c.forward, c.up);
  c.position += right * delta.x + c.up * delta.y + c.forward * delta.z;
}

// Rotates along pitch and yaw
// Ignores roll
inline void rotate(Camera& c, float yaw, float pitch) {
  glm::mat3 rotate_y = glm::mat3(glm::rotate(glm::mat4(1.f), yaw, c.up));
  c.forward = glm::normalize(rotate_y * c.forward);
  c.up = glm::normalize(rotate_y * c.up);

  glm::vec3 right = getRight(c.forward, c.up);

  glm::mat3 rotate_x = glm::mat3(glm::rotate(glm::mat4(1.f), pitch, right));
  c.forward = glm::normalize(rotate_x * c.forward);
  c.up = glm::normalize(rotate_x * c.up);

  orthonormalize(c);
}

// Rotates camera around a target position
// Updates both position and rotation of the camera
// Note: up vector may be inverted internally, but functionality should not be affected
inline void orbit(Camera& c, float delta_yaw, float delta_pitch) {
  glm::vec3 offset  = c.position - c.target;
  float radius      = glm::length(offset);

  glm::vec3 world_up  = glm::vec3(0.f, 1.f, 0.f);
  glm::vec3 right     = glm::normalize(glm::cross(world_up, offset));

  // Apply rotations
  offset = glm::rotate(offset, -delta_yaw, world_up);
  right  = glm::normalize(glm::cross(world_up, offset));
  offset = glm::rotate(offset, -delta_pitch, right);

  // Update camera
  c.position  = c.target + offset;
  c.forward   = -glm::normalize(offset);
  c.up        = glm::normalize(glm::cross(c.forward, right));
}

inline void zoomFOV(Camera& c, float delta_degrees) {
  c.fov_deg = glm::clamp(c.fov_deg + delta_degrees, 5.f, 180.f);
}

// Positive = dolly forward; Negative = dolly backwards
inline void dolly(Camera& c, float amount) {
  c.position += c.forward * amount;
  c.target += c.forward * amount;
}


// --- Setters ---

inline void setAspectRatio(Camera& c, float aspect) { c.aspect = aspect; }
inline void setClip(Camera& c, float near_clip, float far_clip) { c.near_clip = near_clip; c.far_clip = far_clip; }


// --- Matrix builders ---

inline void updateView(Camera& c) {
  c.view = glm::lookAt(c.position, c.position+c.forward, c.up);
}

inline void updateProject(Camera& c) {
  c.proj = glm::perspective(c.fov_deg, c.aspect, c.near_clip, c.far_clip);
}

inline glm::mat4 viewProject(const Camera& c) {
  return c.proj * c.view;
}

inline Camera makeCamera(
  const glm::vec3& position,
  const glm::vec3& target,
  float fov_deg   = 30.f,
  float aspect    = 16.f/9.f,
  float near_clip = 0.1f,
  float far_clip  = 1000.f
) {
  Camera c{
    .position   = position,
    .target     = target,
    .fov_deg    = fov_deg,
    .aspect     = aspect,
    .near_clip  = near_clip,
    .far_clip   = far_clip
  };

  c.forward = glm::normalize(target - position);
  // Note: at some point, the world got flipped turned upside down. Not sure where it happened
  // but everything seems to work fine, just inverted.
  c.up = glm::vec3(0.f, -1.f, 0.f);
  orthonormalize(c);

  updateView(c);
  updateProject(c);

  return c;
}

inline Camera makeDefaultCamera() {
  return makeCamera(glm::vec3(2.121f, 2.121f, 2.121f), glm::vec3(0.f, 0.f, 0.f));
}

}; // namespace cam
