#include "gl.hpp"

// stb image
#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include <stb_image.h>


shrekrooms::gl::Texture::Texture(const std::string &filename, bool interpolation) {
    int channels;
    stbi_uc *data = stbi_load(filename.c_str(), &m_width, &m_height, &channels, STBI_rgb_alpha);

    if (!data)
        throw error { "gl.cpp", "shrekrooms::gl::Texture::Texture", "Texture failed to load:\n" } << stbi_failure_reason();

    m_tex = std::make_shared<GLuint>(0);
    glGenTextures(1, m_tex.get());
    glBindTexture(GL_TEXTURE_2D, *m_tex);

    glTexImage2D(
        GL_TEXTURE_2D,
        0, GL_RGBA, m_width, m_height, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, data
    );

    stbi_image_free(data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (interpolation ? GL_LINEAR : GL_NEAREST));
    glGenerateMipmap(GL_TEXTURE_2D);
}
