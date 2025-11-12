#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>

#include <stdexcept>

#include "app_context.hpp"
#include "camera.hpp"
#include "update_flags.hpp"

static void SDL_Throw(const char* err) {
  throw std::runtime_error(std::string(err) + ": " + SDL_GetError());
}

// Handles creation and initialization of SDL components
AppContext makeApp(AppConfig& config) {
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

  AppContext app{};
  app.window      = Win{ window, SDL_DestroyWindow };
  app.gl_context  = GLc { context, SDL_GL_DestroyContext };
  app.width       = config.width;
  app.height      = config.height;

  return app;
}

// Mask flag to signal for update
void pollInput(UpdateFlags& flags, InputState& input) {
  // Rest per-frame data
  input.mouse_dx = 0.f;
  input.mouse_dy = 0.f;
  input.scroll_dx = 0.f;
  input.scroll_dy = 0.f;

  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_EVENT_KEY_DOWN:
        if (event.key.scancode == SDL_SCANCODE_ESCAPE) {
            flags |= STOP;
        }
        if (event.key.scancode == SDL_SCANCODE_LALT || event.key.scancode == SDL_SCANCODE_RALT) {
            input.alt_held = true;
        }
        if (event.key.scancode == SDL_SCANCODE_LCTRL) {
            input.ctrl_held = true;
        }
        break;
      case SDL_EVENT_KEY_UP:
        if (event.key.scancode == SDL_SCANCODE_LALT || event.key.scancode == SDL_SCANCODE_RALT) {
            input.alt_held = false;
        }
        break;
      case SDL_EVENT_MOUSE_BUTTON_DOWN:
        if (event.button.button == 1) input.lmb_held = true;
        if (event.button.button == 2) input.mmb_held = true;
        if (event.button.button == 3) input.rmb_held = true;
        break;
      case SDL_EVENT_MOUSE_BUTTON_UP:
        if (event.button.button == 1) input.lmb_held = false;
        if (event.button.button == 2) input.mmb_held = false;
        if (event.button.button == 3) input.rmb_held = false;
        break;
      case SDL_EVENT_MOUSE_MOTION:
        if (input.alt_held && input.rmb_held) {
            input.mouse_dx += event.motion.xrel;
            input.mouse_dy += event.motion.yrel;
            flags |= ORBIT;
        }
        break;
      case SDL_EVENT_WINDOW_RESIZED:
        flags |= RESIZE;
        break;
      case SDL_EVENT_QUIT:
        flags |= STOP;
        break;
    }
  }
}

// Separate function from PollInputs()
void updateState(UpdateFlags& flags, AppContext& app, InputState& input) {
  auto& camera = activeCamera(app);
  bool camera_moved = false;

  if ((flags & RESIZE) == RESIZE) {
    SDL_GetWindowSizeInPixels(app.window.get(), &app.width, &app.height);
    cam::setAspectRatio(camera, float(app.width) / app.height);
    camera_moved = true;
    flags &= ~RESIZE;
  }

  if ((flags & ORBIT) == ORBIT) {
    // printf("Orbit activated!\n");
    printf("mouse_dx = %.9f\n", input.mouse_dx);
    float yaw = input.mouse_dx * 0.02;
    float pitch = input.mouse_dy * 0.02;
    cam::orbit(camera, yaw, pitch);
    camera_moved = true;
    flags &= ~ORBIT;
  }

  if (camera_moved) {
    cam::updateView(camera);
    cam::updateProject(camera);
  }
}

void draw(const AppContext& app) {
  SDL_GL_SwapWindow(app.window.get());
}
