#include "ModuleSizeOverLifetime.hpp"
#include <iostream>
#include "Cl/ClProgram.hpp"
#include "OpenCGL_Tools.hpp"
#include "Cl/ClKernel.hpp"
#include "Particle/PaticleEmitter/AParticleEmitter.hpp"

ModuleSizeOverLifetime::ModuleSizeOverLifetime(AParticleEmitter &emitter) :
        AParticleModule(emitter),
        gpuBufferModuleParam_(ClContext::Get().context, CL_MEM_READ_WRITE, sizeof(ModuleParamSizeOverLifetime))
{
    ClProgram::Get().addProgram(pathKernel_ / "Size.cl");
    cpuBufferModuleParam_.size.rangeMin = 0.0f;
    cpuBufferModuleParam_.size.rangeMax = 10.0f;

    kernelUpdate_.setKernel(emitter_, "sizeUpdate");
    kernelUpdate_.setArgsGPUBuffers(eParticleBuffer::kData);
}

void	ModuleSizeOverLifetime::init() {
	if (debug_)
		printf("%s\n", __FUNCTION_NAME__);
    queue_.getQueue().enqueueWriteBuffer(gpuBufferModuleParam_, CL_TRUE, 0, sizeof(ModuleParamSizeOverLifetime), &cpuBufferModuleParam_);
}

void	ModuleSizeOverLifetime::update(float deltaTime) {
	if (debug_)
		printf("%s\n", __FUNCTION_NAME__);

	queue_.getQueue().enqueueWriteBuffer(gpuBufferModuleParam_, CL_TRUE, 0, sizeof(ModuleParamSizeOverLifetime), &cpuBufferModuleParam_);
	kernelUpdate_.beginAndSetUpdatedArgs(gpuBufferModuleParam_);

    OpenCGL::RunKernelWithMem(queue_.getQueue(), kernelUpdate_, emitter_.getParticleOCGL_BufferData().mem, cl::NullRange, cl::NDRange(nbParticleMax_));
}

void    ModuleSizeOverLifetime::reload()
{
	if (debug_)
		printf("%s\n", __FUNCTION_NAME__);
    gpuBufferModuleParam_ = cl::Buffer(ClContext::Get().context, CL_MEM_READ_WRITE, sizeof(ModuleParamSizeOverLifetime));
    init();
}

float &ModuleSizeOverLifetime::getSizeMin() {
    return cpuBufferModuleParam_.size.rangeMin;
}
float &ModuleSizeOverLifetime::getSizeMax() {
    return cpuBufferModuleParam_.size.rangeMax;
}


void ModuleSizeOverLifetime::setSizeMin(float min) {
    cpuBufferModuleParam_.size.rangeMin = min;
}

void ModuleSizeOverLifetime::setSizeMax(float max) {
    cpuBufferModuleParam_.size.rangeMax = max;
}