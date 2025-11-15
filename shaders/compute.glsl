#version 430 core
layout(local_size_x = 16, local_size_y = 16) in;

// Currently a catchall block for camera, window, and app data
// TODO separate data into separate blocks for organization
layout(std140, binding = 0) uniform camera_block {
  mat4 u_view;
  mat4 u_proj;
  vec4 u_cam;
  int u_width;
  int u_height;
};

// Render passes
layout(rgba32f, binding = 0) uniform image2D u_albedo;
layout(rgba32f, binding = 1) uniform image2D u_depth;
layout(rgba32f, binding = 2) uniform image2D u_normal;

// Voxel texture
uniform sampler3D u_voxel_data;

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

void rayMarch(vec3 ray_origin, vec3 ray_dir, out vec4 albedo, out vec4 depth, out vec4 normal) {
  // Test cube of size 2.0 in center of world (from -1 to 1)
  vec3 box_min = vec3(-1.0);
  vec3 box_max = vec3(1.0);

  vec2 intersection = intersectAABB(ray_origin, ray_dir, box_min, box_max);
  // Check for miss
  // Color miss black
  if (intersection.x > intersection.y || intersection.y < 0.0) {
    albedo = vec4(0.0);
    depth = vec4(0.0);
    normal = vec4(0.0);
    return;
  }

  // Color position
  float t = intersection.x > 0.0 ? intersection.x : intersection.y;
  vec3 hit_point = ray_origin + ray_dir * t;

  vec4 wireframe = drawBounds(0.01, hit_point, box_min, box_max);
  albedo = mix(vec4(1.0), wireframe.rgba, wireframe.a);
  depth = vec4(vec3(length(hit_point - ray_origin) / 5.0), 1.0);

  float eps = 0.001;
  vec3 world_normal = vec3(0.0);
  if (abs(hit_point.x - box_min.x) < eps) world_normal.r = -1.0;
  else if (abs(hit_point.x - box_max.x) < eps) world_normal.r = 1.0;
  else if (abs(hit_point.y - box_min.y) < eps) world_normal.g = -1.0;
  else if (abs(hit_point.y - box_max.y) < eps) world_normal.g = 1.0;
  else if (abs(hit_point.z - box_min.z) < eps) world_normal.b = -1.0;
  else if (abs(hit_point.z - box_max.z) < eps) world_normal.b = 1.0;

  vec3 encoded_normal = max(vec3(0.0), world_normal);
  normal = vec4(encoded_normal, 1.0);
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

  vec4 albedo, depth, normal = vec4(0.0);
  rayMarch(ray_origin, ray_dir, albedo, depth, normal);

  imageStore(u_albedo, pixel, albedo);
  imageStore(u_depth, pixel, depth);
  imageStore(u_normal, pixel, normal);
}
