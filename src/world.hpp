#pragma once

#include "imports.hpp"
#include "gl.hpp"
#include "maze.hpp"


namespace shrekrooms {


struct Collision {
    bool isColliding;
    glm::vec2 cancelVector;

    Collision() :
        isColliding(false), cancelVector({ 0.0f, 0.0f }) { }

    Collision(bool isColliding, glm::vec2 cancelVector) :
        isColliding(isColliding), cancelVector(cancelVector) { }

    void addOtherCollision(const Collision &coll) {
        if (!coll.isColliding)
            return;
        isColliding = true;
        cancelVector += coll.cancelVector;
    }

};


struct Hitbox {
    glm::vec2 posMin, posMax;

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

    Collision getCircleIntersection(const glm::vec2 &pos, float radius) const {
        glm::vec2 closest = glm::clamp(pos, posMin, posMax);
        float dist = glm::distance(pos, closest);
        if (radius < dist)
            return { };
        glm::vec2 dir = glm::normalize(pos - closest);
        dir *= (radius - dist);
        return { true, dir };
    }
};


namespace world_data {
    constexpr float chunkSize = 5.0f;
    constexpr float chunkHeight = 5.0f;
    constexpr float wallThicknessHalf = 0.5f;
    constexpr int chunkFloorTiles = 2;
    constexpr int chunkWallTiles = 1;

    constexpr int chunksCountWidth = 10;
    constexpr float bridgePercentage = 0.2f;
};


#define _M_SHREKROOMS_WORLD_DEFINE_WORLD_DATA_CONSTEXPR()                                           \
    constexpr float pmax = 0.5f * world_data::chunkSize;                                            \
    constexpr float ymax = 0.5f * world_data::chunkHeight;                                          \
    constexpr float wmax = pmax - world_data::wallThicknessHalf;                                    \
    constexpr float gmax = pmax + world_data::wallThicknessHalf - 2.0f*gl::epsilon;                 \
    constexpr float tfmax = world_data::chunkFloorTiles;                                            \
    constexpr float twmax = world_data::chunkWallTiles;                                             \
    constexpr float tgmax = twmax * (2.0f * world_data::wallThicknessHalf / world_data::chunkSize);


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

    MeshManager(const gl::UniformManager &uniman, const gl::Texture &floorTex, const gl::Texture &wallTex) :
            m_uniman(uniman), m_textures() {
        m_textures = { floorTex, wallTex, wallTex, wallTex, wallTex };

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

        const gl::Geometry &geo = m_geometries[meshId];
        const gl::Texture &tex = m_textures[meshId]->get();

        m_uniman.setTexture(tex);
        glBindVertexArray(geo.vao);
        glDrawArrays(GL_TRIANGLES, 0, geo.vertCount);
    }

protected:
    static constexpr size_t s_meshCount = 5;
    std::array<gl::Geometry, s_meshCount> m_geometries;
    std::array<std::optional<std::reference_wrapper<const gl::Texture>>, s_meshCount> m_textures;

    static constexpr size_t s_meshToId(Mesh mesh) {
        return static_cast<size_t>(mesh) - 1;
    }

    const gl::UniformManager &m_uniman;

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
        _M_SHREKROOMS_WORLD_DEFINE_WORLD_DATA_CONSTEXPR();

        gl::Geometry &geometry = m_geometries[s_meshToId(Mesh::ChunkFloor)];

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
        _M_SHREKROOMS_WORLD_DEFINE_WORLD_DATA_CONSTEXPR();

        gl::Geometry &geometry = m_geometries[s_meshToId(Mesh::ChunkWallXPos)];

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
        _M_SHREKROOMS_WORLD_DEFINE_WORLD_DATA_CONSTEXPR();

        gl::Geometry &geometry = m_geometries[s_meshToId(Mesh::ChunkWallXNeg)];

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
        _M_SHREKROOMS_WORLD_DEFINE_WORLD_DATA_CONSTEXPR();

        gl::Geometry &geometry = m_geometries[s_meshToId(Mesh::ChunkWallZPos)];

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
        _M_SHREKROOMS_WORLD_DEFINE_WORLD_DATA_CONSTEXPR();

        gl::Geometry &geometry = m_geometries[s_meshToId(Mesh::ChunkWallZNeg)];

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


class Chunk {
public:
    Chunk(const gl::UniformManager &uniman, const MeshManager &meshman, const glm::vec2 &chunkPos, const maze::MazeNode &node) :
            m_uniman(uniman), m_meshman(meshman), m_chunkPos(chunkPos), m_meshes(), m_walls() {
        m_setWalls(node);
        m_addMeshes();

        glm::vec2 offset = m_chunkPos * world_data::chunkSize;
        if (static_cast<int>(m_chunkPos.x + m_chunkPos.y) % 2 == 0)
            offset += glm::vec2 { gl::epsilon, gl::epsilon };

        m_chunkTranslateMat = glm::translate(gl::mat4identity, { offset.x, 0.0f, offset.y });

        m_genHitboxes(offset);
    }

    void draw() const {
        for (MeshManager::Mesh mesh : m_meshes) {
            m_uniman.setTranslateMatrix(m_chunkTranslateMat);
            m_meshman.renderMesh(mesh);
        }
    }

    // TODO: false if player far enough
    bool playerCanCollide(const glm::vec2 &pos) const {
        return true;
    }

    void addThisToCollision(Collision &coll, const glm::vec2 &pos, float radius) const {
        bool hadColl = coll.isColliding;
        for (size_t i = 0; i < s_wallCount; i++) {
            if (m_walls[i])
                coll.addOtherCollision(m_hitboxes[i].getCircleIntersection(pos, radius));
        }
        if (!hadColl && coll.isColliding)
            std::cout << "Collision occured; chunk pos: " << m_chunkPos.x << ", " << m_chunkPos.y << '\n';
    }

    Collision getCollision(const glm::vec2 &pos, float radius) const {
        Collision res { };
        addThisToCollision(res, pos, radius);
        return res;
    }

protected:
    static constexpr size_t s_wallCount = 4;

    const gl::UniformManager &m_uniman;
    const MeshManager &m_meshman;
    glm::vec2 m_chunkPos;
    glm::mat4 m_chunkTranslateMat;
    std::array<MeshManager::Mesh, (s_wallCount + 1)> m_meshes;
    /*
     * 0: x+
     * 1: x-
     * 2: z+
     * 3: z-
    */
    std::array<bool, s_wallCount> m_walls;
    std::array<Hitbox, s_wallCount> m_hitboxes;

    void m_setWalls(const maze::MazeNode &node) {
        m_walls[0] = ((node.walls & maze::Direction::XPos) != maze::Direction::Null);
        m_walls[1] = ((node.walls & maze::Direction::XNeg) != maze::Direction::Null);
        m_walls[2] = ((node.walls & maze::Direction::ZPos) != maze::Direction::Null);
        m_walls[3] = ((node.walls & maze::Direction::ZNeg) != maze::Direction::Null);
    }

    void m_addMeshes() {
        // floor
        m_meshes[0] = MeshManager::Mesh::ChunkFloor;

        // walls
        if (m_walls[0]) m_meshes[1] = MeshManager::Mesh::ChunkWallXPos;
        if (m_walls[1]) m_meshes[2] = MeshManager::Mesh::ChunkWallXNeg;
        if (m_walls[2]) m_meshes[3] = MeshManager::Mesh::ChunkWallZPos;
        if (m_walls[3]) m_meshes[4] = MeshManager::Mesh::ChunkWallZNeg;
    }

    void m_genHitboxes(const glm::vec2 &offset) {
        _M_SHREKROOMS_WORLD_DEFINE_WORLD_DATA_CONSTEXPR();

        // x+
        m_hitboxes[0] = Hitbox {
            {  wmax, -gmax },
            {  pmax,  gmax }
        };
        // x-
        m_hitboxes[1] = Hitbox {
            { -pmax, -gmax },
            { -wmax,  gmax }
        };
        // z+
        m_hitboxes[2] = Hitbox {
            { -gmax,  wmax },
            {  gmax,  gmax }
        };
        // z-
        m_hitboxes[3] = Hitbox {
            { -gmax, -gmax },
            {  gmax, -wmax }
        };

        for (Hitbox &hb : m_hitboxes) {
            hb.posMin += offset;
            hb.posMax += offset;
        }
    }

};


class World {
public:
    World(const gl::GLContext &glc, const MeshManager &meshman, const maze::Maze &maze) :
            m_glc(glc), m_uniman(glc.getUniformManager()), m_meshman(meshman), m_maze(maze) {
        m_chunks.reserve(world_data::chunksCountWidth*world_data::chunksCountWidth);
        for (int x = 0; x < world_data::chunksCountWidth; x++) {
            for (int y = 0; y < world_data::chunksCountWidth; y++) {
                m_chunks.emplace_back(m_uniman, meshman, glm::vec2 { static_cast<float>(x), static_cast<float>(y) }, m_maze.getNode({ x, y }));
            }
        }
    }

    void draw() const {
        m_glc.enableShader();

        for (const Chunk &ch : m_chunks)
            ch.draw();
    }
    
    Collision getPlayerCollision(const glm::vec2 &pos, float radius) const {
        Collision res { };
        for (const Chunk &ch : m_chunks) {
            if (ch.playerCanCollide(pos))
                ch.addThisToCollision(res, pos, radius);
        }
        return res;
    }


protected:
    const gl::GLContext &m_glc;
    const gl::UniformManager &m_uniman;
    const MeshManager &m_meshman;
    const maze::Maze &m_maze;
    std::vector<Chunk> m_chunks;

};


} // namespace shrekrooms
