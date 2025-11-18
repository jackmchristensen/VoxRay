#include "app/update_flags.hpp"
#include "app/app_context.hpp"
#include "app/camera.hpp"
#include "app/frame_data.hpp"

#include "ui/imgui_utils.hpp"
#include "ui/viewport_window.hpp"

#include "graphics/gl_utils.hpp"
#include "graphics/render_targets.hpp"

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl3.h"

#include "preprocessing/shapes.hpp"
#include "preprocessing/voxel_grid.hpp"

int main() {
  using namespace graphics;
  using namespace cam;

  AppConfig config{
    .title      = "VoxRay",
    .width      = 1280,
    .height     = 720,
    .gl_major   = 4,
    .gl_minor   = 3
  };

  AppContext app = makeApp(config);
  InputState input;

  ui::initUI(app.window.get(), app.gl_context.get());

  if (glewInit() != GLEW_OK) {
    SDL_Log("Failed to initialize GLEW");
    return 1;
  }

  std::string error;
  const char* compute_path = "shaders/compute.glsl";
  Shader compute{}; Program compute_prog;
  if (!compileShader(GL_COMPUTE_SHADER, compute_path, compute, &error))     { SDL_Log("%s", error.c_str()); return 1; }
  if (!linkProgram(compute, compute_prog, &error))                          { SDL_Log("%s", error.c_str()); return 1; }
  destroy(compute);

  const char* vertex_path = "shaders/vertex.glsl";
  const char* fragment_path = "shaders/fragment.glsl";
  Shader vertex{}; Shader fragment{}; Program display_prog{};
  if (!compileShader(GL_VERTEX_SHADER, vertex_path, vertex, &error))        { SDL_Log("%s", error.c_str()); return 1; }
  if (!compileShader(GL_FRAGMENT_SHADER, fragment_path, fragment, &error))  { SDL_Log("%s", error.c_str()); return 1; }
  if (!linkProgram(vertex, fragment, display_prog, &error))                 { SDL_Log("%s", error.c_str()); return 1; }
  destroy(vertex); destroy(fragment);

  VertexArray vao{};
  if (!makeVao(vao)) return 1;

  Buffer cam_ubo{};
  if (!makeBuffer(GL_UNIFORM_BUFFER, sizeof(glm::mat4)*2 + sizeof(glm::vec4) + sizeof(GLuint)*2, nullptr, GL_DYNAMIC_DRAW, cam_ubo)) return 1;
  glBindBufferBase(GL_UNIFORM_BUFFER, 0, cam_ubo.id);

  frame::FrameTimer timer = frame::makeFrameTimer();
  frame::FrameData frame_data{};
  frame::beginFrame(timer);

  // --- Create voxel sphere ---
  printf("Generating voxel sphere...\n");
  preprocessing::VoxelGrid voxels(64, 64, 64);
  preprocessing::generateSphere(voxels, 0.f, 0.f, 0.f, 20.f);
  printf("Sphere generated!\n");

  Texture3D voxel_texture;
  makeTexture3D(GL_R32F, 64, 64, 64, voxel_texture);
  uploadTexture3D(voxel_texture, voxels.data.data());


  // --- Viewport subwindow ---
  Framebuffer framebuffer{}; Texture color_attach{};
  if (!makeTexture2D(GL_TEXTURE_2D, GL_RGBA32F, app.width, app.height, color_attach)) return 1;
  if (!makeFramebuffer(color_attach, framebuffer)) return 1;
  ui::ViewportWindow viewport {
    .name    = "Viewport",
    .fbo     = framebuffer,
    .texture = color_attach
  };

  RenderTargets targets{};
  if (!makeRenderTargets(app.width, app.height, targets)) return 1;

  // UseProgram(display_prog);
  bindVao(vao);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  useProgram(compute_prog);
  bindTexture3D(voxel_texture, 0);
  bindForCompute(targets);

  UpdateFlags flags = NONE;
  while ((flags & STOP) != STOP) {
    // Gather frame rate data
    frame::endFrame(timer, frame_data);

    pollInput(flags, input);

    // --- DearImGui stuff ---
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    ImGui::DockSpaceOverViewport();
    ui::renderUI(frame_data);
    ui::renderViewport(viewport);

    // printf("App dim: (%d, %d) | Current dim: (%d, %d)\n", app.width, app.height, current_w, current_h);
    // TODO move resizing textures to function in the graphics directory
    // The main function should be used to bridge the gap between the different directorys
    // and shouldn't do any updating itself.
    // Note: This should be called after updateState() but because updateState() resets the flags I have to
    // put it before and use a workaround for the resizing.
    // This is only an issue for full screening and docking/free floating the window for some reason, it doesn't affect
    // manually resizing the window.
    ImVec2 viewport_size = ImGui::GetContentRegionAvail();
    if (viewport_size.x != viewport.width || viewport_size.y != viewport.height) {
      viewport.width = viewport_size.x;
      viewport.height = viewport_size.y;
      int new_w, new_h;
      SDL_GetWindowSizeInPixels(app.window.get(), &new_w, &new_h);
      resizeRenderTargets(new_w, new_h, targets);
      useProgram(compute_prog);
      bindTexture3D(voxel_texture, 0);
      bindForCompute(targets);
    }
    if (flags) {
      updateState(flags, app, input);
      useProgram(compute_prog);
      bindTexture3D(voxel_texture, 0);
      bindForCompute(targets);
    }

    auto& c = activeCamera(app);
    glm::vec4 pos = glm::vec4(glm::vec3(c.position), 1.f);
    glBindBuffer(cam_ubo.target, cam_ubo.id);
    glBufferSubData(cam_ubo.target, 0,                   sizeof(glm::mat4), &c.view[0][0]);
    glBufferSubData(cam_ubo.target, sizeof(glm::mat4),   sizeof(glm::mat4), &c.proj[0][0]);
    glBufferSubData(cam_ubo.target, sizeof(glm::mat4)*2, sizeof(glm::vec4), &pos.x);
    glBufferSubData(cam_ubo.target, sizeof(glm::mat4)*2 + sizeof(glm::vec4), sizeof(GLuint), &app.width);
    glBufferSubData(cam_ubo.target, sizeof(glm::mat4)*2 + sizeof(glm::vec4) + sizeof(GLuint), sizeof(GLuint), &app.height);

    bindFramebuffer(viewport.fbo);
    glViewport(0, 0, app.width, app.height);
    glClear(GL_COLOR_BUFFER_BIT);

    GLuint gx = (app.width + 16 - 1) / 16;
    GLuint gy = (app.height + 16 - 1) / 16;
    glDispatchCompute(gx, gy, 1);

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

    // Display pass
    useProgram(display_prog);
    bindForDisplay(targets);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    unbindFramebuffer();

    glViewport(0, 0, app.width, app.height);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Currently just swaps window
    // Keeping this because I might add more functionality in the future
    draw(app);
  }

  destroy(vao);
  destroy(compute_prog);
  destroy(display_prog);
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL3_Shutdown();
  ImGui::DestroyContext();
  SDL_GL_DestroyContext(app.gl_context.get());
  SDL_DestroyWindow(app.window.get());
  SDL_Quit();
  return 0;
}
