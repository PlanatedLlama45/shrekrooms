#include "gl_util.hpp"

// stb image
#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include <stb_image.h>


shrekrooms::gl::Texture shrekrooms::gl::loadTexture(const std::string &filename, bool interpolation) {
    Texture tex;
    int channels, width, height;
    stbi_uc *data = stbi_load(filename.c_str(), &width, &height, &channels, STBI_rgb_alpha);

    if (!data)
        throw error { "gl.cpp", "shrekrooms::gl::Texture::Texture", "Texture failed to load:\n" } << stbi_failure_reason();

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexImage2D(
        GL_TEXTURE_2D,
        0, GL_RGBA, width, height, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, data
    );

    stbi_image_free(data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (interpolation ? GL_LINEAR : GL_NEAREST));
    glGenerateMipmap(GL_TEXTURE_2D);

    return tex;
}
