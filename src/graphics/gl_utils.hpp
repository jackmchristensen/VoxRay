// gl_utils.hpp
#pragma once
#include <GL/glew.h>

#include <string>

namespace graphics {

struct Shader       { GLuint id{}; GLenum type{}; };
struct Program      { GLuint id{}; };
struct Buffer       { GLuint id{}; GLenum target{}; };
struct VertexArray  { GLuint id{}; };
struct Texture      { GLuint id{}; GLenum target{}; GLenum format{}; };

bool compileShader(GLenum type, const char* src, Shader& out, std::string* err);
bool linkProgram(const Shader& vertex, const Shader& fragment, Program& out, std::string* err);
bool linkProgram(const Shader& compute, Program& out, std::string* err);

bool makeBuffer(GLenum target, GLsizeiptr size, const void* data, GLenum usage, Buffer& out);
bool makeVao(VertexArray& out);
bool makeTexture2D(GLenum target, GLenum format, GLsizei width, GLsizei height, Texture& out);

void destroy(const Shader& s);
void destroy(const Program& p);
void destroy(const Buffer& b);
void destroy(const VertexArray& v);

inline void useProgram(const Program& p) { glUseProgram(p.id); }
inline void bindBuffer(const Buffer& b) { glBindBuffer(b.target, b.id); }
inline void bindVao(const VertexArray& v) { glBindVertexArray(v.id); }

}; // namespace graphics
