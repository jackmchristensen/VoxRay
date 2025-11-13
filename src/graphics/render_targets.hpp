// graphics/render_targets.hpp
#pragma once
#include "gl_utils.hpp"

namespace graphics {
  struct RenderTargets {
    Texture albedo;
    Texture depth;
    Texture normal;

    int width = 0;
    int height = 0;
  };

  inline bool makeRenderTargets(int width, int height, RenderTargets& out) {
    if (!makeTexture2D(GL_TEXTURE_2D, GL_RGBA32F, width, height, out.albedo)) return false;
    if (!makeTexture2D(GL_TEXTURE_2D, GL_RGBA32F, width, height, out.depth))  return false;
    if (!makeTexture2D(GL_TEXTURE_2D, GL_RGBA32F, width, height, out.normal)) return false;

    out.width = width;
    out.height = height;

    return true;
  }

  inline bool resizeRenderTargets(int width, int height, RenderTargets& targets) {
    destroy(targets.albedo);
    destroy(targets.depth);
    destroy(targets.normal);

    return makeRenderTargets(width, height, targets);
  }

  inline void destroy(const RenderTargets& targets) {
    destroy(targets.albedo);
    destroy(targets.depth);
    destroy(targets.normal);
  }

  inline void bindForCompute(const RenderTargets& targets) {
    glBindImageTexture(0, targets.albedo.id, 0, GL_FALSE, 0, GL_WRITE_ONLY, targets.albedo.format);
    glBindImageTexture(1, targets.depth.id, 0, GL_FALSE, 0, GL_WRITE_ONLY, targets.depth.format);
    glBindImageTexture(2, targets.normal.id, 0, GL_FALSE, 0, GL_WRITE_ONLY, targets.normal.format);
  }

  inline void bindForDisplay(const RenderTargets& targets) {
    glBindTextureUnit(0, targets.albedo.id);
    glBindTextureUnit(1, targets.depth.id);
    glBindTextureUnit(2, targets.normal.id);
  }
} // namespace graphics
