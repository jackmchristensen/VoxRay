#include "app/update_flags.hpp"
#include "app/app_context.hpp"
#include "app/camera.hpp"
#include "gl_utils.hpp"

int main() {
  AppConfig config{
    "VoxRay", // title
    1280,     // width
    720,      // height
    4,        // gl_major
    3         // gl_minor
  };

  AppContext app = makeApp(config);

  if (glewInit() != GLEW_OK) {
    SDL_Log("Failed to initialize GLEW");
    return 1;
  }

  std::string error;
  const char* compute_path = "shaders/compute.glsl";
  Shader compute{}; Program compute_prog;
  if (!compileShader(GL_COMPUTE_SHADER, compute_path, compute, &error))     { SDL_Log("%s", error.c_str()); return 1; }
  if (!linkProgram(compute, compute_prog, &error))                          { SDL_Log("%s", error.c_str()); return 1; }
  destroy(compute);

  const char* vertex_path = "shaders/vertex.glsl";
  const char* fragment_path = "shaders/fragment.glsl";
  Shader vertex{}; Shader fragment{}; Program display_prog{};
  if (!compileShader(GL_VERTEX_SHADER, vertex_path, vertex, &error))        { SDL_Log("%s", error.c_str()); return 1; }
  if (!compileShader(GL_FRAGMENT_SHADER, fragment_path, fragment, &error))  { SDL_Log("%s", error.c_str()); return 1; }
  if (!linkProgram(vertex, fragment, display_prog, &error))                 { SDL_Log("%s", error.c_str()); return 1; }
  destroy(vertex); destroy(fragment);

  VertexArray vao{};
  if (!makeVao(vao)) return 1;

  Buffer cam_ubo{};
  if (!makeBuffer(GL_UNIFORM_BUFFER, sizeof(glm::mat4)*2 + sizeof(glm::vec4) + sizeof(GLuint)*2, nullptr, GL_DYNAMIC_DRAW, cam_ubo)) return 1;
  glBindBufferBase(GL_UNIFORM_BUFFER, 0, cam_ubo.id);

  Texture tex{};
  if (!makeTexture2D(GL_TEXTURE_2D, GL_RGBA32F, app.width, app.height, tex)) return 1;
  glBindImageTexture(1, tex.id, 0, GL_FALSE, 0, GL_WRITE_ONLY, tex.format);

  // UseProgram(display_prog);
  bindVao(vao);

  UpdateFlags flags = None;
  while ((flags & Stop) != Stop) {
    glViewport(0, 0, app.width, app.height);
    glClear(GL_COLOR_BUFFER_BIT);

    pollInput(flags);
    if (flags != None) updateState(flags, app);

    useProgram(compute_prog);
    auto& c = ActiveCamera(app);
    glm::vec4 pos = glm::vec4(glm::vec3(c.position), 1.f);
    glBindBuffer(cam_ubo.target, cam_ubo.id);
    glBufferSubData(cam_ubo.target, 0,                   sizeof(glm::mat4), &c.view[0][0]);
    glBufferSubData(cam_ubo.target, sizeof(glm::mat4),   sizeof(glm::mat4), &c.proj[0][0]);
    glBufferSubData(cam_ubo.target, sizeof(glm::mat4)*2, sizeof(glm::vec4), &pos.x);
    glBufferSubData(cam_ubo.target, sizeof(glm::mat4)*2 + sizeof(glm::vec4), sizeof(GLuint), &app.width);
    glBufferSubData(cam_ubo.target, sizeof(glm::mat4)*2 + sizeof(glm::vec4) + sizeof(GLuint), sizeof(GLuint), &app.height);
    printf("(%d, %d)\n", app.width, app.height);

    GLuint gx = (app.width + 16 - 1) / 16;
    GLuint gy = (app.height + 16 - 1) / 16;
    glDispatchCompute(gx, gy, 1);

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

    useProgram(display_prog);
    glBindTextureUnit(0, tex.id);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Currently just swaps window
    // Keeping this because I might add more functionality in the future
    draw(app);
  }

  destroy(vao);
  destroy(compute_prog);
  destroy(display_prog);
  SDL_GL_DestroyContext(app.gl_context.get());
  SDL_DestroyWindow(app.window.get());
  SDL_Quit();
  return 0;
}
