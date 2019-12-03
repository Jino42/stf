#pragma once

#include "AParticleEmitter.hpp"

class Shader;
class Model;

class ParticleEmitterMesh : public AParticleEmitter {
public:
	ParticleEmitterMesh(ParticleSystem &system, ClQueue &queue, std::string const &name, size_t nbParticlePerSec, size_t nbParticleMax);

	void init() override;

	void render() override;

	void update(float deltaTime) override;

private:
	Model											*model_;
};
