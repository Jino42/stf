#include "ParticleEmitterMesh.hpp"
#include "Engine/Shader.hpp"
#include "Cl/ClQueue.hpp"
#include "Particle/ParticleData.hpp"
#include "Particle/ParticleModule/ParticleSpawnModule.hpp"
#include "Engine/Camera.hpp"
#include <PathManager.hpp>
#include <Engine/ModelEngine/ModelManager.hpp>

ParticleEmitterMesh::ParticleEmitterMesh(ParticleSystem &system, ClQueue &queue, std::string const &name, size_t nbParticlePerSec, size_t nbParticleMax) :
	AParticleEmitter(system, queue, name, nbParticleMax, nbParticlePerSec)
{
	modules_.emplace_back(std::make_unique<ParticleSpawnModule>(*this));

	shader_.attach((PathManager::Get().getPath("shaders") / "particleMesh.vert").generic_string());
	shader_.attach((PathManager::Get().getPath("shaders") / "particleMesh.frag").generic_string());
	shader_.link();

	ModelManager::Get().addModel("nanosuit", PathManager::Get().getPath("objects") / "nanosuit" / "nanosuit.obj");
	model_ = &ModelManager::Get().getModel("nanosuit");

	glBindBuffer(GL_ARRAY_BUFFER, OCGLBufferEmitterParticles_.vbo);
	for (const auto &mesh : model_->getMeshes()) {
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

void ParticleEmitterMesh::init() {
	if (debug_)
		printf("%s\n", __FUNCTION_NAME__);
	for (auto &module : modules_) {
		module->init();
	}
}

void ParticleEmitterMesh::update(float deltaTime) {
	if (debug_)
		printf("%s\n", __FUNCTION_NAME__);
	AParticleEmitter::update(deltaTime);
	checkReload();
	for (auto &module : modules_) {
		module->update(deltaTime);
	}
}

void ParticleEmitterMesh::render() {
	if (debug_)
		printf("%s\n", __FUNCTION_NAME__);
	shader_.activate();
	shader_.setMat4("projection", Camera::focus->getProjectionMatrix());
	shader_.setMat4("view", Camera::focus->getViewMatrix());

	for (const auto &mesh : model_->getMeshes()) {
		mesh.activeTexture();
		glBindVertexArray(mesh.getVAO());
		glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(mesh.getIndice().size()), GL_UNSIGNED_INT, 0, nbParticleMax_);
	}
}