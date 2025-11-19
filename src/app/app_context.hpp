// app_context.hpp
#pragma once
#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>

#include <cstddef>
#include <memory>
#include <type_traits>
#include <vector>

#include "ui/viewport_window.hpp"

#include "camera.hpp"
#include "update_flags.hpp"
#include "input_state.hpp"

using Win = std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)>;
using GLc = std::unique_ptr<std::remove_pointer_t<SDL_GLContext>, decltype(&SDL_GL_DestroyContext)>;

// Contains variables that need to exist for lifetime of entire app run
struct AppContext {
  // --- Window variables ---
  Win window{ nullptr, SDL_DestroyWindow };
  GLc gl_context{nullptr, SDL_GL_DestroyContext};
  int width{};
  int height{};
  float dpi_scalar = 1.667f;

  // --- Cameras ---
  std::vector<cam::Camera>  cameras;
  std::size_t               active = 0;
};

inline       cam::Camera& activeCamera(AppContext& app)       { return app.cameras[app.active]; }
inline const cam::Camera& activeCamera(const AppContext& app) { return app.cameras[app.active]; }

// Initial configuration settings
struct AppConfig {
  const char* title = "OpenGL Window";
  int width = 1280;
  int height = 720;
  int gl_major = 3;
  int gl_minor = 3;
};

AppContext makeApp(AppConfig& config);
void pollInput(UpdateFlags& flags, InputState& input);
void updateState(UpdateFlags& flags, AppContext& app, InputState& input, const ui::ViewportWindow& viewport);
void draw(const AppContext& app);
