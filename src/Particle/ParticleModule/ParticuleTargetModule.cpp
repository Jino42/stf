#include "ParticuleTargetModule.hpp"
#include "Particle/ParticleSystem.hpp"
#include "Particle/PaticleEmitter/AParticleEmitter.hpp"
#include "Engine/Random.hpp"
#include "Cl/ClContext.hpp"
#include "Cl/ClProgram.hpp"
#include "Cl/ClQueue.hpp"
#include "OpenCGL_Tools.hpp"
#include "cl_type.hpp"

ParticuleTargetModule::ParticuleTargetModule(AParticleEmitter &emitter) :
		AParticleModule(emitter),
		gpuBufferTargetParticles_(ClContext::Get().context, CL_MEM_WRITE_ONLY, nbParticleMax_ * sizeof(ModuleTargetData)),
		cpuBufferTargetParticles_(std::make_unique<ModuleTargetData[]>(nbParticleMax_)) {

	ClProgram::Get().addProgram(pathKernel_ / "MeshParticulizer.cl");

	kernelInit_.setKernel(emitter_, "initMeshParticulizer");
	kernelInit_.setArgsGPUBuffers(eParticleBuffer::kData);
}

ModuleTargetData *ParticuleTargetModule::getCpuBuffer() {
	return cpuBufferTargetParticles_.get();
}

cl::Buffer &ParticuleTargetModule::getGpuBuffer() {
	return gpuBufferTargetParticles_;
}

void ParticuleTargetModule::writeDataToGPU_Buffer() {
	queue_.getQueue().enqueueWriteBuffer(gpuBufferTargetParticles_, CL_TRUE, 0, sizeof(ModuleTargetData) * nbParticleMax_, cpuBufferTargetParticles_.get());
	kernelInit_.beginAndSetUpdatedArgs(gpuBufferTargetParticles_, glmVec3toClFloat3(emitter_.getSystem().getPosition()));
}