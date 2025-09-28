#pragma once

#include "imports.hpp"
#include "gl.hpp"
#include "maze.hpp"


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
    constexpr float chunkSize = 5.0f;
    constexpr float chunkHeight = 5.0f;
    constexpr int chunkFloorTiles = 2;
    constexpr int chunkWallTiles = 1;

    constexpr int chunksCountWidth = 10;
};


class Chunk {
public:

    Chunk(const gl::GLContext &glc, const glm::ivec2 &chunkPos, const maze::MazeNode &node) : 
            m_glc(glc), m_uniman(glc.getUniformManager()), m_chunkPos(chunkPos) {
        if (s_geometry.use_count() == 0)
            m_generateGeometry(node);

        glm::vec2 offset = { m_chunkPos.x, m_chunkPos.y };
        offset *= world_data::chunkSize;
        m_chunkTranslateMat = glm::translate(gl::mat4identity, { offset.x, 0.0f, offset.y });

        float wmax = 0.5f * world_data::chunkSize;
        m_hitbox = Hitbox {
            offset - glm::vec2 { wmax },
            offset + glm::vec2 { wmax },
        };
    }

    ~Chunk() {
        if (s_geometry.use_count() == 1) {
            glDeleteVertexArrays(1, &s_geometry->vao);
            glDeleteBuffers(1, &s_geometry->vbo);
        }
    }

    void draw() const {
        m_uniman.setTranslateMatrix(m_chunkTranslateMat);
        glBindVertexArray(s_geometry->vao);
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
    std::shared_ptr<gl::Geometry> s_geometry;
    Hitbox m_hitbox;

    // TODO: make walls appear depending on node.walls value
    void m_generateGeometry(const maze::MazeNode &node) {
        GLuint vao, vbo;

        float pmax = 0.5f * world_data::chunkSize;
        float ymax = 0.5f * world_data::chunkHeight;
        float tfmax = world_data::chunkFloorTiles;
        float twmax = world_data::chunkWallTiles;

        std::vector<float> verts {
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
        };

        // walls
        if ((node.walls & maze::Direction::XPos) != maze::Direction::Null) {
            verts.insert(verts.end(), {
                 pmax, -ymax, -pmax,    0.0f,  twmax,   1.0f,
                 pmax, -ymax,  pmax,    twmax, twmax,   1.0f,
                 pmax,  ymax,  pmax,    twmax, 0.0f,    1.0f,
                 pmax, -ymax, -pmax,    0.0f,  twmax,   1.0f,
                 pmax,  ymax,  pmax,    twmax, 0.0f,    1.0f,
                 pmax,  ymax, -pmax,    0.0f,  0.0f,    1.0f,
            });
        }
        if ((node.walls & maze::Direction::XNeg) != maze::Direction::Null) {
            verts.insert(verts.end(), {
                -pmax, -ymax,  pmax,    0.0f,  twmax,   1.0f,
                -pmax, -ymax, -pmax,    twmax, twmax,   1.0f,
                -pmax,  ymax, -pmax,    twmax, 0.0f,    1.0f,
                -pmax, -ymax,  pmax,    0.0f,  twmax,   1.0f,
                -pmax,  ymax, -pmax,    twmax, 0.0f,    1.0f,
                -pmax,  ymax,  pmax,    0.0f,  0.0f,    1.0f,
            });
        }
        if ((node.walls & maze::Direction::ZPos) != maze::Direction::Null) {
            verts.insert(verts.end(), {
                 pmax, -ymax,  pmax,    0.0f,  twmax,   1.0f,
                -pmax, -ymax,  pmax,    twmax, twmax,   1.0f,
                -pmax,  ymax,  pmax,    twmax, 0.0f,    1.0f,
                 pmax, -ymax,  pmax,    0.0f,  twmax,   1.0f,
                -pmax,  ymax,  pmax,    twmax, 0.0f,    1.0f,
                 pmax,  ymax,  pmax,    0.0f,  0.0f,    1.0f,
            });
        }
        if ((node.walls & maze::Direction::ZNeg) != maze::Direction::Null) {
            verts.insert(verts.end(), {
                -pmax, -ymax, -pmax,    0.0f,  twmax,   1.0f,
                 pmax, -ymax, -pmax,    twmax, twmax,   1.0f,
                 pmax,  ymax, -pmax,    twmax, 0.0f,    1.0f,
                -pmax, -ymax, -pmax,    0.0f,  twmax,   1.0f,
                 pmax,  ymax, -pmax,    twmax, 0.0f,    1.0f,
                -pmax,  ymax, -pmax,    0.0f,  0.0f,    1.0f,
            });
        }

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        constexpr size_t stride = 6;

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, verts.size()*sizeof(GLfloat), verts.data(), GL_STATIC_DRAW);
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
};


class World {
public:
    World(const gl::GLContext &glc, const maze::Maze &maze, const gl::Texture &floorTex, const gl::Texture &wallTex) :
            m_glc(glc), m_uniman(glc.getUniformManager()), m_maze(maze), m_floorTex(floorTex), m_wallTex(wallTex) {
        m_chunks.reserve(world_data::chunksCountWidth*world_data::chunksCountWidth);
        for (int x = 0; x < world_data::chunksCountWidth; x++) {
            for (int y = 0; y < world_data::chunksCountWidth; y++) {
                m_chunks.push_back(Chunk { m_glc, { x, y }, m_maze.getNode({ x, y }) });
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
    std::pair<bool, glm::vec2> getPlayerIntersection(const glm::vec2 &pos, float radius) const {
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
    const maze::Maze &m_maze;
    const gl::Texture &m_floorTex;
    const gl::Texture &m_wallTex;
    std::vector<Chunk> m_chunks;
};


} // namespace shrekrooms
