#include "ParticleMoveToTarget.hpp"
#include "Particle/ParticleSystem.hpp"
#include "Particle/PaticleEmitter/AParticleEmitter.hpp"
#include "Engine/Random.hpp"
#include "Cl/ClContext.hpp"
#include "Cl/ClProgram.hpp"
#include "Cl/ClQueue.hpp"
#include "OpenCGL_Tools.hpp"
#include "cl_type.hpp"
#include "ParticuleTargetModule.hpp"


ParticleMoveToTarget::ParticleMoveToTarget(AParticleEmitter &emitter) :
		AParticleModule(emitter)
{
	if (debug_)
		printf("%s\n", __FUNCTION_NAME__);
	if (!emitter_.contain<ParticuleTargetModule>()) {
		emitter_.addModule<ParticuleTargetModule>();
	}
	targetModule_ = emitter_.getModule<ParticuleTargetModule>();

	ClProgram::Get().addProgram(pathKernel_ / "Target.cl");

	kernelUpdate_.setKernel(emitter_, "updateMoveToTarget");
	kernelUpdate_.setArgsGPUBuffers(eParticleBuffer::kData);
}

void ParticleMoveToTarget::update(float deltaTime) {
	if (debug_)
		printf("%s\n", __FUNCTION_NAME__);
	kernelUpdate_.beginAndSetUpdatedArgs(targetModule_->getGpuBuffer(), deltaTime);
	std::vector<cl::Memory> cl_vbos;
	cl_vbos.push_back(emitter_.getParticleOCGL_BufferData().mem);
	OpenCGL::RunKernelWithMem(queue_.getQueue(), kernelUpdate_, cl_vbos, cl::NullRange, cl::NDRange(nbParticleMax_));
}
