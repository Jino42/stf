#include "ModuleSizeOverLifetime.hpp"
#include "Particle/PaticleEmitter/ParticleEmitterMesh.hpp"
#include <iostream>
#include "Cl/ClProgram.hpp"
#include "OpenCGL_Tools.hpp"

ModuleSizeOverLifetime::ModuleSizeOverLifetime(AParticleEmitter &emitter) :
        AParticleModule(emitter),
        bufferModule_(ClContext::Get().context, CL_MEM_READ_WRITE, sizeof(ModuleSizeOverLifetimeParams))
{
    ClProgram::Get().addProgram(pathKernel_ / "Size.cl");
    moduleSizeOverLifetimeParams_.size.rangeMin = 0.0f;
    moduleSizeOverLifetimeParams_.size.rangeMax = 10.0f;
}

void	ModuleSizeOverLifetime::init() {
	if (debug_)
		printf("%s\n", __FUNCTION_NAME__);
    queue_.getQueue().enqueueWriteBuffer(bufferModule_, CL_TRUE, 0, sizeof(ModuleSizeOverLifetimeParams), &moduleSizeOverLifetimeParams_);
}

void	ModuleSizeOverLifetime::update(float deltaTime) {
	if (debug_)
		printf("%s\n", __FUNCTION_NAME__);
    cl::Kernel &kernel = ClProgram::Get().getKernel("sizeUpdate");

    queue_.getQueue().enqueueWriteBuffer(bufferModule_, CL_TRUE, 0, sizeof(ModuleSizeOverLifetimeParams), &moduleSizeOverLifetimeParams_);
    kernel.setArg(0, emitter_.getDeviceBuffer().mem);
    kernel.setArg(1, bufferModule_);

    OpenCGL::RunKernelWithMem(queue_.getQueue(), kernel, emitter_.getDeviceBuffer().mem, cl::NullRange, cl::NDRange(nbParticleMax_));
}

void    ModuleSizeOverLifetime::reload()
{
	if (debug_)
		printf("%s\n", __FUNCTION_NAME__);
    bufferModule_ = cl::Buffer(ClContext::Get().context, CL_MEM_READ_WRITE, sizeof(ModuleSizeOverLifetimeParams));
    init();
}

float &ModuleSizeOverLifetime::getSizeMin() {
    return moduleSizeOverLifetimeParams_.size.rangeMin;
}
float &ModuleSizeOverLifetime::getSizeMax() {
    return moduleSizeOverLifetimeParams_.size.rangeMax;
}


void ModuleSizeOverLifetime::setSizeMin(float min) {
    moduleSizeOverLifetimeParams_.size.rangeMin = min;
}

void ModuleSizeOverLifetime::setSizeMax(float max) {
    moduleSizeOverLifetimeParams_.size.rangeMax = max;
}