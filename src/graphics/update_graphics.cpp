#include "update_graphics.hpp"

namespace graphics {

  void updateGraphicsState(UpdateFlags& flags, RenderTargets &targets, ui::ViewportWindow& viewport) {
    if ((flags & VIEWPORT_RESIZE) == VIEWPORT_RESIZE){
      resizeRenderTargets(viewport.width, viewport.height, targets);

      flags &= ~VIEWPORT_RESIZE;
    }
  }

} // namespace graphics
