#pragma once

#include "imports.hpp"
#include "gl.hpp"


namespace shrekrooms {


struct Hitbox {
    Hitbox() :
        posMin(0.0f), posMax(0.0f) { }
    
    Hitbox(const glm::vec2 &posMin, const glm::vec2 &posMax) :
        posMin(posMin), posMax(posMax) { }

    bool isInside(const glm::vec2 &pos) const {
        return (
            posMin.x <= pos.x && pos.x <= posMax.x &&  
            posMin.y <= pos.y && pos.y <= posMax.y
        );
    }

    // pair { bool, vec2 delta or {0,0} if no intersection }
    std::pair<bool, glm::vec2> getCircleIntersection(const glm::vec2 &pos, float radius) const {
        glm::vec2 closest = glm::clamp(pos, posMin, posMax);
        float dist = glm::distance(pos, closest);
        if (radius < dist)
            return { false, { 0.0f, 0.0f } };
        glm::vec2 dir = glm::normalize(pos - closest);
        dir *= (radius - dist);
        return { true, dir };
    }

    glm::vec2 posMin, posMax;
};


namespace world_data {
    constexpr float chunkSize = 8.0f;
    constexpr float chunkHeight = 5.0f;
    constexpr int chunkFloorTiles = 3;
    constexpr int chunkWallTiles = 1;
    constexpr float wallPercentage = 0.5f; // in %/100 (e.g. 0.5f == 50%)

    constexpr int chunksCountWidth = 10;
};


class Chunk {
public:
    Chunk(const gl::GLContext &glc, const glm::ivec2 &chunkPos) : 
            m_glc(glc), m_uniman(glc.getUniformManager()), m_chunkPos(chunkPos) {
        if (m_geometry.use_count() == 0)
            m_generateGeometry();

        glm::vec2 offset = { m_chunkPos.x, m_chunkPos.y };
        offset *= world_data::chunkSize;
        m_chunkTranslateMat = glm::translate(gl::mat4identity, { offset.x, 0.0f, offset.y });

        float wmax = 0.5f * world_data::chunkSize * world_data::wallPercentage;
        m_hitbox = Hitbox {
            offset - glm::vec2 { wmax },
            offset + glm::vec2 { wmax },
        };
    }

    ~Chunk() {
        if (m_geometry.use_count() == 1) {
            glDeleteVertexArrays(1, &m_geometry->vao);
            glDeleteBuffers(1, &m_geometry->vbo);
        }
    }

    void draw() const {
        m_uniman.setTranslateMatrix(m_chunkTranslateMat);
        glBindVertexArray(m_geometry->vao);
        glDrawArrays(GL_TRIANGLES, 0, gl::quadVertCount*6);
    }

    const Hitbox &getHitbox() const {
        return m_hitbox;
    }

protected:
    const gl::GLContext &m_glc;
    const gl::UniformManager &m_uniman;
    glm::ivec2 m_chunkPos;
    glm::mat4 m_chunkTranslateMat;
    std::shared_ptr<gl::Geometry> m_geometry;
    Hitbox m_hitbox;

    void m_generateGeometry() {
        GLuint vao, vbo;

        constexpr float pmax = 0.5f * world_data::chunkSize;
        constexpr float wmax = pmax * world_data::wallPercentage;
        constexpr float ymax = 0.5f * world_data::chunkHeight;
        constexpr float tfmax = world_data::chunkFloorTiles;
        constexpr float twmax = world_data::chunkWallTiles;

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
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride*sizeof(GLfloat), (void*)0);
        // Texture coordinates
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride*sizeof(GLfloat), (void*)(3*sizeof(GLfloat)));
        // Texture id
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, stride*sizeof(GLfloat), (void*)(5*sizeof(GLfloat)));

        m_geometry = std::make_unique<gl::Geometry>(vao, vbo);
    }
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
    
    // pair { bool, vec2 delta or {0,0} if no intersection }
    std::pair<bool, glm::vec2> getCircleIntersection(const glm::vec2 &pos, float radius) const {
        std::pair<bool, glm::vec2> res { false, { 0.0f, 0.0f } };
        for (const Chunk &ch : m_chunks) {
            auto inter = ch.getHitbox().getCircleIntersection(pos, radius);
            if (inter.first) {
                res.first = true;
                res.second += inter.second;
            }
        }
        return res;
    }


protected:
    const gl::GLContext &m_glc;
    const gl::UniformManager &m_uniman;
    const gl::Texture &m_floorTex;
    const gl::Texture &m_wallTex;
    std::vector<Chunk> m_chunks;
};


} // namespace shrekrooms
