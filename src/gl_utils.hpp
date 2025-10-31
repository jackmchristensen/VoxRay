// gl_utils.hpp
#pragma once
#include <GL/glew.h>
#include <string>

struct Shader       { GLuint id{}; GLenum type{}; };
struct Program      { GLuint id{}; };
struct Buffer       { GLuint id{}; GLenum target{}; };
struct VertexArray  { GLuint id{}; };

bool CompileShader(GLenum type, const char* src, Shader& out, std::string* err);
bool LinkProgram(const Shader& vertex, const Shader& fragment, Program& out, std::string* err);

bool MakeBuffer(GLenum target, GLsizeiptr size, const void* data, GLenum usage, Buffer& out);
bool MakeVao(VertexArray& out);

void Destroy(const Shader& s);
void Destroy(const Program& p);
void Destroy(const Buffer& b);
void Destroy(const VertexArray& v);

inline void UseProgram(const Program& p) { glUseProgram(p.id); }
inline void BindBuffer(const Buffer& b) { glBindBuffer(b.target, b.id); }
inline void BindVao(const VertexArray& v) { glBindVertexArray(v.id); }

