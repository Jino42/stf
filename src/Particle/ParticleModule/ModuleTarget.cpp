#include "ModuleTarget.hpp"
#include "Particle/ParticleSystem.hpp"
#include "Particle/PaticleEmitter/AParticleEmitter.hpp"
#include "Engine/Random.hpp"
#include "Cl/ClContext.hpp"
#include "Cl/ClProgram.hpp"
#include "Cl/ClQueue.hpp"
#include "OpenCGL_Tools.hpp"
#include "cl_type.hpp"

ModuleTarget::ModuleTarget(AParticleEmitter &emitter) :
		AParticleModule(emitter),
		gpuBufferParticles_Target_(ClContext::Get().context, CL_MEM_WRITE_ONLY, nbParticleMax_ * sizeof(ParticleDataTarget)),
		cpuBufferParticles_Target_(std::make_unique<ParticleDataTarget[]>(nbParticleMax_)) {

	ClProgram::Get().addProgram(pathKernel_ / "MeshParticulizer.cl");

	kernelInit_.setKernel(emitter_, "initMeshParticulizer");
	kernelInit_.setArgsGPUBuffers(eParticleBuffer::kData);
}

ParticleDataTarget *ModuleTarget::getCpuBuffer() {
	return cpuBufferParticles_Target_.get();
}

cl::Buffer &ModuleTarget::getGpuBuffer() {
	return gpuBufferParticles_Target_;
}

void ModuleTarget::writeDataToGPU_Buffer() {
	queue_.getQueue().enqueueWriteBuffer(gpuBufferParticles_Target_, CL_TRUE, 0, sizeof(ParticleDataTarget) * nbParticleMax_, cpuBufferParticles_Target_.get());
	kernelInit_.beginAndSetUpdatedArgs(gpuBufferParticles_Target_, glmVec3toClFloat3(emitter_.getSystem().getPosition()));
}