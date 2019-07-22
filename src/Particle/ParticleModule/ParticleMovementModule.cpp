#include "ParticleMovementModule.hpp"
#include "Particle/PaticleEmitter/AParticleEmitter.hpp"
#include <iostream>
#include <Engine/ModelEngine/MainGraphicExtendModel.hpp>
#include "Particle/ParticleData.hpp"
#include "Particle/ParticleSystem.hpp"
#include "Cl/ClProgram.hpp"
#include "OpenCGL_Tools.hpp"
#include "Engine/Random.hpp"
#include "cl_type.hpp"

ParticleMovementModule::ParticleMovementModule(AParticleEmitter &emitter) :
		AParticleModule(emitter),

		buffer_(ClContext::Get().context, CL_MEM_WRITE_ONLY, nbParticleMax_ * sizeof(ParticleMovementModuleData))
{
	ClProgram::Get().addProgram(pathKernel_ / "Movement.cl");
}


void	ParticleMovementModule::init() {
	if (debug_)
		printf("%s\n", __FUNCTION_NAME__);
}
void	ParticleMovementModule::update(float deltaTime) {
	if (debug_)
		printf("%s\n", __FUNCTION_NAME__);
	cl::Kernel &kernel = ClProgram::Get().getKernel("movement");

	kernel.setArg(0, emitter_.getDeviceBuffer().mem);
	kernel.setArg(1, buffer_);
	kernel.setArg(2, deltaTime);
	glm::vec3 attractorPosition = MainGraphicExtendModel::Get().attractorPoint;
	kernel.setArg(3, glmVec3toClFloat3(MainGraphicExtendModel::Get().attractorPoint));

    OpenCGL::RunKernelWithMem(queue_.getQueue(), kernel, emitter_.getDeviceBuffer().mem, cl::NullRange, cl::NDRange(nbParticleMax_));
}

void	ParticleMovementModule::spawn(unsigned int nbToSpawn, unsigned int at) {
	if (debug_)
		printf("%s\n", __FUNCTION_NAME__);
	cl::Kernel &kernel = ClProgram::Get().getKernel("spawnMovementRandom");

	kernel.setArg(0, emitter_.getDeviceBuffer().mem);
	kernel.setArg(1, buffer_);
	kernel.setArg(2, Random::Get().getRandomSeed());

    OpenCGL::RunKernelWithMem(queue_.getQueue(), kernel, emitter_.getDeviceBuffer().mem, cl::NDRange(at), cl::NDRange(nbToSpawn));
}

void    ParticleMovementModule::reload()
{
	if (debug_)
		printf("%s\n", __FUNCTION_NAME__);
    buffer_ = cl::Buffer(ClContext::Get().context, CL_MEM_WRITE_ONLY, nbParticleMax_ * sizeof(ParticleMovementModuleData));
    init();
}