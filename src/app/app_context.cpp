#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_video.h>

#include <stdexcept>

#include "app_context.hpp"
#include "update_flags.hpp"

static void SDL_Throw(const char* err) {
  throw std::runtime_error(std::string(err) + ": " + SDL_GetError());
}

// Handles creation and initialization of SDL components
AppContext MakeApp(AppConfig& config) {
  if (!SDL_Init(SDL_INIT_VIDEO)) SDL_Throw("SDL_Init");

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, config.gl_major);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, config.gl_minor);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  SDL_Window* window = SDL_CreateWindow(config.title, config.width, config.height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
  if (!window) SDL_Throw("SDL_CreateWindow");

  SDL_GLContext context = SDL_GL_CreateContext(window);
  if (!context) {
    SDL_DestroyWindow(window);
    SDL_Quit();
    SDL_Throw("SDL_GL_CreateContext");
  }

  if (!SDL_GL_MakeCurrent(window, context)) {
    SDL_GL_DestroyContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    SDL_Throw("SDL_GL_MakeCurrent");
  }

  SDL_GL_SetSwapInterval(1);

  AppContext app{
    Win{window, SDL_DestroyWindow},
    GLc{context, SDL_GL_DestroyContext},
    config.width,
    config.height
  };

  return app;
}

void PollInput(UpdateFlags& flags) {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_EVENT_KEY_DOWN:
        switch (event.key.scancode) {
          case SDL_SCANCODE_ESCAPE:
            flags |= Stop;
            break;
          default:
            continue;
        }
      case SDL_EVENT_WINDOW_RESIZED:
        flags |= Resize;
        continue;
      case SDL_EVENT_QUIT:
        flags |= Stop;
        break;
    }
  }
}

void UpdateState(UpdateFlags& flags, AppContext& app) {
  if ((flags & Resize) == Resize) {
    SDL_GetWindowSizeInPixels(app.window.get(), &app.width, &app.height);
  }
}

void Draw(const AppContext& app) {
  SDL_GL_SwapWindow(app.window.get());
}
