// camera.hpp
#pragma once
#include <glm/common.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace cam {

struct Camera{
  // Variables for transformations
  glm::vec3 position  { 0.f, 0.f,  3.f };
  glm::vec3 forward   { 0.f, 0.f, -1.f };
  glm::vec3 up        { 0.f, 1.f,  0.f };

  // Orbit specifics
  glm::vec3 target { 0.f, 0.f, 0.f };

  // Projection variables
  float fov_deg   = 60.f;
  float aspect    = 16.f/9.f;
  float near_clip = 0.1f;
  float far_clip  = 1000.f;
};


// --- Simple helper functions ---

inline glm::vec3 Right(const glm::vec3& forward, const glm::vec3& up) {
  return glm::normalize(glm::cross(forward, up));
}

inline void Orthonormalize(Camera& c) {
  c.forward  = glm::normalize(c.forward);
  glm::vec3 right = glm::normalize(glm::cross(c.forward, c.up));
  c.up       = glm::normalize(glm::cross(c.forward, right));
}


// --- Mutators ---

inline void TranslateWorld(Camera& c, const glm::vec3& delta) {
  c.position += delta;
}

inline void TranslateLocal(Camera& c, const glm::vec3& delta) {
  glm::vec3 right = Right(c.forward, c.up);
  c.position += right * delta.x + c.up * delta.y + c.forward * delta.z;
}

// Rotates along pitch and yaw
// Ignores roll
inline void Rotate(Camera& c, float yaw, float pitch) {
  glm::vec3 right = Right(c.forward, c.up);

  glm::quat quat_yaw    = glm::angleAxis(yaw, c.up);
  glm::quat quat_pitch  = glm::angleAxis(pitch, right);
  glm::quat quat_rotate = glm::normalize(quat_yaw * quat_pitch);

  c.forward  = glm::normalize(quat_rotate * c.forward);
  c.up       = glm::normalize(quat_rotate * c.up);

  Orthonormalize(c);
}

// Rotates camera around a target position
// Updates both position and rotation of the camera
inline void Orbit(Camera& c, float delta_yaw, float delta_pitch) {
  glm::vec3 to_cam = c.position - c.target;
  glm::vec3 right = Right(c.forward, c.up);

  glm::quat quat_yaw    = glm::angleAxis(delta_yaw, c.up);
  glm::quat quat_pitch  = glm::angleAxis(delta_pitch, right);
  glm::quat quat_rotate = glm::normalize(quat_yaw * quat_pitch);

  to_cam = quat_rotate * to_cam;
  c.position = c.target + to_cam;
  c.forward = glm::normalize(c.target - c.position);
  
  Orthonormalize(c);
}

inline void ZoomFOV(Camera& c, float delta_degrees) {
  c.fov_deg = glm::clamp(c.fov_deg + delta_degrees, 5.f, 180.f);
}

// Positive = dolly forward; Negative = dolly backwards
inline void Dolly(Camera& c, float amount) {
  c.position += c.forward * amount;
  c.target += c.forward * amount;
}


// --- Setters ---

inline void SetAspectRatio(Camera& c, float aspect) { c.aspect = aspect; }
inline void SetClip(Camera& c, float near_clip, float far_clip) { c.near_clip = near_clip; c.far_clip = far_clip; }


// --- Matrix builders ---

inline glm::mat4 View(const Camera& c) {
  return glm::lookAt(c.position, c.position + c.forward, c.up);
}

inline glm::mat4 Project(const Camera& c) {
  return glm::perspective(c.fov_deg, c.aspect, c.near_clip, c.far_clip);
}

inline glm::mat4 ViewProject(const Camera& c) { 
  return Project(c) * View(c);
}

}; // namespace cam
