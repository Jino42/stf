#pragma once

#include "AParticleModule.hpp"

class AParticleEmitter;

struct ParticleDataTarget {
	cl_float3	target;
};

class ParticuleTargetModule : public AParticleModule {
public:
	ParticuleTargetModule(AParticleEmitter &emitter);

	ParticleDataTarget *getCpuBuffer();
	cl::Buffer &getGpuBuffer();
	void writeDataToGPU_Buffer();

private:
	cl::Buffer		gpuBufferParticles_Target_;
	std::unique_ptr<ParticleDataTarget[]> cpuBufferParticles_Target_;
};
