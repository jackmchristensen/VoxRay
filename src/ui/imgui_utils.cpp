#include <glm/glm.hpp>

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"

#include "app/update_flags.hpp"

#include "graphics/gl_utils.hpp"

#include "windows.hpp"
#include "imgui_utils.hpp"

namespace ui {

  void initUI(SDL_Window* window, SDL_GLContext context) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports;

    ImGuiStyle& style = ImGui::GetStyle();
    // style.ScaleAllSizes(1.875f);
    // style.FontScaleDpi = 1.875f;

    // io.ConfigDpiScaleFonts = true;
    // io.ConfigDpiScaleViewports = true;

    ImGui_ImplSDL3_InitForOpenGL(window, context);
    ImGui_ImplOpenGL3_Init();
  }

  void renderViewport(ViewportWindow& viewport, UpdateFlags& flags) {
    ImGui::Begin(viewport.name.c_str());

    ImVec2 size = ImGui::GetContentRegionAvail();
    if (viewport.width != size.x || viewport.height != size.y) {
      viewport.width = size.x;
      viewport.height = size.y;

      viewport.update_framebuffer = true;
      flags |= VIEWPORT_RESIZE | RESIZE;
    }

    if (viewport.update_framebuffer) {
      if (!viewport.fbo.id || !viewport.texture.id) {
        graphics::makeTexture2D(GL_TEXTURE_2D, GL_RGBA32F, viewport.width, viewport.height, viewport.texture);
        graphics::makeFramebuffer(viewport.texture, viewport.fbo);
      } else {
        graphics::destroy(viewport.fbo);
        graphics::destroy(viewport.texture);

        graphics::makeTexture2D(GL_TEXTURE_2D, GL_RGBA32F, viewport.width, viewport.height, viewport.texture);
        graphics::makeFramebuffer(viewport.texture, viewport.fbo);
      }

      viewport.update_framebuffer = false;
    }

    ImGui::Image((void*)(intptr_t)viewport.texture.id, size, ImVec2(0, 1), ImVec2(1,0));

    ImGui::End();
  }

  void renderUI(const frame::FrameData& frame_data) {
    renderDiagnostics(frame_data);
    renderControls();
  }

} // namespace ui
