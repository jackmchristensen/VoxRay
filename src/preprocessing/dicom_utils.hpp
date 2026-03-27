// preprocessing/dicom_utils.hpp
#pragma once

#include <string>

#include "preprocessing/voxel_grid.hpp"

namespace preprocessing {

  struct DicomMetadata {
    float spacing_x, spacing_y, spacing_z;
    float origin_x, origin_y, origin_z;
    int width, height, depth;
    float min_value, max_value;
  };

  bool importDicomSeries(const std::string& directory, VoxelGrid& grid, DicomMetadata& metadata);

} // namespace preprocessing
