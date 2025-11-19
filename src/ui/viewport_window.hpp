// ui/viewport_window.hpp
#pragma once
#include <string>

#include "graphics/gl_utils.hpp"

namespace ui {

  struct ViewportWindow {
    std::string name;
    bool open = true;
    bool update_framebuffer = true;

    graphics::Framebuffer fbo;
    graphics::Texture texture;

    int width = 800;
    int height = 600;
    size_t camera_index = 0;
  };

} // namespace ui
