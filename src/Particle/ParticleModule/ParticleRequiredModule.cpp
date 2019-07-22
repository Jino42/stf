#include "ParticleRequiredModule.hpp"
#include "Particle/PaticleEmitter/AParticleEmitter.hpp"
#include <iostream>
#include "Particle/ParticleData.hpp"
#include "Engine/Display/DisplayWindow.hpp"
#include "Particle/ParticleSystem.hpp"
#include "Cl/ClProgram.hpp"
#include "OpenCGL_Tools.hpp"
#include "Engine/Random.hpp"
#include "NTL_Debug.hpp"
#include "cl_type.hpp"

ParticleRequiredModule::ParticleRequiredModule(AParticleEmitter &emitter) :
AParticleModule(emitter),
bufferModuleParams_(ClContext::Get().context, CL_MEM_WRITE_ONLY, sizeof(ModuleRequiredParams))
{
	ClProgram::Get().addProgram(pathKernel_ / "RequiredModule.cl");
}


void	ParticleRequiredModule::init() {
	if (debug_)
		printf("%s\n", __FUNCTION_NAME__);
	cl::Kernel &kernel = ClProgram::Get().getKernel("RequiredInitialisation");

    queue_.getQueue().enqueueWriteBuffer(bufferModuleParams_, CL_TRUE, 0, sizeof(ModuleRequiredParams), &moduleRequiredParams_);
    kernel.setArg(0, bufferModuleParams_);
	kernel.setArg(1, emitter_.getDeviceBuffer().mem);
	kernel.setArg(2, glmVec3toClFloat3(emitter_.getSystem().getPosition()));
    kernel.setArg(3, Random::Get().getRandomSeed());

	OpenCGL::RunKernelWithMem(queue_.getQueue(), kernel, emitter_.getDeviceBuffer().mem, cl::NullRange, cl::NDRange(nbParticleMax_));

	printStructSizeCPU();
	printStructSizeGPU(emitter_, queue_.getQueue());
}
void	ParticleRequiredModule::update(float deltaTime) {
	if (debug_)
		printf("%s\n", __FUNCTION_NAME__);
	cl::Kernel &kernel = ClProgram::Get().getKernel("RequiredUpdate");

	unsigned int flag = KeyStateManager::Get().getKey(GLFW_KEY_M) == KeyState::kDown ? 1 : 0;
	if (flag) {
		init();
	}

	kernel.setArg(0, emitter_.getDeviceBuffer().mem);
	kernel.setArg(1, deltaTime);
    OpenCGL::RunKernelWithMem(queue_.getQueue(), kernel, emitter_.getDeviceBuffer().mem, cl::NullRange, cl::NDRange(nbParticleMax_));
}

void    ParticleRequiredModule::reload()
{
	if (debug_)
		printf("%s\n", __FUNCTION_NAME__);
    bufferModuleParams_ = cl::Buffer(ClContext::Get().context, CL_MEM_WRITE_ONLY, sizeof(ModuleRequiredParams));
    init();
}