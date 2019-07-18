#include "ParticleEmitterPoint.hpp"
#include "Engine/Shader.hpp"
#include "Cl/ClQueue.hpp"
#include "Particle/ParticleData.hpp"
#include "Particle/ParticleModule/ParticleAttractorModule.hpp"
#include "Engine/MainGraphic.hpp"

ParticleEmitterPoint::ParticleEmitterPoint(ParticleSystem &system, ClQueue &queue, std::string const &name, size_t nbParticle) :
	AParticleEmitter(system, queue, name, nbParticle, 0)
{
	shader_.attach((boost::filesystem::path(ROOT_PATH) / "shader" / "particlePoint.vert").generic_string());
	shader_.attach((boost::filesystem::path(ROOT_PATH) / "shader" / "particlePoint.frag").generic_string());
	shader_.link();

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, deviceBuffer_.vbo);

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

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void ParticleEmitterPoint::init() {
	for (auto &module : modules_) {
		module->init();
	}
}

void ParticleEmitterPoint::update(float deltaTime) {
	for (auto &module : modules_) {
		module->update(deltaTime);
	}
}

void ParticleEmitterPoint::render() {
	shader_.activate();
	shader_.setMat4("projection", MainGraphic::Get().getProjectionMatrix());
	shader_.setMat4("view", MainGraphic::Get().getViewMatrix());
	//shader_.setVec3("uCameraPosition", Camera::Get().getPosition());
	glBindVertexArray(VAO);
	glDrawArrays(GL_POINTS, 0, nbParticleMax_);
}