#version 330 core

in vec2 v_uv;

out vec4 fragColor;

void main() {
  fragColor = vec4(v_uv.x, v_uv.y, 0.0f, 1.0f);
  // fragColor = vec4(0.243f, 0.353f, 0.541f, 1.0f);
}
