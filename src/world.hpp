#pragma once

#include "imports.hpp"
#include "gl.hpp"


namespace shrekrooms {


namespace world_data {
    constexpr float chunkSize = 4.0f;
    constexpr float chunkHeight = 4.0f;
    constexpr int chunkFloorTiles = 2;

    const int chunksCountSize = 4;
};


class Chunk {
public:
    Chunk(const gl::GLContext &glc, const glm::ivec2 &chunkPos, const gl::Texture &tex) : 
            m_glc(glc), m_uniman(glc.getUniformManager()), m_chunkPos(chunkPos), m_tex(tex) {
        if (m_geometry.use_count() != 0)
            return;

        GLuint vao, vbo;

        float pmax = 0.5f * world_data::chunkSize;
        float ymax = 0.5f * world_data::chunkHeight;
        float tmax = world_data::chunkFloorTiles;

        float verts[30] = {
             pmax, -ymax,  pmax,  tmax, 0.0f,
             pmax, -ymax, -pmax,  tmax, tmax,
            -pmax, -ymax,  pmax,  0.0f, 0.0f,

            -pmax, -ymax,  pmax,  0.0f, 0.0f,
             pmax, -ymax, -pmax,  tmax, tmax,
            -pmax, -ymax, -pmax,  0.0f, tmax
        };

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, 30*sizeof(GLfloat), verts, GL_STATIC_DRAW);
        // Position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);
        // Texture coordinates
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (void*)(3*sizeof(GLfloat)));
        glEnableVertexAttribArray(1);

        m_geometry = std::make_unique<gl::Geometry>(vao, vbo);
    }

    ~Chunk() {
        if (m_geometry.use_count() == 1) {
            std::cout << "Destroy chunk geometry\n";
            glDeleteVertexArrays(1, &m_geometry->vao);
            glDeleteBuffers(1, &m_geometry->vbo);
        }
    }

    void draw() const {
        glm::vec2 offset = { m_chunkPos.x, m_chunkPos.y };
        offset *= world_data::chunkSize * 1.05f;
        glm::mat4 translate = glm::translate(gl::mat4identity, { offset.x, 0.0f, offset.y });
        m_uniman.setTranslateMatrix(translate);

        m_uniman.setTexture(m_tex);
        glBindVertexArray(m_geometry->vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

protected:
    const gl::GLContext &m_glc;
    const gl::UniformManager &m_uniman;
    const gl::Texture &m_tex;
    glm::ivec2 m_chunkPos;
    std::shared_ptr<gl::Geometry> m_geometry;
};


class World {
public:
    World(const gl::GLContext &glc, const gl::Texture &tex) :
            m_glc(glc), m_tex(tex) {
        m_chunks.reserve(world_data::chunksCountSize*world_data::chunksCountSize);
        for (int x = 0; x < world_data::chunksCountSize; x++) {
            for (int y = 0; y < world_data::chunksCountSize; y++) {
                m_chunks.push_back(Chunk { m_glc, { x, y }, m_tex });
            }
        }
    }

    void draw() const {
        m_glc.enableShader();
        for (const Chunk &ch : m_chunks)
            ch.draw();
    }

protected:
    const gl::GLContext &m_glc;
    const gl::Texture &m_tex;
    std::vector<Chunk> m_chunks;
};


} // namespace shrekrooms
