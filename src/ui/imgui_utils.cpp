#include <glm/glm.hpp>

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"

#include "windows.hpp"
#include "imgui_utils.hpp"

namespace ui {

  void initUI(SDL_Window* window, SDL_GLContext context) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports;

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForOpenGL(window, context);
    ImGui_ImplOpenGL3_Init();
  }

  void renderViewport(ViewportWindow& viewport) {
    ImGui::Begin(viewport.name.c_str());

    ImVec2 size = ImGui::GetContentRegionAvail();
    ImGui::Image((void*)(intptr_t)viewport.texture.id, size, ImVec2(0, 1), ImVec2(1,0));

    ImGui::End();
  }

  void renderUI(const frame::FrameData& frame_data) {
    renderDiagnostics(frame_data);
    renderControls();
  }

} // namespace ui
