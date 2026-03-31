#version 430 core

layout(std140, binding = 0) uniform camera_block {
  mat4 u_view;
  mat4 u_proj;
  vec4 u_camera_pos;
};

// Render passes
layout(binding = 0) uniform sampler2D u_albedo;
layout(binding = 1) uniform sampler2D u_depth;
layout(binding = 2) uniform sampler2D u_normal;

in vec2 v_uv;

out vec4 fragColor;

void main() {
  vec4 tex = texture(u_albedo, v_uv);
  float r = pow(tex.r, 1.0 / 2.2);
  float g = pow(tex.g, 1.0 / 2.2);
  float b = pow(tex.b, 1.0 / 2.2);
  vec3 gamma_corrected = vec3(r, g, b);
  vec3 background = vec3(0.1, 0.1, 0.15);

  fragColor = vec4(mix(background, gamma_corrected, tex.a), 1.0);
}
