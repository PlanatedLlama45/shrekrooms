#include "world.hpp"

using namespace shrekrooms;


/*
 * struct shrekrooms::Collision
*/

Collision::Collision() :
    isColliding(false), cancelVector({ 0.0f, 0.0f }) { }

Collision::Collision(bool isColliding, glm::vec2 cancelVector) :
    isColliding(isColliding), cancelVector(cancelVector) { }

void Collision::addOtherCollision(const Collision &coll) {
    if (!coll.isColliding)
        return;
    isColliding = true;
    cancelVector += coll.cancelVector;
}


/*
 * struct shrekrooms::Hitbox
*/

Hitbox::Hitbox() :
    posMin(0.0f), posMax(0.0f) { }

Hitbox::Hitbox(const glm::vec2 &posMin, const glm::vec2 &posMax) :
    posMin(posMin), posMax(posMax) { }

bool Hitbox::isInside(const glm::vec2 &pos) const {
    return (
        posMin.x <= pos.x && pos.x <= posMax.x &&  
        posMin.y <= pos.y && pos.y <= posMax.y
    );
}

Collision Hitbox::getCircleIntersection(const glm::vec2 &pos, float radius) const {
    glm::vec2 closest = glm::clamp(pos, posMin, posMax);
    float dist = glm::distance(pos, closest);
    if (radius < dist)
        return { };
    glm::vec2 dir = glm::normalize(pos - closest);
    dir *= (radius - dist);
    return { true, dir };
}


/*
 * class shrekrooms::Chunk
*/

Chunk::Chunk(const UniformManager &uniman, const MeshManager &meshman, const glm::vec2 &chunkPos, const maze::MazeNode &node) :
        m_uniman(uniman), m_meshman(meshman), m_chunkPos(chunkPos), m_meshes(), m_walls() {
    m_setWalls(node);
    m_addMeshes();

    m_chunkOffset = m_chunkPos * defines::world::chunkSize;
    if (static_cast<int>(m_chunkPos.x + m_chunkPos.y) % 2 == 0)
        m_chunkOffset += glm::vec2 { defines::epsilon, defines::epsilon };

    m_chunkTranslateMat = glm::translate(defines::mat4identity, { m_chunkOffset.x, 0.0f, m_chunkOffset.y });

    m_genHitboxes();
}

void Chunk::draw() const {
    for (MeshManager::Mesh mesh : m_meshes) {
        m_uniman.setTranslateMatrix(m_chunkTranslateMat);
        m_meshman.renderMesh(mesh);
    }
}

bool Chunk::playerCanCollide(const glm::vec2 &pos) const {
    glm::vec2 rel = glm::abs(pos - m_chunkOffset);
    if (std::max(rel.x, rel.y) <= 1.5f * defines::world::chunkSize)
        return true;
    return false;
}

void Chunk::addThisToCollision(Collision &coll, const glm::vec2 &pos, float radius) const {
    for (size_t i = 0; i < s_wallCount; i++) {
        if (m_walls[i])
            coll.addOtherCollision(m_hitboxes[i].getCircleIntersection(pos, radius));
    }
}

Collision Chunk::getCollision(const glm::vec2 &pos, float radius) const {
    Collision res { };
    addThisToCollision(res, pos, radius);
    return res;
}

void Chunk::m_setWalls(const maze::MazeNode &node) {
    m_walls[0] = ((node.walls & maze::Direction::XPos) != maze::Direction::Null);
    m_walls[1] = ((node.walls & maze::Direction::XNeg) != maze::Direction::Null);
    m_walls[2] = ((node.walls & maze::Direction::ZPos) != maze::Direction::Null);
    m_walls[3] = ((node.walls & maze::Direction::ZNeg) != maze::Direction::Null);
}

void Chunk::m_addMeshes() {
    // floor
    m_meshes[0] = MeshManager::Mesh::ChunkFloor;

    // walls
    if (m_walls[0]) m_meshes[1] = MeshManager::Mesh::ChunkWallXPos;
    if (m_walls[1]) m_meshes[2] = MeshManager::Mesh::ChunkWallXNeg;
    if (m_walls[2]) m_meshes[3] = MeshManager::Mesh::ChunkWallZPos;
    if (m_walls[3]) m_meshes[4] = MeshManager::Mesh::ChunkWallZNeg;
}

void Chunk::m_genHitboxes() {
    const float pmax = 0.5f * defines::world::chunkSize;
    const float wmax = pmax - defines::world::wallThicknessHalf;
    const float gmax = pmax + defines::world::wallThicknessHalf - 2.0f*defines::epsilon;

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
        hb.posMin += m_chunkOffset;
        hb.posMax += m_chunkOffset;
    }
}


/*
 * class shrekrooms::World
*/

World::World(const gl::GLContext &glc, const MeshManager &meshman, const maze::Maze &maze) :
        m_glc(glc), m_uniman(glc.getUniformManager()), m_meshman(meshman), m_maze(maze) {
    m_chunks.reserve(defines::world::chunksCountWidth*defines::world::chunksCountWidth);
    for (int x = 0; x < defines::world::chunksCountWidth; x++) {
        for (int y = 0; y < defines::world::chunksCountWidth; y++) {
            m_chunks.emplace_back(m_uniman, meshman, glm::vec2 { static_cast<float>(x), static_cast<float>(y) }, m_maze.getNode({ x, y }));
        }
    }
}

void World::draw() const {
    m_glc.enableShader();

    for (const Chunk &ch : m_chunks)
        ch.draw();
}

Collision World::getCollision(const glm::vec2 &pos, float radius) const {
    Collision res { };
    for (const Chunk &ch : m_chunks) {
        if (ch.playerCanCollide(pos))
            ch.addThisToCollision(res, pos, radius);
    }
    return res;
}
