#include "shrek.hpp"

using namespace shrekrooms;


Shrek::Shrek(const gl::GLContext &glc, const maze::Maze &maze, const Player &player, const glm::ivec2 &mazePos) : 
        m_glc(glc), m_meshman(glc.getMeshManager()), m_uniman(glc.getUniformManager()), m_maze(maze), m_player(player),
        m_mazePos(mazePos), m_pathFindThread(m_pathFind, std::ref(*this)) {
    glm::vec2 tmp = defines::world::chunkSize * static_cast<glm::vec2>(m_mazePos);
    m_pos = { tmp.x, 0.0f, tmp.y };
}

shrekrooms::Shrek::~Shrek() {
    m_pathFindThread.detach();
}

void Shrek::update(const World &world, float dt) {
    static glm::vec3 nextPos = m_pos;
    bool playerFollow = false;

    if (m_mazePos == worldToChunkCoords(m_player.getPos()))
        playerFollow = true;
    else if (!m_pathExhausted || glm::distance(m_pos, nextPos) < 0.1f) {
        if (m_targetPath.empty()) {
            m_pathExhausted = true;
            std::cout << "\nPath exhausted\n";
            return;
        }

        // TODO: Fix shrek passing through walls????
        m_mazePos = m_targetPath.back();
        glm::vec2 tmp = defines::world::chunkSize * static_cast<glm::vec2>(m_targetPath.back());
        m_targetPath.pop_back();
        nextPos = { tmp.x, 0.0f, tmp.y };
    }

    glm::vec3 dir = glm::normalize((playerFollow ? m_player.getPos() : nextPos) - m_pos);
    m_pos += defines::shrek::walkSpeed * dt * dir;

    if (playerFollow) {
        Collision coll = world.getCollision({ m_pos.x, m_pos.z }, defines::shrek::radius);
        if (coll.isColliding)
            m_pos += glm::vec3 { coll.cancelVector.x, 0.0f, coll.cancelVector.y };
    }
}

void Shrek::draw() const {
    glm::vec3 playerPos = m_player.getPos();

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

bool Shrek::isCollidingPlayer() const {
    return glm::distance(m_player.getPos(), m_pos) < (defines::shrek::radius + defines::player::radius);
}

void shrekrooms::Shrek::m_pathFind() {
    while (true) {
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            std::cout << "\nBegin pathfind\n";
            m_targetPath = m_getShortestPath(m_mazePos, worldToChunkCoords(m_player.getPos()));
            m_pathExhausted = false;
            std::cout << "End pathfind\n";
            m_condVar.wait_for(lock, defines::shrek::pathFindPeriod, [&]{ return m_pathExhausted; });
        }
    }
}

std::deque<glm::ivec2> shrekrooms::Shrek::m_getShortestPath(const glm::ivec2 &start, const glm::ivec2 &end) const {
    using namespace maze;
    using NodeT = glm::ivec2;
    static const NodeT undefinedNode { -1, -1 };

    Grid<size_t> dist { defines::world::chunksCountWidth, defines::world::chunksCountWidth, SIZE_MAX };
    dist[start] = 0;

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
        if (u == end)
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

    std::deque<glm::ivec2> path;
    u = end;
    if (prev[u] != undefinedNode || u == start) {
        while (u != undefinedNode) {
            path.push_back(u);
            u = prev[u];
        }
    }
    if (path.size() != 1)
        path.pop_back();

    return path;
}
