#pragma once

#include "defines.hpp"


namespace shrekrooms::shaders {


GLuint makeShaderModule(const char *filename, GLuint moduleType);
GLuint makeShaderProgram();


} // namespace shrekrooms::shaders
