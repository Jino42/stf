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
#include "Cl/ClKernel.hpp"

ParticleMovementModule::ParticleMovementModule(AParticleEmitter &emitter) :
		AParticleModule(emitter),

		gpuBufferParticles_Movement_(ClContext::Get().context, CL_MEM_WRITE_ONLY, nbParticleMax_ * sizeof(ParticleDataMovement))
{
	ClProgram::Get().addProgram(pathKernel_ / "Movement.cl");

	kernelUpdate_.setKernel(emitter_, "movement");
	kernelUpdate_.setArgsGPUBuffers(eParticleBuffer::kData);

	kernelSpawn_.setKernel(emitter_, "spawnMovementRandom");
	kernelSpawn_.setArgsGPUBuffers(eParticleBuffer::kData | eParticleBuffer::kSpawned);
}


void	ParticleMovementModule::init() {
	if (debug_)
		printf("%s\n", __FUNCTION_NAME__);
}

void	ParticleMovementModule::update(float deltaTime) {
	if (debug_)
		printf("%s\n", __FUNCTION_NAME__);

	glm::vec3 attractorPosition = MainGraphicExtendModel::Get().attractorPoint;
	kernelUpdate_.beginAndSetUpdatedArgs(gpuBufferParticles_Movement_,
			glmVec3toClFloat3(MainGraphicExtendModel::Get().attractorPoint));

    OpenCGL::RunKernelWithMem(queue_.getQueue(), kernelUpdate_, emitter_.getParticleOCGL_BufferData().mem, cl::NullRange, cl::NDRange(nbParticleMax_));
}

void	ParticleMovementModule::spawn(unsigned int nbToSpawn, unsigned int at) {
	if (debug_)
		printf("%s\n", __FUNCTION_NAME__);

	kernelSpawn_.beginAndSetUpdatedArgs(gpuBufferParticles_Movement_,
			Random::Get().getRandomSeed());

	std::vector<cl::Memory> cl_vbos;
	cl_vbos.push_back(emitter_.getParticleOCGL_BufferData().mem);

    OpenCGL::RunKernelWithMem(queue_.getQueue(), kernelSpawn_, cl_vbos, cl::NullRange, cl::NDRange(nbToSpawn));
}

void    ParticleMovementModule::reload()
{
	if (debug_)
		printf("%s\n", __FUNCTION_NAME__);
    gpuBufferParticles_Movement_ = cl::Buffer(ClContext::Get().context, CL_MEM_WRITE_ONLY, nbParticleMax_ * sizeof(ParticleDataMovement));
    init();
}