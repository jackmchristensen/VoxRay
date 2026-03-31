#include <cuda_runtime.h>
#include <vector_types.h>

#include "preprocessing/voxel_grid.hpp"

__global__ void computeGradient(const float* density, float4* normals, uint32_t width, uint32_t height, uint32_t depth) {
  uint32_t x = blockIdx.x * blockDim.x + threadIdx.x;
  uint32_t y = blockIdx.y * blockDim.y + threadIdx.y;
  uint32_t z = blockIdx.z * blockDim.z + threadIdx.z;

  if (x >= width || y >= height || z >= depth) return;

  // Sample density at given point (x, y, z)
  auto sample = [&](int xi, int yi, int zi) -> float {
    xi = max(0, min((int)width  - 1, xi));
    yi = max(0, min((int)height - 1, yi));
    zi = max(0, min((int)depth  - 1, zi));
    return density[zi * width * height + yi * width + xi];
  };

  // Calculate gradient
  float gx = sample(x+1, y, z) - sample(x-1, y, z);
  float gy = sample(x, y+1, z) - sample(x, y-1, z);
  float gz = sample(x, y, z+1) - sample(x, y, z-1);

  // Normalize gradient
  float len = sqrtf(gx*gx + gy*gy + gz*gz);
  float4 normal = (len > 1e-6f) ? make_float4(-gx/len, -gy/len, -gz/len, len) : make_float4(0.f, 0.f, 0.f, 0.f);

  normals[z * width * height + y * width + x] = normal;
}

namespace preprocessing {

void computeGradientKernel(VoxelGrid& grid) {
  float4* d_normals = nullptr;
  size_t size_normals = grid.width * grid.height * grid.depth * sizeof(float4);
  cudaMalloc(&d_normals, size_normals);

  float* d_density = nullptr;
  size_t size_density = grid.width * grid.height * grid.depth * sizeof(float);
  cudaMalloc(&d_density, size_density);
  cudaMemcpy(d_density, grid.data.data(), size_density, cudaMemcpyHostToDevice);

  dim3 blockSize(8, 8, 8);
  dim3 gridSize(
    (grid.width  + blockSize.x - 1) / blockSize.x,
    (grid.height + blockSize.y - 1) / blockSize.y,
    (grid.depth  + blockSize.z - 1) / blockSize.z
  );

  computeGradient<<<gridSize, blockSize>>>(d_density, d_normals, grid.width, grid.height, grid.depth);

  cudaMemcpy(grid.normals.data(), d_normals, size_normals, cudaMemcpyDeviceToHost);
 
  cudaFree(d_normals);
  cudaFree(d_density);
  cudaDeviceSynchronize();
}

}; // namespace preprocessing
