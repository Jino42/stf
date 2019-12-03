#pragma once

#include "AParticleModule.hpp"
#include "Cl/ClQueue.hpp"
#include "OCGL_Buffer.hpp"
#include "Range.hpp"

class AParticleEmitter;

struct ModuleParamSpawn {
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
	cl::Buffer		    gpuBufferModuleParam_;
    ModuleParamSpawn	cpuBufferModuleParam_;
	unsigned int	    spwaned_;
};
