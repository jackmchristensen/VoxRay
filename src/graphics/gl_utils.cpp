#include <fstream>
#include <iterator>

#include "gl_utils.hpp"

namespace graphics {

bool compileShader(GLenum type, const char* path, Shader& out, std::string* err) {
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

bool linkProgram(const Shader& vertex, const Shader& fragment, Program& out, std::string* err) {
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

bool linkProgram(const Shader& compute, Program& out, std::string* err) {
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

bool makeBuffer(GLenum target, GLsizeiptr size, const void* data, GLenum usage, Buffer& out) {
  GLuint id = 0;
  glGenBuffers(1, &id);
  if (!id) return false;

  glBindBuffer(target, id);
  glBufferData(target, size, data, usage);

  out = Buffer{ id, target };
  return true;
}

bool makeVao(VertexArray& out) {
  GLuint id = 0;
  glGenVertexArrays(1, &id);
  if (!id) return false;
  out = VertexArray{ id };
  return true;
}

bool makeTexture2D(GLenum target, GLenum format, GLsizei width, GLsizei height, Texture& out) {
  GLuint id = 0;
  glCreateTextures(target, 1, &id);
  if (!id) return false;
  glTextureStorage2D(id, 1, format, width, height);
  out = Texture{ id, target, format };
  return true;
}

bool makeTexture3D(GLenum format, GLsizei width, GLsizei height, GLsizei depth, Texture3D& out) {
  GLuint id = 0;
  glCreateTextures(GL_TEXTURE_3D, 1, &id);
  if (!id) return false;

  glTextureStorage3D(id, 1, format, width, height, depth);

  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  out = Texture3D{ id, GL_TEXTURE_3D, format };
  return true;
}

void uploadTexture3D(const Texture3D& tex, const void* data) {
  GLint width, height, depth;
  glGetTextureLevelParameteriv(tex.id, 0, GL_TEXTURE_WIDTH, &width);
  glGetTextureLevelParameteriv(tex.id, 0, GL_TEXTURE_HEIGHT, &height);
  glGetTextureLevelParameteriv(tex.id, 0, GL_TEXTURE_DEPTH, &depth);

  GLenum upload_format = (tex.format == GL_R32F) ? GL_RED : GL_RGBA;
  glTextureSubImage3D(tex.id, 0, 0, 0, 0, width, height, depth, upload_format, GL_FLOAT, data);
}

void destroy(const Shader& s)      { if (s.id) glDeleteShader(s.id); }
void destroy(const Program& p)     { if (p.id) glDeleteProgram(p.id); }
void destroy(const Buffer& b)      { if (b.id) glDeleteBuffers(1, &const_cast<GLuint&>(b.id)); }
void destroy(const VertexArray& a) { if (a.id) glDeleteVertexArrays(1, &const_cast<GLuint&>(a.id)); }
void destroy(const Texture& t)     { if (t.id) glDeleteTextures(1, &const_cast<GLuint&>(t.id)); }
void destroy(const Texture3D& t)   { if (t.id) glDeleteTextures(1, &const_cast<GLuint&>(t.id)); }

}; // namespace graphics
