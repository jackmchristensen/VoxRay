#include "itkImage.h"
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkImageSeriesReader.h"
#include "itkImageFileWriter.h"
#include "preprocessing/voxel_grid.hpp"
#include <iterator>
#include <itkMacro.h>

#include "preprocessing/dicom_utils.hpp"

namespace preprocessing {

// namespace {
//   using PixelType = signed short;
//   constexpr unsigned int Dimension = 3;
//   using ImageType = itk::Image<PixelType, Dimension>;
//   using ReaderType = itk::ImageSeriesReader<ImageType>;
//   using ImageIOType = itk::GDCMImageIO;
//   using NamesGeneratorType = itk:: GDCMSeriesFileNames;

//   bool readDicomSeries(const ImageIOType::Pointer& dicomIO, const ReaderType::Pointer& reader, const ImageType::Pointer* image) {
//     reader->SetImageIO(dicomIO);
//     reader->SetFileNames(fileNames);

//     try {
//       reader->Update();
//     } catch (const itk::ExceptionObject e) {
//       printf("Error reading DICOM: %s\n", e.what());
//       return false;
//     }

//     image = reader->GetOutput();
//   }
// } // namespace

namespace {
  void getSize(const itk::Image<signed short, 3>::Pointer& image, DicomMetadata& metadata) {
    itk::Image<signed short, 3>::RegionType region = image->GetLargestPossibleRegion();
    itk::Image<signed short, 3>::SizeType size = region.GetSize();
    metadata.width  = size[0];
    metadata.height = size[1];
    metadata.depth  = size[2];
  }

  void getSpacing(const itk::Image<signed short, 3>::Pointer& image, DicomMetadata& metadata) {
    itk::Image<signed short, 3>::SpacingType spacing = image->GetSpacing();
    metadata.spacing_x = spacing[0];
    metadata.spacing_y = spacing[1];
    metadata.spacing_z = spacing[2];
  }

  void getOrigin(const itk::Image<signed short, 3>::Pointer& image, DicomMetadata& metadata) {
    itk::Image<signed short, 3>::PointType origin = image->GetOrigin();
    metadata.origin_x = origin[0];
    metadata.origin_y = origin[1];
    metadata.origin_z = origin[2];
  }
}

// Mainly based on ITK example function from here: https://examples.itk.org/src/io/gdcm/readdicomseriesandwrite3dimage/documentation
bool importDicomSeries(const std::string& directory, VoxelGrid& grid, DicomMetadata& metadata) {
  // ITK type definitions
  using PixelType = signed short;
  constexpr unsigned int Dimension = 3;
  using ImageType = itk::Image<PixelType, Dimension>;
  using ReaderType = itk::ImageSeriesReader<ImageType>;
  using ImageIOType = itk::GDCMImageIO;
  using NamesGeneratorType = itk:: GDCMSeriesFileNames;

  NamesGeneratorType::Pointer nameGenerator = NamesGeneratorType::New();
  nameGenerator->SetDirectory(directory);

  const std::vector<std::string>& fileNames = nameGenerator->GetInputFileNames();
  if (fileNames.empty()) {
    printf("No DICOM files found in %s\n", directory.c_str());
    return false;
  }

  printf("Found %zu DICOM files\n", fileNames.size());

  // Read DICOM series
  // TODO: break this out into its own helper function for better readability
  ImageIOType::Pointer dicomIO = ImageIOType::New();
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetImageIO(dicomIO);
  reader->SetFileNames(fileNames);

  try {
    reader->Update();
  } catch (const itk::ExceptionObject e) {
    printf("Error reading DICOM: %s\n", e.what());
    return false;
  }

  ImageType::Pointer image = reader->GetOutput();
  getSize(image, metadata);
  getSpacing(image, metadata);
  getOrigin(image, metadata);

  // Extract and normalize pixel data
  size_t total_voxels = (size_t)metadata.width * metadata.height * metadata.depth;
  const PixelType* buffer = image->GetBufferPointer();

  // Find min/max Hounsfield values
  PixelType hu_min = buffer[0];
  PixelType hu_max = buffer[0];
  for (size_t i = 1; i < total_voxels; i++) {
    if (buffer[i] < hu_min) hu_min = buffer[i];
    if (buffer[i] > hu_max) hu_max = buffer[i];
  }
  metadata.min_value = static_cast<float>(hu_min);
  metadata.max_value = static_cast<float>(hu_max);

  printf ("HU range: [%d, %d]", hu_min, hu_max);

  // Normalize range into [0, 1] and write to grid
  grid = VoxelGrid(metadata.width, metadata.height, metadata.depth);
  float hu_range = static_cast<float>(hu_max - hu_min);
  for (size_t i = 0; i < total_voxels; i++) {
    grid.data[i] = static_cast<float>(buffer[i] - hu_min) / hu_range;
  }

  return true;
}

} // namespace preprocessing
