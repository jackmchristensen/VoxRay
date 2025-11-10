#version 430 core
layout(local_size_x = 16, local_size_y = 16) in;

layout(std140, binding = 0) uniform camera_block {
    mat4 u_view;
    mat4 u_proj;
    vec4 u_cam;
    int u_width;
    int u_height;
};

layout(rgba32f, binding = 1) uniform image2D u_out;

void main() {
    ivec2 p = ivec2(gl_GlobalInvocationID.xy);
    imageStore(u_out, p, vec4(p.x / float(u_width), p.y / float(u_height), 0.0, 1.0));
}
