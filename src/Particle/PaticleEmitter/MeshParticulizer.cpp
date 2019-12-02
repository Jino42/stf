//
// Created by Noe TONIOLO on 2019-07-10.
//

#include "MeshParticulizer.hpp"
#include "Particle/ParticleData.hpp"
#include "Engine/Camera.hpp"
#include <PathManager.hpp>

MeshParticulizer::MeshParticulizer(ParticleSystem &system, ClQueue &queue, std::string const &name, Mesh &mesh) :
        AParticleEmitter(system, queue, name, mesh.getIndice().size() / 3, 0),
        mesh_(mesh),
        deviceBufferVertexBasic_(sizeof(PointVertexData) * nbParticleMax_)
{
    //modules_.emplace_back(std::make_unique<ParticleSpawnModule>(*this));

    boost::filesystem::path &pathShaders = PathManager::Get().getPath("shaders");

    shader_.attach((pathShaders / "basic.vert").generic_string());
    shader_.attach((pathShaders / "basic.geom").generic_string());
    shader_.attach((pathShaders / "basic.frag").generic_string());
    shader_.link();

    glGenVertexArrays(1, &VAO);
    reload();
    glBindVertexArray(0);
}

void MeshParticulizer::reload() {
    if (debug_)
        printf("%s\n", __FUNCTION_NAME__);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, particleOCGL_BufferData_.vbo);

    glEnableVertexAttribArray(3); //Position
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleData), reinterpret_cast<const void *>(offsetof(ParticleData, position)));
    glEnableVertexAttribArray(4); //Velocity
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleData), reinterpret_cast<const void *>(offsetof(ParticleData, velocity)));
    glEnableVertexAttribArray(5); //Color
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(ParticleData), reinterpret_cast<const void *>(offsetof(ParticleData, color)));
    glEnableVertexAttribArray(6); //Rotate
    glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleData), reinterpret_cast<const void *>(offsetof(ParticleData, rotate)));
    glEnableVertexAttribArray(7); //Size
    glVertexAttribPointer(7, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleData), reinterpret_cast<const void *>(offsetof(ParticleData, size)));
    glEnableVertexAttribArray(8); //Age
    glVertexAttribPointer(8, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleData), reinterpret_cast<const void *>(offsetof(ParticleData, age)));
    glEnableVertexAttribArray(9); //LifeTime
    glVertexAttribPointer(9, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleData), reinterpret_cast<const void *>(offsetof(ParticleData, lifeTime)));
    glEnableVertexAttribArray(10); //IsAlive
    glVertexAttribPointer(10, 1, GL_INT, GL_FALSE, sizeof(ParticleData), reinterpret_cast<const void *>(offsetof(ParticleData, isAlive)));

    deviceBufferVertexBasic_ = OCGL_Buffer(nbParticleMax_ * sizeof(PointVertexData));
    glBindBuffer(GL_ARRAY_BUFFER, deviceBufferVertexBasic_.vbo);

    glEnableVertexAttribArray(11); //Offset1
    glVertexAttribPointer(11, 3 * 3, GL_FLOAT, GL_FALSE, sizeof(PointVertexData), reinterpret_cast<const void *>(offsetof(PointVertexData, position)));
    glEnableVertexAttribArray(12); //Offset2
    glVertexAttribPointer(12, 3 * 3, GL_FLOAT, GL_FALSE, sizeof(PointVertexData), reinterpret_cast<const void *>(offsetof(PointVertexData, normal)));
    glEnableVertexAttribArray(13); //Blend
    glVertexAttribPointer(13, 2 * 3, GL_FLOAT, GL_FALSE, sizeof(PointVertexData), reinterpret_cast<const void *>(offsetof(PointVertexData, texCoords)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void MeshParticulizer::init() {
    if (debug_)
        printf("%s\n", __FUNCTION_NAME__);
    for (auto &module : modules_) {
        module->init();
    }
}

void MeshParticulizer::update(float deltaTime) {
    if (debug_)
        printf("%s\n", __FUNCTION_NAME__);
    checkReload();
    for (auto &module : modules_) {
        module->update(deltaTime);
    }
}


void MeshParticulizer::render() {
    if (debug_)
        printf("%s\n", __FUNCTION_NAME__);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDepthMask(GL_FALSE);
    shader_.activate();
    shader_.setMat4("projection", Camera::focus->getProjectionMatrix());
    shader_.setMat4("view", Camera::focus->getViewMatrix());
    glActiveTexture(GL_TEXTURE0);

    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, nbParticleMax_);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_TRUE);
}