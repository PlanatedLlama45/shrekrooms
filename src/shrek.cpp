#include "shrek.hpp"

using namespace shrekrooms;


Shrek::Shrek(const gl::GLContext &glc, const maze::Maze &maze, const glm::ivec2 &mazePos) : 
        m_glc(glc), m_meshman(glc.getMeshManager()), m_uniman(glc.getUniformManager()), m_maze(maze), m_mazePos(mazePos) {
    glm::vec2 tmp = defines::world::chunkSize * static_cast<glm::vec2>(m_mazePos);
    m_pos = { tmp.x, 0.0f, tmp.y };
    m_getShortestPath({ 0, 0 });
}

void Shrek::update(const World &world, const Player &player, float dt) {
    static glm::vec3 nextPos = m_pos;

    if (m_mazePos == worldToChunkCoords(player.getPos()))
        nextPos = player.getPos();
    else if (glm::distance(m_pos, nextPos) < 0.1f) {
        if (m_targetPath.empty())
            m_getShortestPath(worldToChunkCoords(player.getPos()));

        m_mazePos = m_targetPath.back();
        glm::vec2 tmp = defines::world::chunkSize * static_cast<glm::vec2>(m_targetPath.back());
        m_targetPath.pop_back();
        nextPos = { tmp.x, 0.0f, tmp.y };
    }

    glm::vec3 dir = glm::normalize(nextPos - m_pos);
    m_pos += defines::shrek::walkSpeed * dt * dir;
}

void Shrek::draw(const Player &player) const {
    glm::vec3 playerPos = player.getPos();

    // Position based
    float angle = glm::acos(glm::dot(glm::normalize(m_pos - playerPos), { 1.0f, 0.0f, 0.0f }));
    if (m_pos.z - playerPos.z < 0) angle = -angle;
    glm::mat4 rotMat = glm::rotate(defines::mat4identity, -angle, defines::globalUp);
    m_uniman.setRotateMatrix(rotMat);

    // View direction based
    // glm::mat4 rotMat = glm::rotate(defines::mat4identity, -player.getCameraRot(), defines::globalUp);

    // m_uniman.setRotateMatrix(rotMat);

    glm::mat4 transMat = glm::translate(defines::mat4identity, m_pos);

    m_uniman.setTranslateMatrix(transMat);
    m_meshman.renderMesh(s_mesh);
    m_uniman.setRotateMatrix(defines::mat4identity);
}

bool Shrek::isCollidingPlayer(const Player &player) const {
    return glm::distance(player.getPos(), m_pos) < (defines::shrek::radius + defines::player::radius);
}

void shrekrooms::Shrek::m_getShortestPath(const glm::ivec2 &target) {
    using namespace maze;
    using NodeT = glm::ivec2;
    static const NodeT undefinedNode { -1, -1 };

    Grid<size_t> dist { defines::world::chunksCountWidth, defines::world::chunksCountWidth, SIZE_MAX };
    dist[m_mazePos] = 0;

    Grid<NodeT> prev { defines::world::chunksCountWidth, defines::world::chunksCountWidth, undefinedNode };

    std::deque<NodeT> que;
    for (int x = 0; x < defines::world::chunksCountWidth; x++) {
        for (int y = 0; y < defines::world::chunksCountWidth; y++)
            que.emplace_back(x, y);
    }

    NodeT u, v;
    while (!que.empty()) {
        size_t minDist = SIZE_MAX;
        for (NodeT n : que) {
            if (dist[n] < minDist) {
                minDist = dist[n];
                u = n;
            }
        }
        if (u == target)
            break;
        que.erase(std::find(que.begin(), que.end(), u));

        for (Direction dir : allDirections) {
            if (m_maze.getNode(u).hasWall(dir))
                continue;

            v = u + getDirectionVector(dir);
            size_t alt = dist[u] + 1;
            if (alt < dist[v]) {
                dist[v] = alt;
                prev[v] = u;
            }
        }
    }

    m_targetPath.clear();
    u = target;
    if (prev[u] != undefinedNode || u == m_mazePos) {
        while (u != undefinedNode) {
            m_targetPath.push_back(u);
            u = prev[u];
        }
    }
    if (m_targetPath.size() != 1)
        m_targetPath.pop_back();
}
