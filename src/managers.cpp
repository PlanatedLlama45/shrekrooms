#include "managers.hpp"

using namespace shrekrooms;


/*
 * class shrekrooms::UniformManager
*/

UniformManager::UniformManager(GLuint shader) :
        m_shader(shader) {
    m_uniforms[s_uniformToId(Uniform::Translate)]  = m_getUniformLocation("u_translate");
    m_uniforms[s_uniformToId(Uniform::Rotate)]     = m_getUniformLocation("u_rotate");
    m_uniforms[s_uniformToId(Uniform::View)]       = m_getUniformLocation("u_view");
    m_uniforms[s_uniformToId(Uniform::Projection)] = m_getUniformLocation("u_projection");
    m_uniforms[s_uniformToId(Uniform::Color)]      = m_getUniformLocation("u_color");
    m_uniforms[s_uniformToId(Uniform::ViewPos)]    = m_getUniformLocation("u_viewPos");
    m_uniforms[s_uniformToId(Uniform::ViewDir)]    = m_getUniformLocation("u_viewDir");
    m_uniforms[s_uniformToId(Uniform::FogColor)]   = m_getUniformLocation("u_fogColor");
    m_uniforms[s_uniformToId(Uniform::UsePBR)]     = m_getUniformLocation("u_usePBR");
}

// Uniforms
void shrekrooms::UniformManager::useMaterial(const gl::Material &mat) const {
    if (mat.isPBR()) {
        glUniform1i(m_uniforms[s_uniformToId(Uniform::UsePBR)], 1); // Set u_usePBR
        // Bind albedo texture
        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, mat.getAlbedoTexture());
        // Bind normal texture
        glActiveTexture(GL_TEXTURE0 + 1);
        glBindTexture(GL_TEXTURE_2D, mat.getNormalTexture());
    } else {
        glUniform1i(m_uniforms[s_uniformToId(Uniform::UsePBR)], 0); // Unset u_usePBR
        // Bind texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mat.getTexture());
    }
}

void UniformManager::setTranslateMatrix(const glm::mat4 &translateMat) const {
    glUniformMatrix4fv(m_uniforms[s_uniformToId(Uniform::Translate)], 1, GL_FALSE, glm::value_ptr(translateMat));
}

void UniformManager::setRotateMatrix(const glm::mat4 &rotateMat) const {
    glUniformMatrix4fv(m_uniforms[s_uniformToId(Uniform::Rotate)], 1, GL_FALSE, glm::value_ptr(rotateMat));
}

void UniformManager::UniformManager::setViewMatrix(const glm::mat4 &viewMat) const {
    glUniformMatrix4fv(m_uniforms[s_uniformToId(Uniform::View)], 1, GL_FALSE, glm::value_ptr(viewMat));
}

void UniformManager::setProjectionMatrix(const glm::mat4 &projectionMat) const {
    glUniformMatrix4fv(m_uniforms[s_uniformToId(Uniform::Projection)], 1, GL_FALSE, glm::value_ptr(projectionMat));
}

void UniformManager::setColor(const gl::Color &color) const {
    glUniform4fv(m_uniforms[s_uniformToId(Uniform::Color)], 1, glm::value_ptr(static_cast<glm::vec4>(color)));
}

void UniformManager::setFogColor(const gl::Color &color) const {
    glUniform4fv(m_uniforms[s_uniformToId(Uniform::FogColor)], 1, glm::value_ptr(static_cast<glm::vec4>(color)));
}

void UniformManager::setViewPos(const glm::vec3 &pos) const {
    glUniform3fv(m_uniforms[s_uniformToId(Uniform::ViewPos)], 1, glm::value_ptr(pos));
}

void UniformManager::setViewDir(const glm::vec3 &dir) const {
    glUniform3fv(m_uniforms[s_uniformToId(Uniform::ViewDir)], 1, glm::value_ptr(dir));
}

constexpr size_t UniformManager::s_uniformToId(Uniform uniform) {
    return static_cast<size_t>(uniform) - 1;
}

GLuint UniformManager::m_getUniformLocation(const std::string &name) const {
    return glGetUniformLocation(m_shader, name.c_str());
}


/*
 * class shrekrooms::MaterialManager
*/

MaterialManager::MaterialManager(const UniformManager &uniman) :
        m_uniman(uniman), m_materials() {
    m_materials[s_materialToId(MaterialID::Floor)] = std::make_unique<gl::Material>("floor");
    m_materials[s_materialToId(MaterialID::Wall)]  = std::make_unique<gl::Material>("wall");
    m_materials[s_materialToId(MaterialID::Shrek)] = std::make_unique<gl::Material>("shrek.jpg", false);
}

const gl::Material &MaterialManager::getMaterial(MaterialID material) const {
    return *m_materials[s_materialToId(material)];
}

constexpr size_t MaterialManager::s_materialToId(MaterialID material) {
    return static_cast<size_t>(material) - 1;
}


/*
 * class shrekrooms::MeshManager
*/

MeshManager::MeshManager(const UniformManager &uniman, const MaterialManager &matman) :
        m_uniman(uniman), m_matman(matman), m_materials() {
    m_genChunkFloor();
    m_genChunkWallXPos();
    m_genChunkWallXNeg();
    m_genChunkWallZPos();
    m_genChunkWallZneg();
    m_genShrek();
}

void MeshManager::renderMesh(Mesh mesh) const {
    if (mesh == Mesh::Null)
        return;

    const size_t meshId = s_meshToId(mesh);

    m_uniman.useMaterial(m_matman.getMaterial(m_materials[meshId]));

    const gl::Geometry &geo = m_geometries[meshId];
    glBindVertexArray(geo.vao);
    glDrawArrays(GL_TRIANGLES, 0, geo.vertCount);
}

constexpr size_t MeshManager::s_meshToId(Mesh mesh) {
    return static_cast<size_t>(mesh) - 1;
}

void MeshManager::m_bindGeometry(gl::Geometry &geometry, const std::vector<float> &verts) {
    static constexpr size_t stride = 3 + 2 + 3 + 3;
    geometry.vertCount = verts.size() / stride;

    glGenVertexArrays(1, &geometry.vao);
    glBindVertexArray(geometry.vao);
    glGenBuffers(1, &geometry.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, geometry.vbo);
    glBufferData(GL_ARRAY_BUFFER, verts.size()*sizeof(GLfloat), verts.data(), GL_STATIC_DRAW);
    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride*sizeof(GLfloat), reinterpret_cast<void *>(0));
    glEnableVertexAttribArray(0);
    // Texture coordinates
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride*sizeof(GLfloat), reinterpret_cast<void *>(3*sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    // Normal
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride*sizeof(GLfloat), reinterpret_cast<void *>(5*sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    // Tangent
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride*sizeof(GLfloat), reinterpret_cast<void *>(8*sizeof(GLfloat)));
    glEnableVertexAttribArray(3);
}

#define _M_SHREKROOMS_DEFINE_WORLD_DATA_CONSTEXPR()                                                 \
    const float pmax = 0.5f * defines::world::chunkSize;                                            \
    const float ymax = 0.5f * defines::world::chunkHeight;                                          \
    const float wmax = pmax - defines::world::wallThicknessHalf;                                    \
    const float gmax = pmax + defines::world::wallThicknessHalf - 2.0f*defines::epsilon;            \
    const float tfmax = defines::world::chunkFloorTiles;                                            \
    const float twmax = defines::world::chunkWallTiles;                                             \
    const float tgmax = twmax * (2.0f * defines::world::wallThicknessHalf / defines::world::chunkSize);

void MeshManager::m_genChunkFloor() {
    _M_SHREKROOMS_DEFINE_WORLD_DATA_CONSTEXPR();

    const size_t meshId = s_meshToId(Mesh::ChunkFloor);

    m_materials[meshId] = MaterialManager::MaterialID::Floor;
    gl::Geometry &geometry = m_geometries[meshId];

    std::vector<float> verts {
        // floor
         pmax, -ymax,  pmax,    tfmax, 0.0f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,  0.0f,
         pmax, -ymax, -pmax,    tfmax, tfmax,    0.0f,  1.0f,  0.0f,     1.0f,  0.0f,  0.0f,
        -pmax, -ymax,  pmax,    0.0f,  0.0f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,  0.0f,
        -pmax, -ymax,  pmax,    0.0f,  0.0f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,  0.0f,
         pmax, -ymax, -pmax,    tfmax, tfmax,    0.0f,  1.0f,  0.0f,     1.0f,  0.0f,  0.0f,
        -pmax, -ymax, -pmax,    0.0f,  tfmax,    0.0f,  1.0f,  0.0f,     1.0f,  0.0f,  0.0f,

        // ceiling
         pmax,  ymax, -pmax,    tfmax, tfmax,    0.0f, -1.0f,  0.0f,    -1.0f,  0.0f,  0.0f,
         pmax,  ymax,  pmax,    tfmax, 0.0f,     0.0f, -1.0f,  0.0f,    -1.0f,  0.0f,  0.0f,
        -pmax,  ymax,  pmax,    0.0f,  0.0f,     0.0f, -1.0f,  0.0f,    -1.0f,  0.0f,  0.0f,
         pmax,  ymax, -pmax,    tfmax, tfmax,    0.0f, -1.0f,  0.0f,    -1.0f,  0.0f,  0.0f,
        -pmax,  ymax,  pmax,    0.0f,  0.0f,     0.0f, -1.0f,  0.0f,    -1.0f,  0.0f,  0.0f,
        -pmax,  ymax, -pmax,    0.0f,  tfmax,    0.0f, -1.0f,  0.0f,    -1.0f,  0.0f,  0.0f,
    };

    m_bindGeometry(geometry, verts);
}

void MeshManager::m_genChunkWallXPos() {
    _M_SHREKROOMS_DEFINE_WORLD_DATA_CONSTEXPR();

    const size_t meshId = s_meshToId(Mesh::ChunkWallXPos);

    m_materials[meshId] = MaterialManager::MaterialID::Wall;
    gl::Geometry &geometry = m_geometries[meshId];

    std::vector<float> verts {
        // main
         wmax, -ymax, -gmax,    0.0f,  twmax,   -1.0f,  0.0f,  0.0f,     0.0f,  1.0f,  0.0f,
         wmax, -ymax,  gmax,    twmax, twmax,   -1.0f,  0.0f,  0.0f,     0.0f,  1.0f,  0.0f,
         wmax,  ymax,  gmax,    twmax, 0.0f,    -1.0f,  0.0f,  0.0f,     0.0f,  1.0f,  0.0f,
         wmax, -ymax, -gmax,    0.0f,  twmax,   -1.0f,  0.0f,  0.0f,     0.0f,  1.0f,  0.0f,
         wmax,  ymax,  gmax,    twmax, 0.0f,    -1.0f,  0.0f,  0.0f,     0.0f,  1.0f,  0.0f,
         wmax,  ymax, -gmax,    0.0f,  0.0f,    -1.0f,  0.0f,  0.0f,     0.0f,  1.0f,  0.0f,

        // sides
         pmax, -ymax, -gmax,    0.0f,  twmax,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
         wmax, -ymax, -gmax,    tgmax, twmax,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
         wmax,  ymax, -gmax,    tgmax, 0.0f,    0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
         pmax, -ymax, -gmax,    0.0f,  twmax,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
         wmax,  ymax, -gmax,    tgmax, 0.0f,    0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
         pmax,  ymax, -gmax,    0.0f,  0.0f,    0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,

         wmax, -ymax,  gmax,    0.0f,  twmax,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
         pmax, -ymax,  gmax,    tgmax, twmax,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
         pmax,  ymax,  gmax,    tgmax, 0.0f,    0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
         wmax, -ymax,  gmax,    0.0f,  twmax,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
         pmax,  ymax,  gmax,    tgmax, 0.0f,    0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
         wmax,  ymax,  gmax,    0.0f,  0.0f,    0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
    };

    m_bindGeometry(geometry, verts);
}

void MeshManager::m_genChunkWallXNeg() {
    _M_SHREKROOMS_DEFINE_WORLD_DATA_CONSTEXPR();

    const size_t meshId = s_meshToId(Mesh::ChunkWallXNeg);

    m_materials[meshId] = MaterialManager::MaterialID::Wall;
    gl::Geometry &geometry = m_geometries[meshId];

    std::vector<float> verts {
        // main
        -wmax, -ymax,  gmax,    0.0f,  twmax,    1.0f,  0.0f,  0.0f,     0.0f, -1.0f,  0.0f,
        -wmax, -ymax, -gmax,    twmax, twmax,    1.0f,  0.0f,  0.0f,     0.0f, -1.0f,  0.0f,
        -wmax,  ymax, -gmax,    twmax, 0.0f,     1.0f,  0.0f,  0.0f,     0.0f, -1.0f,  0.0f,
        -wmax, -ymax,  gmax,    0.0f,  twmax,    1.0f,  0.0f,  0.0f,     0.0f, -1.0f,  0.0f,
        -wmax,  ymax, -gmax,    twmax, 0.0f,     1.0f,  0.0f,  0.0f,     0.0f, -1.0f,  0.0f,
        -wmax,  ymax,  gmax,    0.0f,  0.0f,     1.0f,  0.0f,  0.0f,     0.0f, -1.0f,  0.0f,

        // sides
        -wmax, -ymax, -gmax,    0.0f,  twmax,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
        -pmax, -ymax, -gmax,    tgmax, twmax,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
        -pmax,  ymax, -gmax,    tgmax, 0.0f,    0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
        -wmax, -ymax, -gmax,    0.0f,  twmax,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
        -pmax,  ymax, -gmax,    tgmax, 0.0f,    0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
        -wmax,  ymax, -gmax,    0.0f,  0.0f,    0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,

        -pmax, -ymax,  gmax,    0.0f,  twmax,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
        -wmax, -ymax,  gmax,    tgmax, twmax,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
        -wmax,  ymax,  gmax,    tgmax, 0.0f,    0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
        -pmax, -ymax,  gmax,    0.0f,  twmax,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
        -wmax,  ymax,  gmax,    tgmax, 0.0f,    0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
        -pmax,  ymax,  gmax,    0.0f,  0.0f,    0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
    };

    m_bindGeometry(geometry, verts);
}

void MeshManager::m_genChunkWallZPos() {
    _M_SHREKROOMS_DEFINE_WORLD_DATA_CONSTEXPR();

    const size_t meshId = s_meshToId(Mesh::ChunkWallZPos);

    m_materials[meshId] = MaterialManager::MaterialID::Wall;
    gl::Geometry &geometry = m_geometries[meshId];

    std::vector<float> verts {
        // main
         gmax, -ymax,  wmax,    0.0f,  twmax,    0.0f,  0.0f, -1.0f,     0.0f,  1.0f,  0.0f,
        -gmax, -ymax,  wmax,    twmax, twmax,    0.0f,  0.0f, -1.0f,     0.0f,  1.0f,  0.0f,
        -gmax,  ymax,  wmax,    twmax, 0.0f,     0.0f,  0.0f, -1.0f,     0.0f,  1.0f,  0.0f,
         gmax, -ymax,  wmax,    0.0f,  twmax,    0.0f,  0.0f, -1.0f,     0.0f,  1.0f,  0.0f,
        -gmax,  ymax,  wmax,    twmax, 0.0f,     0.0f,  0.0f, -1.0f,     0.0f,  1.0f,  0.0f,
         gmax,  ymax,  wmax,    0.0f,  0.0f,     0.0f,  0.0f, -1.0f,     0.0f,  1.0f,  0.0f,

        // sides
        -gmax, -ymax,  pmax,    0.0f,  twmax,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
        -gmax, -ymax,  gmax,    tgmax, twmax,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
        -gmax,  ymax,  gmax,    tgmax, 0.0f,    0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
        -gmax, -ymax,  pmax,    0.0f,  twmax,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
        -gmax,  ymax,  gmax,    tgmax, 0.0f,    0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
        -gmax,  ymax,  pmax,    0.0f,  0.0f,    0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,

         gmax, -ymax,  gmax,    0.0f,  twmax,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
         gmax, -ymax,  pmax,    tgmax, twmax,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
         gmax,  ymax,  pmax,    tgmax, 0.0f,    0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
         gmax, -ymax,  gmax,    0.0f,  twmax,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
         gmax,  ymax,  pmax,    tgmax, 0.0f,    0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
         gmax,  ymax,  gmax,    0.0f,  0.0f,    0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
    };

    m_bindGeometry(geometry, verts);
}

void MeshManager::m_genChunkWallZneg() {
    _M_SHREKROOMS_DEFINE_WORLD_DATA_CONSTEXPR();

    const size_t meshId = s_meshToId(Mesh::ChunkWallZNeg);

    m_materials[meshId] = MaterialManager::MaterialID::Wall;
    gl::Geometry &geometry = m_geometries[meshId];

    std::vector<float> verts {
        // main
        -gmax, -ymax, -wmax,    0.0f,  twmax,    0.0f,  0.0f,  1.0f,     0.0f, -1.0f,  0.0f,
         gmax, -ymax, -wmax,    twmax, twmax,    0.0f,  0.0f,  1.0f,     0.0f, -1.0f,  0.0f,
         gmax,  ymax, -wmax,    twmax, 0.0f,     0.0f,  0.0f,  1.0f,     0.0f, -1.0f,  0.0f,
        -gmax, -ymax, -wmax,    0.0f,  twmax,    0.0f,  0.0f,  1.0f,     0.0f, -1.0f,  0.0f,
         gmax,  ymax, -wmax,    twmax, 0.0f,     0.0f,  0.0f,  1.0f,     0.0f, -1.0f,  0.0f,
        -gmax,  ymax, -wmax,    0.0f,  0.0f,     0.0f,  0.0f,  1.0f,     0.0f, -1.0f,  0.0f,

        // sides
         gmax, -ymax, -pmax,    0.0f,  twmax,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
         gmax, -ymax, -gmax,    tgmax, twmax,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
         gmax,  ymax, -gmax,    tgmax, 0.0f,    0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
         gmax, -ymax, -pmax,    0.0f,  twmax,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
         gmax,  ymax, -gmax,    tgmax, 0.0f,    0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
         gmax,  ymax, -pmax,    0.0f,  0.0f,    0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,

        -gmax, -ymax, -gmax,    0.0f,  twmax,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
        -gmax, -ymax, -pmax,    tgmax, twmax,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
        -gmax,  ymax, -pmax,    tgmax, 0.0f,    0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
        -gmax, -ymax, -gmax,    0.0f,  twmax,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
        -gmax,  ymax, -pmax,    tgmax, 0.0f,    0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
        -gmax,  ymax, -gmax,    0.0f,  0.0f,    0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
    };

    m_bindGeometry(geometry, verts);
}

void MeshManager::m_genShrek() {
    const size_t meshId = s_meshToId(Mesh::Shrek);

    m_materials[meshId] = MaterialManager::MaterialID::Shrek;
    gl::Geometry &geometry = m_geometries[meshId];

    const float smax = 0.5f * defines::shrek::width;
    const float ymin = 0.5f * defines::world::chunkHeight;
    const float ymax = -ymin + defines::shrek::height;

    std::vector<float> verts = {
        0.0f, -ymin, -smax,     0.0f, 1.0f,     0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
        0.0f, -ymin,  smax,     1.0f, 1.0f,     0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
        0.0f,  ymax,  smax,     1.0f, 0.0f,     0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
        0.0f, -ymin, -smax,     0.0f, 1.0f,     0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
        0.0f,  ymax,  smax,     1.0f, 0.0f,     0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
        0.0f,  ymax, -smax,     0.0f, 0.0f,     0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
    };

    m_bindGeometry(geometry, verts);
}

#undef _M_SHREKROOMS_DEFINE_WORLD_DATA_CONSTEXPR
