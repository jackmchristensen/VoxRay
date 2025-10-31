// app_context.hpp
#pragma once
#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>
#include <memory>
#include <type_traits>

using Win = std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)>;
using GLc = std::unique_ptr<std::remove_pointer_t<SDL_GLContext>, decltype(&SDL_GL_DestroyContext)>;

struct AppContext {
  Win window{ nullptr, SDL_DestroyWindow };
  GLc gl_context{nullptr, SDL_GL_DestroyContext};
  int width{};
  int height{};
};

struct AppConfig {
  const char* title = "OpenGL Window";
  int width = 1280;
  int height = 720;
  int gl_major = 3;
  int gl_minor = 3;
};

AppContext MakeApp(AppConfig& config);
void PollInput(bool& running);
void Render(const AppContext& app);
