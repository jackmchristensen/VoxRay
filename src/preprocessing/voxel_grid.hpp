// preprocessing/voxel_grid.hpp
#pragma once
#include <vector>
#include <vector_types.h>
#include <cstdint>

namespace preprocessing {

  // Voxel grid data structure
  // Just handles a single source of data like density
  struct VoxelGrid {
    std::vector<float> data;
    std::vector<float4> normals;
    uint32_t width;
    uint32_t height;
    uint32_t depth;

    VoxelGrid() : width(0), height(0), depth(0) {}

    VoxelGrid(uint32_t w, uint32_t h, uint32_t d) : width(w), height(h), depth(d) {
      data.resize(w * h * d, 0.f);
      normals.resize(w * h * d);
    }

    // Helper to get data at a given position
    float& at(uint32_t x, uint32_t y, uint32_t z) {
      return data[z * width * height + y * width + x];
    }

    const float& at(uint32_t x, uint32_t y, uint32_t z) const {
      return data[z * width * height + y * width + x];
    }
  };

} // namespace preprocessing
