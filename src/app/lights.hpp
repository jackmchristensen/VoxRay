// /app/lights.hpp

#include <glm/glm.hpp>

namespace lights {

struct directional {
  glm::vec3 direction = { 0.f, 1.f, 0.f };
  glm::vec3 color     = { 0.88f, 0.88f, 0.88f };
  float power         = 1.f;
  float size          = 1.f;
};

struct point {
  glm::vec3 position  = { 0.f, 0.f, 0.f };
  glm::vec3 color     = { 0.88f, 0.88f, 0.88f };
  float power         = 1.f;
  float size          = 1.f;
};

struct spot {
  glm::vec3 position  = { 0.f, 0.f, 0.f };
  glm::vec3 direction = { 0.f, 1.f, 0.f };
  float power         = 1.f;
  float radius        = 0.5f;
  float size          = 1.f;
};

} // namespace lights
