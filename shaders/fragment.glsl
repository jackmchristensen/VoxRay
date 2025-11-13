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
  vec4 tex = texture(u_normal, v_uv);
  vec3 background = vec3(0.1, 0.1, 0.15);

  fragColor = vec4(mix(background, tex.rgb, tex.a), 1.0);
}
