#pragma once

#include "defines.hpp"
#include "glc.hpp"
#include "world.hpp"


namespace shrekrooms {


class Player {
public:
    Player(const gl::GLContext &glc, const glm::vec3 &pos, float cameraRot);

    const glm::vec3 &getPos() const;
    float getCameraRot() const;

    void update(const World &world, float dt);

protected:
    const gl::GLContext &m_glc;
    const UniformManager &m_uniman;
    glm::vec3 m_pos;
    float m_cameraRot;

};


} // namespace shrekrooms
