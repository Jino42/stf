#pragma once

#include "AParticleEmitter.hpp"
#include "ParticleSpriteAtlas.hpp"

class Shader;

struct ParticleDataSprite {
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
	OCGL_Buffer OCGLBufferParticles_SpriteData_;
	unsigned int VBO, VAO, EBO;

	ParticleSpriteAtlas atlas_;
	cl::Buffer	gpuBufferParticles_Dist_;
	cl::Buffer	gpuBufferOutput_nbParticleActive_;

	void sortDeviceBuffer_();
	void sortDeviceBufferCalculateDistanceParticle_();
	void getNbParticleActive_();
};
