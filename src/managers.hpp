#pragma once

#include "defines.hpp"
#include "gl_util.hpp"


namespace shrekrooms {


class UniformManager {
public:
    enum class Uniform {
        Null = 0,

        Translate,
        View,
        Projection,
        Color,
        ViewPos,
        FogColor
    };

    UniformManager(GLuint shader) :
            m_shader(shader) {
        m_uniforms[s_uniformToId(Uniform::Translate)]  = m_getUniformLocation("u_translate");
        m_uniforms[s_uniformToId(Uniform::View)]       = m_getUniformLocation("u_view");
        m_uniforms[s_uniformToId(Uniform::Projection)] = m_getUniformLocation("u_projection");
        m_uniforms[s_uniformToId(Uniform::Color)]      = m_getUniformLocation("u_color");
        m_uniforms[s_uniformToId(Uniform::ViewPos)]    = m_getUniformLocation("u_viewPos");
        m_uniforms[s_uniformToId(Uniform::FogColor)]   = m_getUniformLocation("u_fogColor");
    }

    // Uniforms
    void useTexture(gl::Texture tex) const {
        glBindTexture(GL_TEXTURE_2D, tex);
    }

    void setTranslateMatrix(const glm::mat4 &translateMat) const {
        glUniformMatrix4fv(m_uniforms[s_uniformToId(Uniform::Translate)], 1, GL_FALSE, glm::value_ptr(translateMat));
    }

    void setViewMatrix(const glm::mat4 &viewMat) const {
        glUniformMatrix4fv(m_uniforms[s_uniformToId(Uniform::View)], 1, GL_FALSE, glm::value_ptr(viewMat));
    }

    void setProjectionMatrix(const glm::mat4 &projectionMat) const {
        glUniformMatrix4fv(m_uniforms[s_uniformToId(Uniform::Projection)], 1, GL_FALSE, glm::value_ptr(projectionMat));
    }

    void setColor(const gl::Color &color) const {
        glUniform4fv(m_uniforms[s_uniformToId(Uniform::Color)], 1, glm::value_ptr(static_cast<glm::vec4>(color)));
    }

    void setFogColor(const gl::Color &color) const {
        glUniform4fv(m_uniforms[s_uniformToId(Uniform::FogColor)], 1, glm::value_ptr(static_cast<glm::vec4>(color)));
    }

    void setViewPos(const glm::vec3 &pos) const {
        glUniform3fv(m_uniforms[s_uniformToId(Uniform::ViewPos)], 1, glm::value_ptr(pos));
    }

protected:
    static constexpr size_t s_uniformCount = 6;
    std::array<GLuint, s_uniformCount> m_uniforms;
    GLuint m_shader;

    static constexpr size_t s_uniformToId(Uniform uniform) {
        return static_cast<size_t>(uniform) - 1;
    }

    GLuint m_getUniformLocation(const std::string &name) const {
        return glGetUniformLocation(m_shader, name.c_str());
    }
};


class TextureManager {
public:
    enum class TextureID {
        Null = 0,

        Floor,
        Wall,
        Shrek
    };

    TextureManager(const UniformManager &uniman) :
            m_uniman(uniman), m_textures() {
        m_textures[s_textureToId(TextureID::Floor)] = gl::loadTexture("../img/floor.jpg");
        m_textures[s_textureToId(TextureID::Wall)] = gl::loadTexture("../img/wall.jpg");
        m_textures[s_textureToId(TextureID::Shrek)] = gl::loadTexture("../img/shrek.jpg");
    }

    ~TextureManager() {
        glDeleteTextures(s_texCount, m_textures.data());
    }

    gl::Texture getTexture(TextureID texture) const {
        return m_textures[s_textureToId(texture)];
    }

protected:
    static constexpr size_t s_texCount = 3;
    const UniformManager &m_uniman;
    std::array<gl::Texture, s_texCount> m_textures;

    static constexpr size_t s_textureToId(TextureID texture) {
        return static_cast<size_t>(texture) - 1;
    }


};


#define _M_SHREKROOMS_DEFINE_WORLD_DATA_CONSTEXPR()                                                 \
    constexpr float pmax = 0.5f * defines::world::chunkSize;                                        \
    constexpr float ymax = 0.5f * defines::world::chunkHeight;                                      \
    constexpr float wmax = pmax - defines::world::wallThicknessHalf;                                \
    constexpr float gmax = pmax + defines::world::wallThicknessHalf - 2.0f*defines::epsilon;        \
    constexpr float tfmax = defines::world::chunkFloorTiles;                                        \
    constexpr float twmax = defines::world::chunkWallTiles;                                         \
    constexpr float tgmax = twmax * (2.0f * defines::world::wallThicknessHalf / defines::world::chunkSize);


class MeshManager {
public:
    enum class Mesh {
        Null = 0,

        ChunkFloor,
        ChunkWallXPos,
        ChunkWallXNeg,
        ChunkWallZPos,
        ChunkWallZNeg,
    };

    MeshManager(const UniformManager &uniman, const TextureManager &texman) :
            m_uniman(uniman), m_texman(texman) {
        m_genChunkFloor();
        m_genChunkWallXPos();
        m_genChunkWallXNeg();
        m_genChunkWallZPos();
        m_genChunkWallZneg();
    }

    void renderMesh(Mesh mesh) const {
        if (mesh == Mesh::Null)
            return;

        const size_t meshId = s_meshToId(mesh);

        m_uniman.useTexture(m_textures[meshId]);

        const gl::Geometry &geo = m_geometries[meshId];
        glBindVertexArray(geo.vao);
        glDrawArrays(GL_TRIANGLES, 0, geo.vertCount);
    }

protected:
    static constexpr size_t s_meshCount = 5;
    const TextureManager &m_texman;
    std::array<gl::Geometry, s_meshCount> m_geometries;
    std::array<gl::Texture, s_meshCount> m_textures;

    static constexpr size_t s_meshToId(Mesh mesh) {
        return static_cast<size_t>(mesh) - 1;
    }

    const UniformManager &m_uniman;

    void m_bindGeometry(gl::Geometry &geometry, const std::vector<float> &verts) {
        static constexpr size_t stride = 5;

        glGenVertexArrays(1, &geometry.vao);
        glBindVertexArray(geometry.vao);
        glGenBuffers(1, &geometry.vbo);
        glBindBuffer(GL_ARRAY_BUFFER, geometry.vbo);
        glBufferData(GL_ARRAY_BUFFER, verts.size()*sizeof(GLfloat), verts.data(), GL_STATIC_DRAW);
        /* Position */
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride*sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);
        /* Texture coordinates */
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride*sizeof(GLfloat), (void*)(3*sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        geometry.vertCount = verts.size() / stride;
    }

    void m_genChunkFloor() {
        _M_SHREKROOMS_DEFINE_WORLD_DATA_CONSTEXPR();

        const size_t meshId = s_meshToId(Mesh::ChunkFloor);

        m_textures[meshId] = m_texman.getTexture(TextureManager::TextureID::Floor);
        gl::Geometry &geometry = m_geometries[meshId];

        std::vector<float> verts {
            // floor
             pmax, -ymax,  pmax,  tfmax, 0.0f,
             pmax, -ymax, -pmax,  tfmax, tfmax,
            -pmax, -ymax,  pmax,  0.0f,  0.0f,
            -pmax, -ymax,  pmax,  0.0f,  0.0f,
             pmax, -ymax, -pmax,  tfmax, tfmax,
            -pmax, -ymax, -pmax,  0.0f,  tfmax,

            // ceiling
             pmax,  ymax, -pmax,  tfmax, tfmax,
             pmax,  ymax,  pmax,  tfmax, 0.0f,
            -pmax,  ymax,  pmax,  0.0f,  0.0f,
             pmax,  ymax, -pmax,  tfmax, tfmax,
            -pmax,  ymax,  pmax,  0.0f,  0.0f,
            -pmax,  ymax, -pmax,  0.0f,  tfmax,
        };

        m_bindGeometry(geometry, verts);
    }

    void m_genChunkWallXPos() {
        _M_SHREKROOMS_DEFINE_WORLD_DATA_CONSTEXPR();

        const size_t meshId = s_meshToId(Mesh::ChunkWallXPos);

        m_textures[meshId] = m_texman.getTexture(TextureManager::TextureID::Wall);
        gl::Geometry &geometry = m_geometries[meshId];

        std::vector<float> verts {
            // main
             wmax, -ymax, -gmax,    0.0f,  twmax,
             wmax, -ymax,  gmax,    twmax, twmax,
             wmax,  ymax,  gmax,    twmax, 0.0f,
             wmax, -ymax, -gmax,    0.0f,  twmax,
             wmax,  ymax,  gmax,    twmax, 0.0f,
             wmax,  ymax, -gmax,    0.0f,  0.0f,

            // sides
             pmax, -ymax, -gmax,    0.0f,  twmax,
             wmax, -ymax, -gmax,    tgmax, twmax,
             wmax,  ymax, -gmax,    tgmax, 0.0f,
             pmax, -ymax, -gmax,    0.0f,  twmax,
             wmax,  ymax, -gmax,    tgmax, 0.0f,
             pmax,  ymax, -gmax,    0.0f,  0.0f,

             wmax, -ymax,  gmax,    0.0f,  twmax,
             pmax, -ymax,  gmax,    tgmax, twmax,
             pmax,  ymax,  gmax,    tgmax, 0.0f,
             wmax, -ymax,  gmax,    0.0f,  twmax,
             pmax,  ymax,  gmax,    tgmax, 0.0f,
             wmax,  ymax,  gmax,    0.0f,  0.0f,
        };

        m_bindGeometry(geometry, verts);
    }

    void m_genChunkWallXNeg() {
        _M_SHREKROOMS_DEFINE_WORLD_DATA_CONSTEXPR();

        const size_t meshId = s_meshToId(Mesh::ChunkWallXNeg);

        m_textures[meshId] = m_texman.getTexture(TextureManager::TextureID::Wall);
        gl::Geometry &geometry = m_geometries[meshId];

        std::vector<float> verts {
            // main
            -wmax, -ymax,  gmax,    0.0f,  twmax,
            -wmax, -ymax, -gmax,    twmax, twmax,
            -wmax,  ymax, -gmax,    twmax, 0.0f,
            -wmax, -ymax,  gmax,    0.0f,  twmax,
            -wmax,  ymax, -gmax,    twmax, 0.0f,
            -wmax,  ymax,  gmax,    0.0f,  0.0f,

            // sides
            -wmax, -ymax, -gmax,    0.0f,  twmax,
            -pmax, -ymax, -gmax,    tgmax, twmax,
            -pmax,  ymax, -gmax,    tgmax, 0.0f,
            -wmax, -ymax, -gmax,    0.0f,  twmax,
            -pmax,  ymax, -gmax,    tgmax, 0.0f,
            -wmax,  ymax, -gmax,    0.0f,  0.0f,

            -pmax, -ymax,  gmax,    0.0f,  twmax,
            -wmax, -ymax,  gmax,    tgmax, twmax,
            -wmax,  ymax,  gmax,    tgmax, 0.0f,
            -pmax, -ymax,  gmax,    0.0f,  twmax,
            -wmax,  ymax,  gmax,    tgmax, 0.0f,
            -pmax,  ymax,  gmax,    0.0f,  0.0f,
        };

        m_bindGeometry(geometry, verts);
    }

    void m_genChunkWallZPos() {
        _M_SHREKROOMS_DEFINE_WORLD_DATA_CONSTEXPR();

        const size_t meshId = s_meshToId(Mesh::ChunkWallZPos);

        m_textures[meshId] = m_texman.getTexture(TextureManager::TextureID::Wall);
        gl::Geometry &geometry = m_geometries[meshId];

        std::vector<float> verts {
            // main
             gmax, -ymax,  wmax,    0.0f,  twmax,
            -gmax, -ymax,  wmax,    twmax, twmax,
            -gmax,  ymax,  wmax,    twmax, 0.0f,
             gmax, -ymax,  wmax,    0.0f,  twmax,
            -gmax,  ymax,  wmax,    twmax, 0.0f,
             gmax,  ymax,  wmax,    0.0f,  0.0f,

            // sides
            -gmax, -ymax,  pmax,    0.0f,  twmax,
            -gmax, -ymax,  gmax,    tgmax, twmax,
            -gmax,  ymax,  gmax,    tgmax, 0.0f,
            -gmax, -ymax,  pmax,    0.0f,  twmax,
            -gmax,  ymax,  gmax,    tgmax, 0.0f,
            -gmax,  ymax,  pmax,    0.0f,  0.0f,

             gmax, -ymax,  gmax,    0.0f,  twmax,
             gmax, -ymax,  pmax,    tgmax, twmax,
             gmax,  ymax,  pmax,    tgmax, 0.0f,
             gmax, -ymax,  gmax,    0.0f,  twmax,
             gmax,  ymax,  pmax,    tgmax, 0.0f,
             gmax,  ymax,  gmax,    0.0f,  0.0f,
        };

        m_bindGeometry(geometry, verts);
    }

    void m_genChunkWallZneg() {
        _M_SHREKROOMS_DEFINE_WORLD_DATA_CONSTEXPR();

        const size_t meshId = s_meshToId(Mesh::ChunkWallZNeg);

        m_textures[meshId] = m_texman.getTexture(TextureManager::TextureID::Wall);
        gl::Geometry &geometry = m_geometries[meshId];

        std::vector<float> verts {
            // main
            -gmax, -ymax, -wmax,    0.0f,  twmax,
             gmax, -ymax, -wmax,    twmax, twmax,
             gmax,  ymax, -wmax,    twmax, 0.0f,
            -gmax, -ymax, -wmax,    0.0f,  twmax,
             gmax,  ymax, -wmax,    twmax, 0.0f,
            -gmax,  ymax, -wmax,    0.0f,  0.0f,

            // sides
             gmax, -ymax, -pmax,    0.0f,  twmax,
             gmax, -ymax, -gmax,    tgmax, twmax,
             gmax,  ymax, -gmax,    tgmax, 0.0f,
             gmax, -ymax, -pmax,    0.0f,  twmax,
             gmax,  ymax, -gmax,    tgmax, 0.0f,
             gmax,  ymax, -pmax,    0.0f,  0.0f,

            -gmax, -ymax, -gmax,    0.0f,  twmax,
            -gmax, -ymax, -pmax,    tgmax, twmax,
            -gmax,  ymax, -pmax,    tgmax, 0.0f,
            -gmax, -ymax, -gmax,    0.0f,  twmax,
            -gmax,  ymax, -pmax,    tgmax, 0.0f,
            -gmax,  ymax, -gmax,    0.0f,  0.0f,
        };

        m_bindGeometry(geometry, verts);
    }

};


} // namespace shrekrooms
