#pragma once

#include "AParticleModule.hpp"
#include "Cl/ClQueue.hpp"
#include "OCGL_Buffer.hpp"
#include "Range.hpp"

class AParticleEmitter;

struct ModuleParamSPH {
	Rangef size;
};

struct ParticleDataSPH {
	cl_float3	position;
	cl_float	density;
	cl_float	pressure;
	cl_float	mass;
	cl_float3	force1;
	cl_float3	force2;
	cl_float3	velocity;
};

class ModuleSPH : public AParticleModule {
public:
	ModuleSPH(AParticleEmitter &emitter);
	void	init() override;
	void	update(float deltaTime) override;
	void    reload() override;

private:
	ClKernel						kernelDensity_;
	ClKernel						kernelPressure_;
	ClKernel						kernelViscosity_;
	cl::Buffer						gpuBufferParticles_SPH_;

	cl::Buffer						gpuBufferModuleParam_;
	ModuleParamSPH					cpuBufferModuleParam_;
};
