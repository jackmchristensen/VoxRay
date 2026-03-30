# VoxRay

A real-time DICOM volume renderer written in C++/CUDA. Built as a way to learn about GPU accelerated medical imaging, specifically voxel preprocessing with CUDA and DICOM I/O with ITK.

## What it does

VoxRay loads a DICOM image series (like a CT scan), voxelizes it into a 3D texture on the GPU, and renders it in real-time using a ray marching compute shader. The viewport is interactive, you can orbit, pan, and dolly the camera, and adjust the CT windowing to isolate different tissue densities.

The rendering pipeline is a deferred-style setup with separate albedo, depth, and normal passes.

## Features

- DICOM series import via ITK (tested with CT; signed short / Hounsfield unit data)
- CUDA voxel preprocessing, GPU side voxel grid generation, with CUDA kernels handling per voxel computation
- Ray marching compute shader in OpenGL with jittered sampling to reduce banding

## Building

Requires: CUDA toolkit, ITK, SDL3, GLEW, OpenGL 4.3+

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

## Dataset

Tested with the [Visible Human Project CT Datasets](https://mri.medicine.uiowa.edu/equipment-information/scanner-images/visible-human-project-ct-datasets) female head dataset (512x512x234, 1mm isotropic spacing) and male head dataset (512x512x245, 1mm isotropic spacing).
