#include "ParticleEmitterPoint.hpp"
#include "Cl/ClQueue.hpp"
#include "Engine/Camera.hpp"
#include "Engine/Shader.hpp"
#include "Particle/ParticleData.hpp"
#include "Particle/ParticleModule/ModuleAttractor.hpp"
#include <Engine/ShaderManager.hpp>
#include <PathManager.hpp>

#include "Particle/ParticleModule/ParticleSpawnModule.hpp"

ParticleEmitterPoint::ParticleEmitterPoint(ParticleSystem &system, ClQueue &queue, std::string const &name, size_t nbParticle)
    : AParticleEmitter(system, queue, name, nbParticle, 0) {

    if (ShaderManager::Get().addShader("particlePoint")) {
        ShaderManager::Get().getShader("particlePoint").attach((PathManager::Get().getPath("shaders") / "particlePoint.vert").generic_string());
        ShaderManager::Get().getShader("particlePoint").attach((PathManager::Get().getPath("shaders") / "particlePoint.geom").generic_string());
        ShaderManager::Get().getShader("particlePoint").attach((PathManager::Get().getPath("shaders") / "particlePoint.frag").generic_string());
        ShaderManager::Get().getShader("particlePoint").link();
    }
    

    AParticleEmitter::reload();


    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, OCGLBufferEmitterParticles_.vbo);

    glEnableVertexAttribArray(0); //IsAlive
    glVertexAttribIPointer(0, 1, GL_INT, sizeof(ParticleData), reinterpret_cast<const void *>(offsetof(ParticleData, isAlive)));
    glEnableVertexAttribArray(1); //Position
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleData), reinterpret_cast<const void *>(offsetof(ParticleData, position)));
    glEnableVertexAttribArray(2); //Color
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(ParticleData), reinterpret_cast<const void *>(offsetof(ParticleData, color)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void ParticleEmitterPoint::init() {
    if (debug_)
        printf("%s\n", __FUNCTION_NAME__);
    for (auto &module: modules_) {
        module->init();
    }
}

void ParticleEmitterPoint::update(float deltaTime) {
    if (debug_)
        printf("%s\n", __FUNCTION_NAME__);
    AParticleEmitter::update(deltaTime);
    checkReload();
    nbParticleActive_ = (nbParticleMax_)-indexSub_[2];
    for (auto &module: modules_) {
        module->update(deltaTime);
    }
}

void ParticleEmitterPoint::render() {
    if (debug_)
        printf("%s\n", __FUNCTION_NAME__);
    ShaderManager::Get().getShader("particlePoint").activate();
    ShaderManager::Get().getShader("particlePoint").setMat4("projection", Camera::focus->getProjectionMatrix());
    ShaderManager::Get().getShader("particlePoint").setMat4("view", Camera::focus->getViewMatrix());
    //shader_.setVec3("uCameraPosition", Camera::Get().getPosition());

    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, nbParticleMax_);
}