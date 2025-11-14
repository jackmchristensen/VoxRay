#include <cuda_runtime.h>
#include <cmath>

#include "shapes.hpp"

// Note: most of these comments are just notes for me as I'm learning CUDA so it's easy for me
// to remember what's happeneing where.
namespace preprocessing {

__global__ void generateSphereKernel(float* voxels, uint32_t width, uint32_t height, uint32_t depth,
                                    float center_x, float center_y, float center_z, float radius) {
  // --- Kernel setup ---
  uint32_t x = blockIdx.x * blockDim.x + threadIdx.x;
  uint32_t y = blockIdx.y * blockDim.y + threadIdx.y;
  uint32_t z = blockIdx.z * blockDim.y + threadIdx.z;

  // This makes sure that the kernel isn't trying to access data outside the grid
  // This shouldn't be an issue in how I calcuated the grid size, but it's good to be safe
  if (x >= width || y >= height || z >= depth) return;

  // --- Sphere generation ---
  float dist_x = (float)x - center_x;
  float dist_y = (float)y - center_y;
  float dist_z = (float)z - center_z;
  float dist   = sqrtf(dist_x*dist_x + dist_y*dist_y + dist_z*dist_z);

  float density = 0.f;
  if (dist < radius) {
    float falloff = (radius - dist) / 2.f;
    density = fminf(1.f, falloff);
  }

  // --- Write output ---
  uint32_t index = z * width * height + y * width + x;
  voxels[index] = density;
}

void generateSphere(VoxelGrid& grid, float center_x, float center_y, float center_z, float radius) {
  // --- Allocate device memory ---
  float* d_voxels = nullptr;
  size_t size = grid.width * grid.height * grid.depth * sizeof(float);
  cudaMalloc(&d_voxels, size);

  // --- Launch kernel ---
  // Try to keep total block size to either 256 or 512 since those seem to be the most optimal
  // Using 512 in this case because it forms a nice even cube with dimensions 8x8x8
  dim3 blockSize(8, 8, 8);
  // Dynamically finding the grid size just in case I change the block size later, I won't
  // have to recalculate the grid size
  dim3 gridSize(
    (grid.width  + blockSize.x - 1) / blockSize.x,
    (grid.height + blockSize.y - 1) / blockSize.y,
    (grid.depth  + blockSize.z - 1) / blockSize.z
  );

  generateSphereKernel<<<gridSize, blockSize>>>(
    d_voxels, grid.width, grid.height, grid.depth,
    center_x, center_y, center_z, radius
  );

  // Using cudaMemcpy for now because it's easy to setup
  // Slow because it copies data from the GPU to the CPU and then needs to copy that data back
  // to the GPU for the compute shader.
  // Better to use OpenGL Interop for efficiency. This should be fine because it's only needed
  // for the setup of the voxel grid and not being called every frame.
  cudaMemcpy(grid.data.data(), d_voxels, size, cudaMemcpyDeviceToDevice);

  // --- Cleanup ---
  cudaFree(d_voxels);
  cudaDeviceSynchronize();
}

}; // namespace preprocessing
