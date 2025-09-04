#pragma once

#include "imports.hpp"
#include "gl.hpp"


namespace shrekrooms {


namespace world_data {
    constexpr float chunkSize = 8.0f;
    constexpr float chunkHeight = 5.0f;
    constexpr int chunkFloorTiles = 2;
    constexpr int chunkWallTiles = 1;
    constexpr float wallPercentage = 0.5f; // in %/100 (e.g. 0.5f == 50%)

    constexpr int chunksCountWidth = 10;
};


class Chunk {
public:
    Chunk(const gl::GLContext &glc, const glm::ivec2 &chunkPos) : 
            m_glc(glc), m_uniman(glc.getUniformManager()), m_chunkPos(chunkPos) {
        if (s_geometry.use_count() != 0)
            return;

        GLuint vao, vbo;

        float pmax = 0.5f * world_data::chunkSize;
        float wmax = pmax * world_data::wallPercentage;
        float ymax = 0.5f * world_data::chunkHeight;
        float tfmax = world_data::chunkFloorTiles;
        float twmax = world_data::chunkWallTiles;

        constexpr size_t stride = 6;
        constexpr size_t floatcount = stride*gl::quadVertCount*(2 + 4);

        float verts[floatcount] = {
            // floor
             pmax, -ymax,  pmax,  tfmax, 0.0f,      0.0f,
             pmax, -ymax, -pmax,  tfmax, tfmax,     0.0f,
            -pmax, -ymax,  pmax,  0.0f,  0.0f,      0.0f,
            -pmax, -ymax,  pmax,  0.0f,  0.0f,      0.0f,
             pmax, -ymax, -pmax,  tfmax, tfmax,     0.0f,
            -pmax, -ymax, -pmax,  0.0f,  tfmax,     0.0f,

            // ceiling
             pmax,  ymax, -pmax,  tfmax, tfmax,     0.0f,
             pmax,  ymax,  pmax,  tfmax, 0.0f,      0.0f,
            -pmax,  ymax,  pmax,  0.0f,  0.0f,      0.0f,
             pmax,  ymax, -pmax,  tfmax, tfmax,     0.0f,
            -pmax,  ymax,  pmax,  0.0f,  0.0f,      0.0f,
            -pmax,  ymax, -pmax,  0.0f,  tfmax,     0.0f,

            // walls
             wmax, -ymax, -wmax,    0.0f,  twmax,   1.0f,
            -wmax, -ymax, -wmax,    twmax, twmax,   1.0f,
            -wmax,  ymax, -wmax,    twmax, 0.0f,    1.0f,
             wmax, -ymax, -wmax,    0.0f,  twmax,   1.0f,
            -wmax,  ymax, -wmax,    twmax, 0.0f,    1.0f,
             wmax,  ymax, -wmax,    0.0f,  0.0f,    1.0f,

             wmax, -ymax,  wmax,    0.0f,  twmax,   1.0f,
             wmax, -ymax, -wmax,    twmax, twmax,   1.0f,
             wmax,  ymax, -wmax,    twmax, 0.0f,    1.0f,
             wmax, -ymax,  wmax,    0.0f,  twmax,   1.0f,
             wmax,  ymax, -wmax,    twmax, 0.0f,    1.0f,
             wmax,  ymax,  wmax,    0.0f,  0.0f,    1.0f,

            -wmax, -ymax,  wmax,    0.0f,  twmax,   1.0f,
             wmax, -ymax,  wmax,    twmax, twmax,   1.0f,
             wmax,  ymax,  wmax,    twmax, 0.0f,    1.0f,
            -wmax, -ymax,  wmax,    0.0f,  twmax,   1.0f,
             wmax,  ymax,  wmax,    twmax, 0.0f,    1.0f,
            -wmax,  ymax,  wmax,    0.0f,  0.0f,    1.0f,

            -wmax, -ymax, -wmax,    0.0f,  twmax,   1.0f,
            -wmax, -ymax,  wmax,    twmax, twmax,   1.0f,
            -wmax,  ymax,  wmax,    twmax, 0.0f,    1.0f,
            -wmax, -ymax, -wmax,    0.0f,  twmax,   1.0f,
            -wmax,  ymax,  wmax,    twmax, 0.0f,    1.0f,
            -wmax,  ymax, -wmax,    0.0f,  0.0f,    1.0f,
        };

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, floatcount*sizeof(GLfloat), verts, GL_STATIC_DRAW);
        // Position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride*sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);
        // Texture coordinates
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride*sizeof(GLfloat), (void*)(3*sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        // Texture id
        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, stride*sizeof(GLfloat), (void*)(5*sizeof(GLfloat)));
        glEnableVertexAttribArray(2);

        s_geometry = std::make_unique<gl::Geometry>(vao, vbo);
    }

    ~Chunk() {
        if (s_geometry.use_count() == 1) {
            glDeleteVertexArrays(1, &s_geometry->vao);
            glDeleteBuffers(1, &s_geometry->vbo);
        }
    }

    void draw() const {
        glm::vec2 offset = { m_chunkPos.x, m_chunkPos.y };
        offset *= world_data::chunkSize /* * 1.05f*/;
        glm::mat4 translate = glm::translate(gl::mat4identity, { offset.x, 0.0f, offset.y });
        m_uniman.setTranslateMatrix(translate);

        glBindVertexArray(s_geometry->vao);
        glDrawArrays(GL_TRIANGLES, 0, gl::quadVertCount*6);
    }

protected:
    const gl::GLContext &m_glc;
    const gl::UniformManager &m_uniman;
    glm::ivec2 m_chunkPos;
    std::shared_ptr<gl::Geometry> s_geometry;
};


class World {
public:
    World(const gl::GLContext &glc, const gl::Texture &floorTex, const gl::Texture &wallTex) :
            m_glc(glc), m_uniman(glc.getUniformManager()), m_floorTex(floorTex), m_wallTex(wallTex) {
        m_chunks.reserve(world_data::chunksCountWidth*world_data::chunksCountWidth);
        for (int x = 0; x < world_data::chunksCountWidth; x++) {
            for (int y = 0; y < world_data::chunksCountWidth; y++) {
                m_chunks.push_back(Chunk { m_glc, { x, y } });
            }
        }
    }

    void draw() const {
        m_glc.enableShader();

        m_uniman.setTexture(0, m_floorTex);
        m_uniman.setTexture(1, m_wallTex);

        for (const Chunk &ch : m_chunks)
            ch.draw();
    }

protected:
    const gl::GLContext &m_glc;
    const gl::UniformManager &m_uniman;
    const gl::Texture &m_floorTex;
    const gl::Texture &m_wallTex;
    std::vector<Chunk> m_chunks;
};


} // namespace shrekrooms
