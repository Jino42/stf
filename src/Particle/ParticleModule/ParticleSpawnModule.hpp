#pragma once

#include "AParticleModule.hpp"
#include "Cl/ClQueue.hpp"
#include "DeviceBuffer.hpp"
#include "Range.hpp"

class AParticleEmitter;

struct ModuleSpawnParams {
    Rangef startLifeTime;
    Rangef startSpeed;
    Rangef startSize;
    Rangef startRotation;
    //cl_float3 startColor;
};

class ParticleSpawnModule : public AParticleModule {
public:
	ParticleSpawnModule(AParticleEmitter &emitter);
	void	init() override;
	void	update(float deltaTime) override;
	void	spawn(unsigned int nbToSpawn, unsigned int at) override;
    void    reload() override;

private:
	cl::Buffer		    bufferModuleParams_;
    ModuleSpawnParams   moduleSpawnParams_;
	unsigned int	    spwaned_;
};