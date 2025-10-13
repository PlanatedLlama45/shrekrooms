#pragma once

#include "defines.hpp"
#include "gl_util.hpp"


namespace shrekrooms {


class UniformManager {
public:
    enum class Uniform {
        Null = 0,

        Translate,
        Rotate,
        View,
        Projection,
        Color,
        ViewPos,
        FogColor
    };

    UniformManager(GLuint shader);

    // Uniforms
    void useTexture(gl::Texture tex) const;
    void setTranslateMatrix(const glm::mat4 &translateMat) const;
    void setRotateMatrix(const glm::mat4 &rotateMat) const;
    void setViewMatrix(const glm::mat4 &viewMat) const;
    void setProjectionMatrix(const glm::mat4 &projectionMat) const;
    void setColor(const gl::Color &color) const;
    void setFogColor(const gl::Color &color) const;
    void setViewPos(const glm::vec3 &pos) const;

protected:
    static constexpr size_t s_uniformCount = 6;
    std::array<GLuint, s_uniformCount> m_uniforms;
    GLuint m_shader;

    static constexpr size_t s_uniformToId(Uniform uniform);

    GLuint m_getUniformLocation(const std::string &name) const;

};


class TextureManager {
public:
    enum class TextureID {
        Null = 0,

        Floor,
        Wall,
        Shrek
    };

    TextureManager(const UniformManager &uniman);
    ~TextureManager();

    gl::Texture getTexture(TextureID texture) const;

protected:
    static constexpr size_t s_texCount = 3;
    const UniformManager &m_uniman;
    std::array<gl::Texture, s_texCount> m_textures;

    static constexpr size_t s_textureToId(TextureID texture);

};


class MeshManager {
public:
    enum class Mesh {
        Null = 0,

        ChunkFloor,
        ChunkWallXPos,
        ChunkWallXNeg,
        ChunkWallZPos,
        ChunkWallZNeg,
        Shrek
    };

    MeshManager(const UniformManager &uniman, const TextureManager &texman);

    void renderMesh(Mesh mesh) const;

protected:
    static constexpr size_t s_meshCount = 6;
    const TextureManager &m_texman;
    const UniformManager &m_uniman;
    std::array<gl::Geometry, s_meshCount> m_geometries;
    std::array<gl::Texture, s_meshCount> m_textures;

    static constexpr size_t s_meshToId(Mesh mesh);

    void m_bindGeometry(gl::Geometry &geometry, const std::vector<float> &verts);

    void m_genChunkFloor();
    void m_genChunkWallXPos();
    void m_genChunkWallXNeg();
    void m_genChunkWallZPos();
    void m_genChunkWallZneg();
    void m_genShrek();

};


} // namespace shrekrooms
