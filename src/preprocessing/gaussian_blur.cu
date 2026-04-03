#include <cuda_runtime.h>
#include "preprocessing/gaussian_blur.hpp"

__constant__ float c_kernel[5] = { 0.25f, 0.5f, 0.25f };
static const int KERNEL_RADIUS = 1;

// ———— X Pass ————
__global__ void gaussianBlurX(const float* input, float* output, uint32_t width, uint32_t height, uint32_t depth) {
  uint32_t x = blockIdx.x * blockDim.x + threadIdx.x;
  uint32_t y = blockIdx.y * blockDim.y + threadIdx.y;
  uint32_t z = blockIdx.z * blockDim.z + threadIdx.z;
  if (x >= width || y >= height || z >= depth) return;

  float sum = 0.f;
  for (int k = -KERNEL_RADIUS; k <= KERNEL_RADIUS; k++) {
    int xi = min(max(0, (int)x + k), (int)width - 1);
    sum += input[z * width * height + y * width + xi] * c_kernel[k + KERNEL_RADIUS];
  }
  output[z * width * height + y * width + x] = sum;
}

// ———— Y Pass ————
__global__ void gaussianBlurY(const float* input, float* output, uint32_t width, uint32_t height, uint32_t depth) {
  uint32_t x = blockIdx.x * blockDim.x + threadIdx.x;
  uint32_t y = blockIdx.y * blockDim.y + threadIdx.y;
  uint32_t z = blockIdx.z * blockDim.z + threadIdx.z;
  if (x >= width || y >= height || z >= depth) return;

  float sum = 0.f;
  for (int k = -KERNEL_RADIUS; k <= KERNEL_RADIUS; k++) {
    int yi = min(max(0, (int)y + k), (int)height - 1);
    sum += input[z * width * height + yi * width + x] * c_kernel[k + KERNEL_RADIUS];
  }
  output[z * width * height + y * width + x] = sum;
}

// ———— Z Pass ————
__global__ void gaussianBlurZ(const float* input, float* output, uint32_t width, uint32_t height, uint32_t depth) {
  uint32_t x = blockIdx.x * blockDim.x + threadIdx.x;
  uint32_t y = blockIdx.y * blockDim.y + threadIdx.y;
  uint32_t z = blockIdx.z * blockDim.z + threadIdx.z;
  if (x >= width || y >= height || z >= depth) return;

  float sum = 0.f;
  for (int k = -KERNEL_RADIUS; k <= KERNEL_RADIUS; k++) {
    int zi = min(max(0, (int)z + k), (int)depth - 1);
    sum += input[zi * width * height + y * width + x] * c_kernel[k + KERNEL_RADIUS];
  }
  output[z * width * height + y * width + x] = sum;
}

namespace preprocessing {

void gaussianBlur(VoxelGrid& grid) {
  size_t size = grid.width * grid.height * grid.depth * sizeof(float);

  float* d_input  = nullptr;
  float* d_tmp    = nullptr;
  float* d_output = nullptr;
  cudaMalloc(&d_input,  size);
  cudaMalloc(&d_tmp,    size);
  cudaMalloc(&d_output, size);

  cudaMemcpy(d_input, grid.data.data(), size, cudaMemcpyHostToDevice);

  dim3 blockSize(8, 8, 8);
  dim3 gridSize(
      (grid.width   + blockSize.x - 1) / blockSize.x,
      (grid.height  + blockSize.y - 1) / blockSize.y,
      (grid.depth   + blockSize.y - 1) / blockSize.z
      );

  gaussianBlurX<<<gridSize, blockSize>>>(d_input,   d_tmp,    grid.width, grid.height, grid.depth);
  gaussianBlurY<<<gridSize, blockSize>>>(d_tmp,     d_output, grid.width, grid.height, grid.depth);
  gaussianBlurZ<<<gridSize, blockSize>>>(d_output,  d_tmp,    grid.width, grid.height, grid.depth);

  cudaMemcpy(grid.data.data(), d_tmp, size, cudaMemcpyDeviceToHost);

  cudaFree(d_input);
  cudaFree(d_tmp);
  cudaFree(d_output);
  cudaDeviceSynchronize();
}


} // namespace preprocessing
