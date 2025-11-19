// graphics/update_graphics.hpp
#pragma once

#include "app/update_flags.hpp"

#include "ui/viewport_window.hpp"

#include "render_targets.hpp"

namespace graphics {

  void updateGraphicsState(UpdateFlags& flags, RenderTargets& targets, ui::ViewportWindow& viewport);

} // namespace graphics
