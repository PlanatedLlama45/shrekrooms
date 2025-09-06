#pragma once

#include "imports.hpp"

namespace shrekrooms::shaders {


inline GLuint makeShaderModule(const std::string &filename, GLuint moduleType) {
	std::ifstream file(filename);

	std::string line, shaderSrc;
	while (std::getline(file, line))
		shaderSrc += line + '\n';

	file.close();
    const GLchar *shaderSrc_c = shaderSrc.c_str();

    GLuint shaderModule = glCreateShader(moduleType);
    glShaderSource(shaderModule, 1, &shaderSrc_c, NULL);
    glCompileShader(shaderModule);

    int success;
    glGetShaderiv(shaderModule, GL_COMPILE_STATUS, &success);

    if (!success) {
        char errorLog[1024];
        glGetShaderInfoLog(shaderModule, 1024, NULL, errorLog);
        throw error { "shaders.hpp", "shrekrooms::shaders::makeShaderModule", "Shader Module compilation error:\n" } << errorLog;
    }

    return shaderModule;
}

inline GLuint makeShaderProgram() {
    GLuint vertexModule = makeShaderModule("../shaders/vertex.glsl", GL_VERTEX_SHADER);
    GLuint fragmentModule = makeShaderModule("../shaders/fragment.glsl", GL_FRAGMENT_SHADER);

    GLuint shader = glCreateProgram();

    glAttachShader(shader, vertexModule);
    glAttachShader(shader, fragmentModule);

    glLinkProgram(shader);

    int success;
    glGetProgramiv(shader, GL_LINK_STATUS, &success);

    if (!success) {
        char errorLog[1024];
        glGetProgramInfoLog(shader, 1024, NULL, errorLog);
        throw error { "gl.hpp", "shrekrooms::gl::GLContext::m_makeShaderProgram", "Shader linking error:\n" } << errorLog;
    }

    glDeleteShader(vertexModule);
    glDeleteShader(fragmentModule);

    return shader;
}


} // namespace shrekrooms::shaders
