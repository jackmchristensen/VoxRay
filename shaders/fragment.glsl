#version 430 core

layout(std140, binding = 0) uniform camera_block {
  mat4 u_view;
  mat4 u_proj;
  vec4 u_camera_pos;
};

in vec2 v_uv;

out vec4 fragColor;

void main() {
  fragColor = normalize(inverse(u_view) * u_camera_pos);
}
