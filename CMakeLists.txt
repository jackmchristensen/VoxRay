cmake_minimum_required(VERSION 3.24)
project(VoxRay LANGUAGES CXX CUDA)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CUDA_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG origin/docking
)
FetchContent_MakeAvailable(imgui)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  message(STATUS "Debug build")
  set(OUT_DIR ${CMAKE_SOURCE_DIR}/bin/debug)
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
  message(STATUS "Release build")
  set(OUT_DIR ${CMAKE_SOURCE_DIR}/bin/release)
else ()
  message(STATUS "Default build")
  set(OUT_DIR ${CMAKE_SOURCE_DIR}/bin)
endif ()

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(SHADER_DIR ${CMAKE_SOURCE_DIR}/shaders)

set(IMGUI_DIR ${imgui_SOURCE_DIR})
add_library(imgui STATIC
  ${IMGUI_DIR}/imgui.cpp
  ${IMGUI_DIR}/imgui_draw.cpp
  ${IMGUI_DIR}/imgui_tables.cpp
  ${IMGUI_DIR}/imgui_widgets.cpp
  ${IMGUI_DIR}/imgui_demo.cpp
  ${IMGUI_DIR}/backends/imgui_impl_sdl3.cpp
  ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC
  ${IMGUI_DIR}
  ${IMGUI_DIR}/backends
)

add_executable(VoxRay
  ${SRC_DIR}/main.cpp
  ${SRC_DIR}/graphics/gl_utils.cpp
  ${SRC_DIR}/app/app_context.cpp
  ${SRC_DIR}/app/frame_data.cpp
  ${SRC_DIR}/ui/imgui_utils.cpp
  ${SRC_DIR}/ui/windows.cpp
  ${SRC_DIR}/preprocessing/shapes.cu
)
target_include_directories(VoxRay PRIVATE ${SRC_DIR})

add_custom_command(
  TARGET VoxRay POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${SHADER_DIR}
          $<TARGET_FILE_DIR:VoxRay>/shaders
  COMMENT "Copying shaders to output directory..."
)

find_package(OpenGL REQUIRED)
target_link_libraries(VoxRay PRIVATE OpenGL::GL)

find_package(GLEW REQUIRED)
target_link_libraries(VoxRay PRIVATE GLEW::GLEW)

find_package(SDL3 REQUIRED)
target_link_libraries(VoxRay PRIVATE SDL3::SDL3)

find_package(CUDAToolkit REQUIRED)
target_link_libraries(VoxRay PRIVATE CUDA::cudart)

target_link_libraries(VoxRay PRIVATE imgui)
target_link_libraries(imgui PUBLIC SDL3::SDL3 OpenGL::GL)

set_target_properties(VoxRay PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${OUT_DIR}
  CUDA_SEPARABLE_COMPILATION ON
)

set_source_files_properties(${SRC_DIR}/preprocessing/shapes.cu PROPERTIES LANGUAGE CUDA)
