#include <fstream>
#include <iterator>

#include "gl_utils.hpp"

bool CompileShader(GLenum type, const char* path, Shader& out, std::string* err) {
  std::fstream in { path };
  std::string source_string = { std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>() };

  if (source_string.empty()) {
    // TODO make error log more descriptive
    *err = "Failed to load shader from path: ";
    *err += path;
    return false;
  }

  const char* source = source_string.c_str();

  GLuint id = glCreateShader(type);
  glShaderSource(id, 1, &source, nullptr);
  glCompileShader(id);

  GLint ok = 0; glGetShaderiv(id, GL_COMPILE_STATUS, &ok);
  if (!ok) {
    glDeleteShader(id);
    // TODO make error log more descriptive
    *err = "Shader compilation failed";
    return false;
  }

  out = Shader{ id, type };
  return true;
}

bool LinkProgram(const Shader& vertex, const Shader& fragment, Program& out, std::string* err) {
  GLuint program = glCreateProgram();
  glAttachShader(program, vertex.id);
  glAttachShader(program, fragment.id);
  glLinkProgram(program);

  GLint ok = 0; glGetProgramiv(program, GL_LINK_STATUS, &ok);
  if (!ok) {
    glDeleteProgram(program);
    // TODO make error log more descriptive
    *err = "Program link failed";
    return false;
  }

  out = Program{ program };
  return true;
}

bool LinkProgram(const Shader& compute, Program& out, std::string* err) {
  GLuint program = glCreateProgram();
  glAttachShader(program, compute.id);
  glLinkProgram(program);

  GLint ok = 0; glGetProgramiv(program, GL_LINK_STATUS, &ok);
  if (!ok) {
    glDeleteProgram(program);
    // TODO make error log more descriptive
    *err = "Program link failed";
    return false;
  }

  out = Program{ program };
  return true;
}

bool MakeBuffer(GLenum target, GLsizeiptr size, const void* data, GLenum usage, Buffer& out) {
  GLuint id = 0;
  glGenBuffers(1, &id);
  if (!id) return false;

  glBindBuffer(target, id);
  glBufferData(target, size, data, usage);

  out = Buffer{ id, target };
  return true;
}

bool MakeVao(VertexArray& out) {
  GLuint id = 0;
  glGenVertexArrays(1, &id);
  if (!id) return false;
  out = VertexArray{ id };
  return true;
}

bool MakeTexture2D(GLenum target, GLenum format, GLsizei width, GLsizei height, Texture& out) {
  GLuint id = 0;
  glCreateTextures(target, 1, &id);
  if (!id) return false;
  glTextureStorage2D(id, 1, format, width, height);
  out = Texture{ id, target, format };
  return true;
}

void Destroy(const Shader& s)      { if (s.id) glDeleteShader(s.id); }
void Destroy(const Program& p)     { if (p.id) glDeleteProgram(p.id); }
void Destroy(const Buffer& b)      { if (b.id) glDeleteBuffers(1, &const_cast<GLuint&>(b.id)); }
void Destroy(const VertexArray& a) { if (a.id) glDeleteVertexArrays(1, &const_cast<GLuint&>(a.id)); }
