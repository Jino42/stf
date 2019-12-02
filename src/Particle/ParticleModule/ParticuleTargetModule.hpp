#pragma once

#include "AParticleModule.hpp"

class AParticleEmitter;

struct ModuleTargetData {
	cl_float3	target;
};

class ParticuleTargetModule : public AParticleModule {
public:
	ParticuleTargetModule(AParticleEmitter &emitter);

	ModuleTargetData *getCpuBuffer();
	cl::Buffer &getGpuBuffer();
	void writeDataToGPU_Buffer();

private:
	cl::Buffer		gpuBufferTargetParticles_;
	std::unique_ptr<ModuleTargetData[]> cpuBufferTargetParticles_;
};
