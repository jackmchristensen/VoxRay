// ui/imgui_utils.hpp
#pragma once
#include "SDL3/SDL_video.h"

#include "app/frame_data.hpp"

#include "viewport_window.hpp"

namespace ui {

  void initUI(SDL_Window* window, SDL_GLContext context);
  void renderViewport(ViewportWindow& viewport);
  void renderUI(const frame::FrameData& frame_data);

} // namespace uig
