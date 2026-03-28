// ui/windows.hpp
#pragma once
#include "app/controls_data.hpp"
#include "app/frame_data.hpp"

namespace ui {

  void renderDiagnostics(const frame::FrameData& frame_data);
  void renderControls(controls::WinData& window);

} // namespace ui
