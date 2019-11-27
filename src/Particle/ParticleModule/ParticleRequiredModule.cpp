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
#include "Cl/ClKernel.hpp"

ParticleRequiredModule::ParticleRequiredModule(AParticleEmitter &emitter) :
AParticleModule(emitter),
bufferModuleParams_(ClContext::Get().context, CL_MEM_WRITE_ONLY, sizeof(ModuleRequiredParams))
{
	ClProgram::Get().addProgram(pathKernel_ / "RequiredModule.cl");
}


void	ParticleRequiredModule::init() {
	if (debug_)
		printf("%s\n", __FUNCTION_NAME__);
	ClKernel kernel("RequiredInitialisation");

    queue_.getQueue().enqueueWriteBuffer(bufferModuleParams_, CL_TRUE, 0, sizeof(ModuleRequiredParams), &moduleRequiredParams_);

	kernel.setArgs(
			bufferModuleParams_,
			emitter_.getDeviceBuffer().mem,
			emitter_.deviceBufferAlive_,
			emitter_.deviceBufferAlive2_,
			emitter_.deviceBufferDeath_,
			emitter_.deviceBufferLengthSub_,

			glmVec3toClFloat3(emitter_.getSystem().getPosition()),
			Random::Get().getRandomSeed(),
			emitter_.getNbParticleMax()
			);

	std::vector<cl::Memory> cl_vbos;
	cl_vbos.push_back(emitter_.getDeviceBuffer().mem);

	emitter_.indexSub_[0] = 0;
	emitter_.indexSub_[1] = 0;
	emitter_.indexSub_[2] = nbParticleMax_;

	queue_.getQueue().enqueueWriteBuffer(emitter_.deviceBufferLengthSub_, CL_TRUE, 0, sizeof(int) * 3, &emitter_.indexSub_);
	OpenCGL::RunKernelWithMem(queue_.getQueue(), kernel, cl_vbos, cl::NullRange, cl::NDRange(nbParticleMax_));
	queue_.getQueue().enqueueReadBuffer(emitter_.deviceBufferLengthSub_, CL_TRUE, 0, sizeof(int) * 3, &emitter_.indexSub_);

	std::cout << emitter_.getNbParticleMax() << std::endl;

	printStructSizeCPU();
	printStructSizeGPU(emitter_, queue_.getQueue());



}

void	ParticleRequiredModule::update(float deltaTime) {
	if (debug_)
		printf("%s\n", __FUNCTION_NAME__);
	ClKernel kernel("RequiredUpdate");

	unsigned int flag = KeyStateManager::Get().getKey(GLFW_KEY_M) == KeyState::kDown ? 1 : 0;
	if (flag) {
		init();
	}

	{
		ClKernel kernelClean("CleanAlive");
		kernelClean.setArgs(emitter_.deviceBufferAlive_, emitter_.deviceBufferAlive2_, emitter_.deviceBufferLengthSub_);
		std::vector<cl::Memory> cl_vbos;
		queue_.getQueue().enqueueWriteBuffer(emitter_.deviceBufferLengthSub_, CL_TRUE, 0, sizeof(int) * 3, &emitter_.indexSub_);
		OpenCGL::RunKernelWithMem(queue_.getQueue(), kernelClean, cl_vbos, cl::NullRange, cl::NDRange(nbParticleMax_));
		queue_.getQueue().enqueueReadBuffer(emitter_.deviceBufferLengthSub_, CL_TRUE, 0, sizeof(int) * 3, &emitter_.indexSub_);
	}
	std::cout << "-------- Clean Alive" << std::endl;
	printSubArrayParticle(emitter_, queue_.getQueue());
	std::cout << "xxxxxxxx Clean Alive" << std::endl;

	kernel.setArgs(emitter_.getDeviceBuffer().mem,
			emitter_.deviceBufferAlive_,
			emitter_.deviceBufferAlive2_,
			emitter_.deviceBufferDeath_,
			emitter_.deviceBufferLengthSub_,
			deltaTime);

	std::vector<cl::Memory> cl_vbos;
	cl_vbos.push_back(emitter_.getDeviceBuffer().mem);

	queue_.getQueue().enqueueWriteBuffer(emitter_.deviceBufferLengthSub_, CL_TRUE, 0, sizeof(int) * 3, &emitter_.indexSub_);
	OpenCGL::RunKernelWithMem(queue_.getQueue(), kernel, cl_vbos, cl::NullRange, cl::NDRange(nbParticleMax_));
	queue_.getQueue().enqueueReadBuffer(emitter_.deviceBufferLengthSub_, CL_TRUE, 0, sizeof(int) * 3, &emitter_.indexSub_);

	std::cout << "-------- RequiredUpdate" << std::endl;
	printSubArrayParticle(emitter_, queue_.getQueue());
	std::cout << "xxxxxxxx RequiredUpdate" << std::endl;

	/*
	static int count = 0;
	count++;
	if (count == 5)
		exit(0);
	*/
}

void    ParticleRequiredModule::reload()
{
	if (debug_)
		printf("%s\n", __FUNCTION_NAME__);
    bufferModuleParams_ = cl::Buffer(ClContext::Get().context, CL_MEM_WRITE_ONLY, sizeof(ModuleRequiredParams));
    init();
}