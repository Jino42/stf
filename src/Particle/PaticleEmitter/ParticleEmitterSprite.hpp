#pragma once

#include "AParticleEmitter.hpp"
#include "ParticleSpriteAtlas.hpp"

class Shader;

struct ParticleSpriteData {
	cl_float2	offset1;
	cl_float2	offset2;
	float		blend;
};

class ParticleEmitterSprite : public AParticleEmitter {
public:
	ParticleEmitterSprite(ParticleSystem &system, ClQueue &queue, std::string const &name, size_t nbParticlePerSec, size_t nbParticleMax);

	void init() override;

	void render() override;

	void update(float deltaTime) override;

	void updateSpriteData();
	void reload() override;


private:
	DeviceBuffer deviceBufferSpriteData_;
	unsigned int VBO, VAO, EBO;

	ParticleSpriteAtlas atlas_;
	cl::Buffer	distBuffer_;
	cl::Buffer	nbParticleActiveOutpourBuffer_;

	void sortDeviceBuffer_();
	void sortDeviceBufferCalculateDistanceParticle_();
	void getNbParticleActive_();
};
