#version 430 core
layout(local_size_x = 16, local_size_y = 16) in;

// Currently a catchall block for camera, window, and app data
// TODO: separate data into separate blocks for organization
layout(std140, binding = 0) uniform camera_block {
  mat4 u_view;
  mat4 u_proj;
  vec4 u_cam;
  vec4 u_volume_scale;
  int u_width;
  int u_height;
  float u_win_center;
  float u_win_width;
  float u_density_scale;
};

// Render passes
layout(rgba32f, binding = 0) uniform image2D u_albedo;
layout(rgba32f, binding = 1) uniform image2D u_depth;
layout(rgba32f, binding = 2) uniform image2D u_normal;

// Voxel texture
layout(binding = 0) uniform sampler3D u_voxel_data;
layout(binding = 1) uniform sampler3D u_voxel_normals;

// Rotation matrix for temp viewing purposes
mat4 u_volume_rotation = mat4(
  1.0,  0.0,  0.0,  0.0,
  0.0,  0.0,  1.0,  0.0,
  0.0, -1.0,  0.0,  0.0,
  0.0,  0.0,  0.0,  1.0
);

float hash(vec2 p) {
  return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

vec4 drawBounds(float thickness, vec3 hit_point, vec3 box_min, vec3 box_max) {
  // Normalize position to [0,1] range within the box
  vec3 p = (hit_point - box_min) / (box_max - box_min);

  // For each axis, check if we're near the min (0) or max (1) boundary
  vec3 distToEdge = min(p, 1.0 - p);

  // Count how many axes are near a boundary
  int nearEdgeCount = 0;
  if (distToEdge.x < thickness) nearEdgeCount++;
  if (distToEdge.y < thickness) nearEdgeCount++;
  if (distToEdge.z < thickness) nearEdgeCount++;

  // Wireframe: draw if we're on an edge (near boundary on exactly 2 axes)
  if (nearEdgeCount >= 2) {
    return vec4(0.0, 0.0, 0.0, 1.0);
  }

  return vec4(0.0, 0.0, 0.0, 0.0);
}

// Find near and far intersections of a given cube
vec2 intersectAABB(vec3 ray_origin, vec3 ray_dir, vec3 box_min, vec3 box_max) {
  vec3 t_min = (box_min - ray_origin) / ray_dir;
  vec3 t_max = (box_max - ray_origin) / ray_dir;

  vec3 t_0 = min(t_min, t_max);
  vec3 t_1 = max(t_min, t_max);

  float t_near = max(max(t_0.x, t_0.y), t_0.z);
  float t_far = min(min(t_1.x, t_1.y), t_1.z);

  return vec2(t_near, t_far);
}

void rayMarch(vec3 ray_origin, vec3 ray_dir, out vec4 albedo, out vec4 depth, out vec4 normal, ivec2 pixel) {
  vec3 box_max = u_volume_scale.xyz;
  vec3 box_min = -box_max;

  vec2 intersection = intersectAABB(ray_origin, ray_dir, box_min, box_max);
  // Check for miss
  // Color miss black
  if (intersection.x > intersection.y || intersection.y < 0.0) {
    albedo = vec4(0.0);
    depth = vec4(0.0);
    normal = vec4(0.0);
    return;
  }
 
  float t_end = intersection.y;
  float step_size = 0.005;
  int max_steps = 1000;
  float jitter = hash(vec2(pixel)) * step_size;
  float t = max(intersection.x, 0.0) + jitter;

  vec4 accumulated_color = vec4(0.0);
  vec3 first_hit_normal = vec3(0.0);
  float first_hit_depth = 0.0;
  bool hit = false;
  vec3 first_hit_tex_pos = vec3(0.0);

  for (int step = 0; step < max_steps && t < t_end; step++) {
    if (accumulated_color.a >= 0.95) break;

    vec3 world_pos = ray_origin + ray_dir * t;
    // vec3 rotated_pos = (u_volume_rotation * vec4(world_pos, 1.0)).xyz;
    vec3 tex_pos = (world_pos - box_min) / (box_max - box_min);

    float raw = texture(u_voxel_data, tex_pos).r;

    // Apply HU windowing: remap so that win_center is mid-gray
    // Clamp values so air is not shown
    float half_width = u_win_width * 0.5;
    float density = clamp((raw - (u_win_center - half_width)) / u_win_width * u_density_scale, 0.0, 1.0);

    if (density > 0.01) {
      if (!hit) {
        first_hit_normal = vec3(0.5, 0.5, 1.0); // Placeholder
        first_hit_depth = t;
        first_hit_tex_pos = tex_pos;
        hit = true;
      }

      vec3 sample_color = vec3(1.0, 0.8, 0.6) * density;
      float sample_alpha = clamp(density * step_size * 100.0, 0.0, 1.0);

      accumulated_color.rgb += sample_color * sample_alpha * (1.0 - accumulated_color.a);
      accumulated_color.a += sample_alpha * (1.0 - accumulated_color.a);
    }

    t += step_size;
  }

  albedo = accumulated_color;
  depth = hit ? vec4(vec3(first_hit_depth / 5.0), 1.0) : vec4(0.0);
  normal = hit ? texture(u_voxel_normals, first_hit_tex_pos) : vec4(0.0);
}

void main() {
  // Get pixel and convert to device coordinates (NDC)
  ivec2 pixel = ivec2(gl_GlobalInvocationID.xy);
  vec2 uv = vec2(pixel) / vec2(u_width, u_height);
  uv = uv * 2.0 - 1.0;

  vec3 ray_origin = u_cam.xyz;

  // Find ray direction
  mat4 inv_ViewProj = inverse(u_proj * u_view);
  vec4 target = inv_ViewProj * vec4(uv.x, uv.y, 1.0, 1.0);
  vec3 ray_dir = normalize(target.xyz / target.w - ray_origin);

  mat3 inv_rot = transpose(mat3(-u_volume_rotation));
  vec3 local_origin = inv_rot * ray_origin;
  vec3 local_dir    = inv_rot * ray_dir;

  vec4 albedo, depth, normal = vec4(0.0);
  rayMarch(local_origin, local_dir, albedo, depth, normal, pixel);

  imageStore(u_albedo, pixel, albedo);
  imageStore(u_depth, pixel, depth);
  imageStore(u_normal, pixel, normal);
}
