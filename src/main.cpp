#include "app/camera.hpp"
#include "gl_utils.hpp"
#include <GL/glext.h>

#include "app/update_flags.hpp"
#include "app/app_context.hpp"

int main() {
  AppConfig config{
    "VoxRay", // title
    1280,     // width
    720,      // height
    4,        // gl_major
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
  Destroy(vertex); Destroy(fragment);

  VertexArray vao{};
  if (!MakeVao(vao)) return 1;

  Buffer cam_ubo{};
  if (!MakeBuffer(GL_UNIFORM_BUFFER, sizeof(glm::mat4)*2 + sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW, cam_ubo)) return 1;
  glBindBufferBase(GL_UNIFORM_BUFFER, 0, cam_ubo.id);

  UseProgram(program);
  BindVao(vao);

  UpdateFlags flags = None;
  while ((flags & Stop) != Stop) {
    glViewport(0, 0, app.width, app.height);
    glClear(GL_COLOR_BUFFER_BIT);

    PollInput(flags);
    if (flags != None) UpdateState(flags, app);

    auto& c = ActiveCamera(app);
    glm::vec4 pos = glm::vec4(glm::vec3(c.position), 1.f);
    glBindBuffer(cam_ubo.target, cam_ubo.id);
    glBufferSubData(cam_ubo.target, 0,                   sizeof(glm::mat4), &c.view[0][0]);
    glBufferSubData(cam_ubo.target, sizeof(glm::mat4),   sizeof(glm::mat4), &c.proj[0][0]);
    glBufferSubData(cam_ubo.target, sizeof(glm::mat4)*2, sizeof(glm::vec4), &pos.x);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Currently just swaps window
    // Keeping this because I might add more functionality in the future
    Draw(app);
  }

  Destroy(vao);
  Destroy(program);
  SDL_GL_DestroyContext(app.gl_context.get());
  SDL_DestroyWindow(app.window.get());
  SDL_Quit();
  return 0;
}
