cmake_minimum_required(VERSION 3.24)
project(VoxRay LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CUDA_STANDARD 20)

set(OUT_DIR ${CMAKE_SOURCE_DIR}/bin)
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(SHADER_DIR ${CMAKE_SOURCE_DIR}/shaders)

add_executable(VoxRay
  ${SRC_DIR}/main.cpp
  ${SRC_DIR}/app/app_context.cpp
  ${SRC_DIR}/gl/gl_utils.cpp
)

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

set_target_properties(VoxRay PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${OUT_DIR}
  CUDA_SEPARABLE_COMPILATION ON
)

# set_source_files_properties(${SRC_DIR}/preprocessing.cu PROPERTIES LANGUAGE CUDA)
