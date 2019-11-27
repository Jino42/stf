//
// Created by ntoniolo on 27/11/2019.
//

#include "ClKernel.hpp"
#include "ClProgram.hpp"
#include "Particle/PaticleEmitter/AParticleEmitter.hpp"

ClKernel::ClKernel(std::string const &name) :
	kernel_(ClProgram::Get().getKernel(name)),
	currentArgs_(0)
{ }

cl::Kernel &ClKernel::getKernel() {
	return kernel_;
}

cl::Kernel const &ClKernel::getKernel() const {
	return kernel_;
}

cl_int ClKernel::setArgsGPUBuffers(AParticleEmitter &emitter, eParticleBuffer buffers) {
	if ((buffers & eParticleBuffer::kData) == eParticleBuffer::kData)
		setArgs(emitter.getParticleOCGL_BufferData().mem);

	if ((buffers & eParticleBuffer::kAlive) == eParticleBuffer::kAlive)
		setArgs(emitter.getParticleBufferAlive());
	if ((buffers & eParticleBuffer::kSpawned) == eParticleBuffer::kSpawned)
		setArgs(emitter.getParticleBufferSpawned());
	if ((buffers & eParticleBuffer::kDeath) == eParticleBuffer::kDeath)
		setArgs(emitter.getParticleBufferDeath());
	if ((buffers & eParticleBuffer::kSubIndex) == eParticleBuffer::kSubIndex)
		setArgs(emitter.getParticleSubBuffersLength());
	return 0;
}
