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
    ImGui::End();
  }

} // namespace ui
