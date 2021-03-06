#include "ParticleEmitterSPH.hpp"
#include "Cl/ClQueue.hpp"
#include "Engine/Camera.hpp"
#include "Engine/Shader.hpp"
#include "Particle/ParticleData.hpp"
#include "Particle/ParticleModule/ModuleSPH.hpp"
#include <Engine/ShaderManager.hpp>
#include <PathManager.hpp>

ParticleEmitterSPH::ParticleEmitterSPH(ParticleSystem &system, ClQueue &queue, std::string const &name, size_t nbParticle)
    : AParticleEmitter(system, queue, name, nbParticle, 0),
      moduleSph_(nullptr),
      lineWidth_(1.f) {
    modules_.emplace_back(std::make_unique<ModuleSPH>(*this));
    moduleSph_ = this->getModule<ModuleSPH>();
    ShaderManager::Get().addShader("particleSPH");
    ShaderManager::Get().getShader("particleSPH").attach((PathManager::Get().getPath("shaders") / "particleSPH.vert").generic_string());
    ShaderManager::Get().getShader("particleSPH").attach((PathManager::Get().getPath("shaders") / "particleSPH.frag").generic_string());
    ShaderManager::Get().getShader("particleSPH").link();

    ShaderManager::Get().addShader("particleSPHDebug");
    ShaderManager::Get().getShader("particleSPHDebug").attach((PathManager::Get().getPath("shaders") / "particleSPHDebug.vert").generic_string());
    ShaderManager::Get().getShader("particleSPHDebug").attach((PathManager::Get().getPath("shaders") / "particleSPHDebug.geom").generic_string());
    ShaderManager::Get().getShader("particleSPHDebug").attach((PathManager::Get().getPath("shaders") / "particleSPHDebug.frag").generic_string());
    ShaderManager::Get().getShader("particleSPHDebug").link();

    model_.makeSphere(1.f, 36, 18);
    actor_.assign(&model_);
    glBindBuffer(GL_ARRAY_BUFFER, OCGLBufferEmitterParticles_.vbo);
    for (const auto &mesh: model_.getMeshes()) {
        unsigned int VAO = mesh.getVAO();
        glBindVertexArray(VAO);
        glEnableVertexAttribArray(3); //Position
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleData), reinterpret_cast<const void *>(offsetof(ParticleData, position)));

        glVertexAttribDivisor(3, 1);

        glBindBuffer(GL_ARRAY_BUFFER, moduleSph_->OCGLBufferParticles_SPH_Data_.vbo);

        glEnableVertexAttribArray(4); //Velocity
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleDataSPH), reinterpret_cast<const void *>(offsetof(ParticleDataSPH, velocity)));
        glEnableVertexAttribArray(5); //Pressure
        glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleDataSPH), reinterpret_cast<const void *>(offsetof(ParticleDataSPH, pressure)));
        glEnableVertexAttribArray(6); //Density
        glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleDataSPH), reinterpret_cast<const void *>(offsetof(ParticleDataSPH, density)));
        glEnableVertexAttribArray(7); //Mass
        glVertexAttribPointer(7, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleDataSPH), reinterpret_cast<const void *>(offsetof(ParticleDataSPH, mass)));
        glEnableVertexAttribArray(8); //Flag
        glVertexAttribPointer(8, 1, GL_INT, GL_FALSE, sizeof(ParticleDataSPH), reinterpret_cast<const void *>(offsetof(ParticleDataSPH, flag)));

        glEnableVertexAttribArray(9); //ForcePressure
        glVertexAttribPointer(9, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleDataSPH), reinterpret_cast<const void *>(offsetof(ParticleDataSPH, force1)));
        glEnableVertexAttribArray(10); //ForceViscosity
        glVertexAttribPointer(10, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleDataSPH), reinterpret_cast<const void *>(offsetof(ParticleDataSPH, force2)));

        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);
        glVertexAttribDivisor(7, 1);
        glVertexAttribDivisor(8, 1);
        glVertexAttribDivisor(9, 1);
        glVertexAttribDivisor(10, 1);

        glBindVertexArray(0);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void ParticleEmitterSPH::init() {
    for (auto &module: modules_) {
        module->init();
    }
}

void ParticleEmitterSPH::update(float deltaTime) {
    AParticleEmitter::update(deltaTime);

    for (auto &module: modules_) {
        module->update(deltaTime);
    }
}

void ParticleEmitterSPH::supprMe(float scale) {
    actor_.resetTransform();
    actor_.scale(scale);
    actor_.updateTransform();
}

void ParticleEmitterSPH::render() {
    ShaderManager::Get().getShader("particleSPH").activate();
    ShaderManager::Get().getShader("particleSPH").setMat4("projection", Camera::focus->getProjectionMatrix());
    ShaderManager::Get().getShader("particleSPH").setMat4("view", Camera::focus->getViewMatrix());
    ShaderManager::Get().getShader("particleSPH").setMat4("model", actor_.getTransform());

    for (const auto &mesh: model_.getMeshes()) {
        mesh.activeTexture();
        glBindVertexArray(mesh.getVAO());
        glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(mesh.getIndice().size()), GL_UNSIGNED_INT, 0, nbParticleMax_);
    }

    ShaderManager::Get().getShader("particleSPHDebug").activate();
    ShaderManager::Get().getShader("particleSPHDebug").setMat4("projection", Camera::focus->getProjectionMatrix());
    ShaderManager::Get().getShader("particleSPHDebug").setMat4("view", Camera::focus->getViewMatrix());
    ShaderManager::Get().getShader("particleSPHDebug").setMat4("model", actor_.getTransform());
    ShaderManager::Get().getShader("particleSPHDebug").setInt("flag", moduleSph_->flag_);
    glLineWidth(lineWidth_);
    glDrawArraysInstanced(GL_POINTS, 0, 1, nbParticleMax_);
}

float &ParticleEmitterSPH::getLineWidth() {
    return lineWidth_;
}