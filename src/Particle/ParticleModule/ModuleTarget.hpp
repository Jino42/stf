#pragma once

#include "AParticleModule.hpp"

class AParticleEmitter;

struct ParticleDataTarget {
	cl_float3	target;
};

class ModuleTarget : public AParticleModule {
public:
	ModuleTarget(AParticleEmitter &emitter);

	ParticleDataTarget *getCpuBuffer();
	cl::Buffer &getGpuBuffer();
	void writeDataToGPU_Buffer();

private:
	cl::Buffer		gpuBufferParticles_Target_;
	std::unique_ptr<ParticleDataTarget[]> cpuBufferParticles_Target_;
};
