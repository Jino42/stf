#pragma once

#include "AParticleModule.hpp"
#include "Cl/ClQueue.hpp"
#include "OCGL_Buffer.hpp"

class AParticleEmitter;

struct ParticleMovementModuleData {
	cl_float3	acceleration;
	cl_float3	velocity;
	float		masse;
};

class ParticleMovementModule : public AParticleModule {
public:
	ParticleMovementModule(AParticleEmitter &emitter);
	void	init() override;
	void	update(float deltaTime) override;
	void	spawn(unsigned int nbToSpawn, unsigned int at) override;
	void    reload() override;

private:
	cl::Buffer		buffer_;
};
