#pragma once

#include "AParticleEmitter.hpp"
#include "Engine/ModelEngine/Model.hpp"

class Shader;

class ParticleEmitterMesh : public AParticleEmitter {
public:
	ParticleEmitterMesh(ParticleSystem &system, ClQueue &queue, std::string const &name, size_t nbParticle, size_t nbParticlePerSec);

	void init() override;

	void render() override;

	void update(float deltaTime) override;

private:
	Model											model_;
};
