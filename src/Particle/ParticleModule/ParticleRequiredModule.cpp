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
	kernel.setArg(2, emitter_.deviceBufferAlive_.mem);
	kernel.setArg(3, emitter_.deviceBufferAlive2_.mem);
	kernel.setArg(4, emitter_.deviceBufferDeath_.mem);
	kernel.setArg(5, emitter_.deviceBufferLengthSub_);

	kernel.setArg(6, glmVec3toClFloat3(emitter_.getSystem().getPosition()));
    kernel.setArg(7, Random::Get().getRandomSeed());
	kernel.setArg(8, emitter_.getNbParticleMax());

	std::vector<cl::Memory> cl_vbos;
	cl_vbos.push_back(emitter_.getDeviceBuffer().mem);
	cl_vbos.push_back(emitter_.deviceBufferAlive_.mem);
	cl_vbos.push_back(emitter_.deviceBufferAlive2_.mem);
	cl_vbos.push_back(emitter_.deviceBufferDeath_.mem);

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
	cl::Kernel &kernel = ClProgram::Get().getKernel("RequiredUpdate");

	unsigned int flag = KeyStateManager::Get().getKey(GLFW_KEY_M) == KeyState::kDown ? 1 : 0;
	if (flag) {
		init();
	}

	{
		cl::Kernel &kernelClean = ClProgram::Get().getKernel("CleanAlive");
		kernelClean.setArg(0, emitter_.deviceBufferAlive_.mem);
		kernelClean.setArg(1, emitter_.deviceBufferAlive2_.mem);
		kernelClean.setArg(2, emitter_.deviceBufferLengthSub_);
		std::vector<cl::Memory> cl_vbos;
		cl_vbos.push_back(emitter_.deviceBufferAlive_.mem);
		cl_vbos.push_back(emitter_.deviceBufferAlive2_.mem);
		queue_.getQueue().enqueueWriteBuffer(emitter_.deviceBufferLengthSub_, CL_TRUE, 0, sizeof(int) * 3, &emitter_.indexSub_);
		OpenCGL::RunKernelWithMem(queue_.getQueue(), kernelClean, cl_vbos, cl::NullRange, cl::NDRange(nbParticleMax_));
		queue_.getQueue().enqueueReadBuffer(emitter_.deviceBufferLengthSub_, CL_TRUE, 0, sizeof(int) * 3, &emitter_.indexSub_);
	}
	std::cout << "-------- Clean Alive" << std::endl;
	printSubArrayParticle(emitter_, queue_.getQueue());
	std::cout << "xxxxxxxx Clean Alive" << std::endl;

	kernel.setArg(0, emitter_.getDeviceBuffer().mem);
	kernel.setArg(1, emitter_.deviceBufferAlive_.mem);
	kernel.setArg(2, emitter_.deviceBufferAlive2_.mem);
	kernel.setArg(3, emitter_.deviceBufferDeath_.mem);
	kernel.setArg(4, emitter_.deviceBufferLengthSub_);
	kernel.setArg(5, deltaTime);

	std::vector<cl::Memory> cl_vbos;
	cl_vbos.push_back(emitter_.getDeviceBuffer().mem);
	cl_vbos.push_back(emitter_.deviceBufferAlive_.mem);
	cl_vbos.push_back(emitter_.deviceBufferAlive2_.mem);
	cl_vbos.push_back(emitter_.deviceBufferDeath_.mem);

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