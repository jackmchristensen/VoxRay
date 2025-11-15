// graphics/gl_utils.hpp
#pragma once
#include <GL/glew.h>

#include <string>

namespace graphics {

struct Shader       { GLuint id{}; GLenum type{}; };
struct Program      { GLuint id{}; };
struct Buffer       { GLuint id{}; GLenum target{}; };
struct VertexArray  { GLuint id{}; };
struct Texture      { GLuint id{}; GLenum target{}; GLenum format{}; };
struct Texture3D    { GLuint id{}; GLenum target{}; GLenum format{}; };

bool compileShader(GLenum type, const char* src, Shader& out, std::string* err);
bool linkProgram(const Shader& vertex, const Shader& fragment, Program& out, std::string* err);
bool linkProgram(const Shader& compute, Program& out, std::string* err);

bool makeBuffer(GLenum target, GLsizeiptr size, const void* data, GLenum usage, Buffer& out);
bool makeVao(VertexArray& out);
bool makeTexture2D(GLenum target, GLenum format, GLsizei width, GLsizei height, Texture& out);
bool makeTexture3D(GLenum format, GLsizei width, GLsizei height, GLsizei depth, Texture3D& out);

void uploadTexture2D(const Texture& tex, const void* data);
void uploadTexture3D(const Texture3D& tex, const void* data);

inline void bindTexture(const Texture& t, GLuint unit)      { glBindTextureUnit(unit, t.id); }
inline void bindTexture3D(const Texture3D& t, GLuint unit)  { glBindTextureUnit(unit, t.id); }

void destroy(const Shader& s);
void destroy(const Program& p);
void destroy(const Buffer& b);
void destroy(const VertexArray& v);
void destroy(const Texture& t);
void destroy(const Texture3D& t);

inline void useProgram(const Program& p) { glUseProgram(p.id); }
inline void bindBuffer(const Buffer& b) { glBindBuffer(b.target, b.id); }
inline void bindVao(const VertexArray& v) { glBindVertexArray(v.id); }

}; // namespace graphics
