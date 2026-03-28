#include "imgui.h"

#include "windows.hpp"

namespace ui {

  void renderDiagnostics(const frame::FrameData& frame_data) {
    ImGui::Begin("Diagnostics");

    ImGui::Text("Average FPS: %.1f", frame_data.avg_fps);
    ImGui::Text("Average Frame Time: %.3f", frame_data.avg_frame_time);

    ImGui::End();
  }

  void renderControls(controls::WinData& window) {
    ImGui::Begin("Controls");

    ImGui::SliderFloat("Window Center", &window.win_center, 0.0f, 1.0f);
    ImGui::SliderFloat("Window Width", &window.win_width, 0.01f, 1.0f);
    ImGui::SliderFloat("Density Scale", &window.density_scale, 0.1f, 1.0f);

    ImGui::End();
  }

} // namespace ui
