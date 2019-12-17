#include "ParticleEmitterPoint.hpp"
#include "Cl/ClQueue.hpp"
#include "Engine/Camera.hpp"
#include "Engine/Shader.hpp"
#include "Particle/ParticleData.hpp"
#include "Particle/ParticleModule/ModuleAttractor.hpp"
#include <PathManager.hpp>

ParticleEmitterPoint::ParticleEmitterPoint(ParticleSystem &system, ClQueue &queue, std::string const &name, size_t nbParticle)
    : AParticleEmitter(system, queue, name, nbParticle, 0) {
    shader_.attach((PathManager::Get().getPath("shaders") / "particlePoint.vert").generic_string());
    shader_.attach((PathManager::Get().getPath("shaders") / "particlePoint.frag").generic_string());
    shader_.link();
    /*
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, OCGLBufferEmitterParticles_.vbo);

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

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	*/
    model_.makeSphere(1.f, 36, 18);
    actor_.assign(&model_);
    glBindBuffer(GL_ARRAY_BUFFER, OCGLBufferEmitterParticles_.vbo);
    for (const auto &mesh: model_.getMeshes()) {
        unsigned int VAO = mesh.getVAO();
        glBindVertexArray(VAO);
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

        glVertexAttribDivisor(3, 1);
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

void ParticleEmitterPoint::init() {
    for (auto &module: modules_) {
        module->init();
    }
}

void ParticleEmitterPoint::update(float deltaTime) {
    AParticleEmitter::update(deltaTime);

    for (auto &module: modules_) {
        module->update(deltaTime);
    }
}

void ParticleEmitterPoint::supprMe(float scale) {
    actor_.resetTransform();
    actor_.scale(scale);
    actor_.updateTransform();
}

void ParticleEmitterPoint::render() {
    shader_.activate();
    shader_.setMat4("projection", Camera::focus->getProjectionMatrix());
    shader_.setMat4("view", Camera::focus->getViewMatrix());
    shader_.setMat4("model", actor_.getTransform());
    //shader_.setVec3("uCameraPosition", Camera::Get().getPosition());

    //glBindVertexArray(VAO);
    //glDrawArrays(GL_POINTS, 0, nbParticleMax_);

    for (const auto &mesh: model_.getMeshes()) {
        mesh.activeTexture();
        glBindVertexArray(mesh.getVAO());
        glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(mesh.getIndice().size()), GL_UNSIGNED_INT, 0, nbParticleMax_);
    }

}