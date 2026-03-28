#include "imgui.h"

#include "windows.hpp"

namespace ui {

  void renderDiagnostics(const frame::FrameData& frame_data) {
    ImGui::Begin("Diagnostics");

    ImGui::Text("Average FPS: %.1f", frame_data.avg_fps);
    ImGui::Text("Average Frame Time: %.3f", frame_data.avg_frame_time);

    ImGui::End();
  }

  void renderControls() {
    ImGui::Begin("Controls");

    float win_center = 0.5f;
    float win_width = 0.5f;
    float dendsity_scale = 0.5f;

    ImGui::SliderFloat("Window Center", &win_center, 0.0f, 1.0f);
    ImGui::SliderFloat("Window Width", &win_width, 0.01f, 1.0f);
    ImGui::SliderFloat("Density Scale", &dendsity_scale, 0.1f, 1.0f);

    ImGui::End();
  }

} // namespace ui
