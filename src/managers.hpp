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
        ViewDir,
        FogColor,
        UsePBR,
    };

    UniformManager(GLuint shader);

    // Uniforms
    void useMaterial(const gl::Material &mat) const;
    void setTranslateMatrix(const glm::mat4 &translateMat) const;
    void setRotateMatrix(const glm::mat4 &rotateMat) const;
    void setViewMatrix(const glm::mat4 &viewMat) const;
    void setProjectionMatrix(const glm::mat4 &projectionMat) const;
    void setColor(const gl::Color &color) const;
    void setFogColor(const gl::Color &color) const;
    void setViewPos(const glm::vec3 &pos) const;
    void setViewDir(const glm::vec3 &dir) const;

protected:
    static constexpr size_t s_uniformCount = 9;
    std::array<GLuint, s_uniformCount> m_uniforms;
    GLuint m_shader;

    inline static constexpr size_t s_uniformToId(Uniform uniform);

    GLuint m_getUniformLocation(const std::string &name) const;

};


class MaterialManager {
public:
    enum class MaterialID {
        Null = 0,

        Floor,
        Wall,
        Shrek
    };

    MaterialManager(const UniformManager &uniman);

    const gl::Material &getMaterial(MaterialID material) const;

protected:
    static constexpr size_t s_matCount = 3;
    const UniformManager &m_uniman;
    std::array<std::unique_ptr<gl::Material>, s_matCount> m_materials;

    inline static constexpr size_t s_materialToId(MaterialID texture);

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

    MeshManager(const UniformManager &uniman, const MaterialManager &texman);

    void renderMesh(Mesh mesh) const;

protected:
    static constexpr size_t s_meshCount = 6;
    const UniformManager &m_uniman;
    const MaterialManager &m_matman;

    std::array<gl::Geometry, s_meshCount> m_geometries;
    std::array<MaterialManager::MaterialID, s_meshCount> m_materials;

    inline static constexpr size_t s_meshToId(Mesh mesh);

    void m_bindGeometry(gl::Geometry &geometry, const std::vector<float> &verts);

    void m_genChunkFloor();
    void m_genChunkWallXPos();
    void m_genChunkWallXNeg();
    void m_genChunkWallZPos();
    void m_genChunkWallZneg();
    void m_genShrek();

};


} // namespace shrekrooms
