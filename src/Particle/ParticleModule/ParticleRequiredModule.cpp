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
#include "Enum/eParticleBuffer.hpp"

ParticleRequiredModule::ParticleRequiredModule(AParticleEmitter &emitter) :
AParticleModule(emitter),
bufferModuleParams_(ClContext::Get().context, CL_MEM_WRITE_ONLY, sizeof(ModuleRequiredParams))
{
	ClProgram::Get().addProgram(pathKernel_ / "RequiredModule.cl");


	kernelInit_.setKernel(emitter_, "RequiredInitialisation");
	kernelInit_.setArgsGPUBuffers(eParticleBuffer::kAllBuffers);

	kernelUpdate_.setKernel(emitter_, "RequiredUpdate");
	kernelUpdate_.setArgsGPUBuffers(eParticleBuffer::kAllBuffers);

	kernelClean_.setKernel(emitter_, "CleanAlive");
	kernelClean_.setArgsGPUBuffers(eParticleBuffer::kAlive | eParticleBuffer::kSpawned | eParticleBuffer::kSubIndex);
}


void	ParticleRequiredModule::init() {
	if (debug_)
		printf("%s\n", __FUNCTION_NAME__);

    queue_.getQueue().enqueueWriteBuffer(bufferModuleParams_, CL_TRUE, 0, sizeof(ModuleRequiredParams), &moduleRequiredParams_);

	assert(!kernelInit_.beginAndSetUpdatedArgs(
			bufferModuleParams_,
			glmVec3toClFloat3(emitter_.getSystem().getPosition()),
			Random::Get().getRandomSeed(),
			emitter_.getNbParticleMax()
			));

	std::vector<cl::Memory> cl_vbos;
	cl_vbos.push_back(emitter_.getParticleOCGL_BufferData().mem);

	emitter_.indexSub_[0] = 0;
	emitter_.indexSub_[1] = 0;
	emitter_.indexSub_[2] = nbParticleMax_;

	queue_.getQueue().enqueueWriteBuffer(emitter_.particleSubBuffersLength_, CL_TRUE, 0, sizeof(int) * 3, &emitter_.indexSub_);
	OpenCGL::RunKernelWithMem(queue_.getQueue(), kernelInit_, cl_vbos, cl::NullRange, cl::NDRange(nbParticleMax_));
	queue_.getQueue().enqueueReadBuffer(emitter_.particleSubBuffersLength_, CL_TRUE, 0, sizeof(int) * 3, &emitter_.indexSub_);
	std::cout << emitter_.getNbParticleMax() << std::endl;

	printStructSizeCPU();
	printStructSizeGPU(emitter_, queue_.getQueue());



}

void	ParticleRequiredModule::update(float deltaTime) {
	if (debug_)
		printf("%s\n", __FUNCTION_NAME__);

	unsigned int flag = KeyStateManager::Get().getKey(GLFW_KEY_M) == KeyState::kDown ? 1 : 0;
	if (flag) {
		init();
	}

	{
		kernelClean_.beginAndSetUpdatedArgs();
		std::vector<cl::Memory> cl_vbos;
		queue_.getQueue().enqueueWriteBuffer(emitter_.particleSubBuffersLength_, CL_TRUE, 0, sizeof(int) * 3, &emitter_.indexSub_);
		OpenCGL::RunKernelWithMem(queue_.getQueue(), kernelClean_, cl_vbos, cl::NullRange, cl::NDRange(nbParticleMax_));
		queue_.getQueue().enqueueReadBuffer(emitter_.particleSubBuffersLength_, CL_TRUE, 0, sizeof(int) * 3, &emitter_.indexSub_);
	}
	std::cout << "-------- Clean Alive" << std::endl;
	printSubArrayParticle(emitter_, queue_.getQueue());
	std::cout << "xxxxxxxx Clean Alive" << std::endl;

	assert(!kernelUpdate_.beginAndSetUpdatedArgs(deltaTime));

	std::vector<cl::Memory> cl_vbos;
	cl_vbos.push_back(emitter_.getParticleOCGL_BufferData().mem);

	queue_.getQueue().enqueueWriteBuffer(emitter_.particleSubBuffersLength_, CL_TRUE, 0, sizeof(int) * 3, &emitter_.indexSub_);
	OpenCGL::RunKernelWithMem(queue_.getQueue(), kernelUpdate_, cl_vbos, cl::NullRange, cl::NDRange(nbParticleMax_));
	queue_.getQueue().enqueueReadBuffer(emitter_.particleSubBuffersLength_, CL_TRUE, 0, sizeof(int) * 3, &emitter_.indexSub_);

	std::cout << "-------- RequiredUpdate" << std::endl;
	printSubArrayParticle(emitter_, queue_.getQueue());
	std::cout << "xxxxxxxx RequiredUpdate" << std::endl;
}

void    ParticleRequiredModule::reload()
{
	if (debug_)
		printf("%s\n", __FUNCTION_NAME__);
    bufferModuleParams_ = cl::Buffer(ClContext::Get().context, CL_MEM_WRITE_ONLY, sizeof(ModuleRequiredParams));
    init();
}