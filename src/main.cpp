#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>

#include "app/app_context.hpp"
#include "gl_utils.hpp"

int main() {
  AppConfig config{
    "VoxRay", // title
    1280,     // width
    720,      // height
    3,        // gl_major
    3         // gl_minor
  };

  AppContext app = MakeApp(config);

  if (glewInit() != GLEW_OK) {
    SDL_Log("Failed to initialize GLEW");
    return 1;
  }

  const char* vertex_path = "shaders/vertex.glsl";
  const char* fragment_path = "shaders/fragment.glsl";
  Shader vertex{}; Shader fragment{}; Program program{}; std::string error;
  if (!CompileShader(GL_VERTEX_SHADER, vertex_path, vertex, &error))        { SDL_Log("%s", error.c_str()); return 1; }
  if (!CompileShader(GL_FRAGMENT_SHADER, fragment_path, fragment, &error))  { SDL_Log("%s", error.c_str()); return 1; }
  if (!LinkProgram(vertex, fragment, program, &error))                      { SDL_Log("%s", error.c_str()); return 1; }

  VertexArray vao{}; Buffer pbo{};
  if (!MakeVao(vao)) return 1;
  if (!MakeBuffer(GL_PIXEL_UNPACK_BUFFER, app.width * app.height * 4, nullptr, GL_DYNAMIC_DRAW, pbo)) return 1;

  bool running = true;
  while (running) {
    glViewport(0, 0, app.width, app.height);
    glClear(GL_COLOR_BUFFER_BIT);

    PollInput(running);

    UseProgram(program);
    BindVao(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    SDL_GL_SwapWindow(app.window.get());
    // Render(app);
  }

  SDL_Quit();
  return 0;
}
