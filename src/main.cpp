#include "app/update_flags.hpp"
#include "app/app_context.hpp"
#include "app/camera.hpp"

#include "graphics/gl_utils.hpp"

int main() {
  AppConfig config{
    "VoxRay", // title
    1280,     // width
    720,      // height
    4,        // gl_major
    3         // gl_minor
  };

  AppContext app = makeApp(config);
  InputState input;

  if (glewInit() != GLEW_OK) {
    SDL_Log("Failed to initialize GLEW");
    return 1;
  }

  std::string error;
  const char* compute_path = "shaders/compute.glsl";
  graphics::Shader compute{}; graphics::Program compute_prog;
  if (!compileShader(GL_COMPUTE_SHADER, compute_path, compute, &error))     { SDL_Log("%s", error.c_str()); return 1; }
  if (!linkProgram(compute, compute_prog, &error))                          { SDL_Log("%s", error.c_str()); return 1; }
  destroy(compute);

  const char* vertex_path = "shaders/vertex.glsl";
  const char* fragment_path = "shaders/fragment.glsl";
  graphics::Shader vertex{}; graphics::Shader fragment{}; graphics::Program display_prog{};
  if (!compileShader(GL_VERTEX_SHADER, vertex_path, vertex, &error))        { SDL_Log("%s", error.c_str()); return 1; }
  if (!compileShader(GL_FRAGMENT_SHADER, fragment_path, fragment, &error))  { SDL_Log("%s", error.c_str()); return 1; }
  if (!linkProgram(vertex, fragment, display_prog, &error))                 { SDL_Log("%s", error.c_str()); return 1; }
  destroy(vertex); destroy(fragment);

  graphics::VertexArray vao{};
  if (!makeVao(vao)) return 1;

  graphics::Buffer cam_ubo{};
  if (!makeBuffer(GL_UNIFORM_BUFFER, sizeof(glm::mat4)*2 + sizeof(glm::vec4) + sizeof(GLuint)*2, nullptr, GL_DYNAMIC_DRAW, cam_ubo)) return 1;
  glBindBufferBase(GL_UNIFORM_BUFFER, 0, cam_ubo.id);

  graphics::Texture tex{};
  if (!makeTexture2D(GL_TEXTURE_2D, GL_RGBA32F, app.width, app.height, tex)) return 1;
  glBindImageTexture(1, tex.id, 0, GL_FALSE, 0, GL_WRITE_ONLY, tex.format);

  // UseProgram(display_prog);
  bindVao(vao);

  // Test camera setup
  {
    auto& check_cam = activeCamera(app);
    cam::orbit(check_cam, glm::pi<float>()/4.f, -glm::pi<float>()/8.f);
    cam::zoomFOV(check_cam, -30.f);
    cam::updateView(check_cam);
    cam::updateProject(check_cam);
    printf("Camera position: (%.3f, %.3f, %.3f)\n", check_cam.position.x, check_cam.position.y, check_cam.position.z);
    printf("Camera look vector: (%.3f, %.3f, %.3f)\n", check_cam.forward.x, check_cam.forward.y, check_cam.forward.z);
  }

  UpdateFlags flags = NONE;
  while ((flags & STOP) != STOP) {
    glViewport(0, 0, app.width, app.height);
    glClear(GL_COLOR_BUFFER_BIT);

    pollInput(flags, input);
    if (flags != NONE) updateState(flags, app, input);

    useProgram(compute_prog);
    auto& c = activeCamera(app);
    glm::vec4 pos = glm::vec4(glm::vec3(c.position), 1.f);
    glBindBuffer(cam_ubo.target, cam_ubo.id);
    glBufferSubData(cam_ubo.target, 0,                   sizeof(glm::mat4), &c.view[0][0]);
    glBufferSubData(cam_ubo.target, sizeof(glm::mat4),   sizeof(glm::mat4), &c.proj[0][0]);
    glBufferSubData(cam_ubo.target, sizeof(glm::mat4)*2, sizeof(glm::vec4), &pos.x);
    glBufferSubData(cam_ubo.target, sizeof(glm::mat4)*2 + sizeof(glm::vec4), sizeof(GLuint), &app.width);
    glBufferSubData(cam_ubo.target, sizeof(glm::mat4)*2 + sizeof(glm::vec4) + sizeof(GLuint), sizeof(GLuint), &app.height);

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
