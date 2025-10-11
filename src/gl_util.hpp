#pragma once

#include "defines.hpp"

namespace shrekrooms::gl {


struct Color : public glm::vec4 {
    Color(float r, float g, float b, float a = 1.0f) :
        glm::vec4(r, g, b, a) {}

    Color(const glm::vec4 &v) :
        glm::vec4(v) {}
};


struct Geometry {
    GLuint vao, vbo;
    size_t vertCount;

    Geometry() :
        vao(0), vbo(0) { }

    Geometry(GLuint vao, GLuint vbo) :
        vao(vao), vbo(vbo) { }

    ~Geometry() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
    }
};


using Texture = GLuint;

Texture loadTexture(const std::string &filename, bool interpolation = true); // Defined in gl_util.cpp


} // namespace shrekrooms::gl
