//
// Created by ntoniolo on 27/11/2019.
//

#include "ClKernel.hpp"
#include "ClProgram.hpp"
#include "Particle/PaticleEmitter/AParticleEmitter.hpp"

ClKernel::ClKernel() :
		kernel_(nullptr),
		emitter_(nullptr),
		name_(""),
		currentArgs_(0),
		enum_(eParticleBuffer::kNone)
{

}

cl::Kernel &ClKernel::getKernel() {
	return *kernel_;
}

cl::Kernel const &ClKernel::getKernel() const {
	return *kernel_;
}

void ClKernel::setKernel(AParticleEmitter &emitter, std::string const &name) {
	name_ = name;
	emitter_ = &emitter;
}

void ClKernel::setArgsGPUBuffers(eParticleBuffer buffers) {
	enum_ = buffers;
}

cl_int ClKernel::beginAndSetUpdatedArgs()
{
	kernel_ = &ClProgram::Get().getKernel(name_);
	currentArgs_ = 0;

	return updateArgsGPUBuffers_();
}

cl_int ClKernel::updateArgsGPUBuffers_() {
	cl_int ret;

	if ((enum_ & eParticleBuffer::kData) == eParticleBuffer::kData) {
		if ((ret = setArgs_(emitter_->getParticleOCGL_BufferData().mem)))
			return ret;
	}


	if ((enum_ & eParticleBuffer::kAlive) == eParticleBuffer::kAlive) {
		if ((ret = setArgs_(emitter_->getParticleBufferAlive())))
			return ret;
	}
	if ((enum_ & eParticleBuffer::kSpawned) == eParticleBuffer::kSpawned) {
		if ((ret = setArgs_(emitter_->getParticleBufferSpawned())))
			return ret;
	}
	if ((enum_ & eParticleBuffer::kDeath) == eParticleBuffer::kDeath) {
		if ((ret = setArgs_(emitter_->getParticleBufferDeath())))
			return ret;
	}
	if ((enum_ & eParticleBuffer::kSubIndex) == eParticleBuffer::kSubIndex) {
		if ((ret = setArgs_(emitter_->getParticleSubBuffersLength())))
			return ret;
	}


	if ((enum_ & eParticleBuffer::kEmitterParam) == eParticleBuffer::kEmitterParam) {
		if ((ret = setArgs_(emitter_->getBufferEmitterParam())))
			return ret;
	}
	return 0;
}
