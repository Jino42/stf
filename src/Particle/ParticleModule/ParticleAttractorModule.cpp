#include "ParticleAttractorModule.hpp"
#include "Particle/PaticleEmitter/ParticleEmitterMesh.hpp"
#include <iostream>
#include <Engine/ModelEngine/MainGraphicExtendModel.hpp>
#include "Cl/ClProgram.hpp"
#include "OpenCGL_Tools.hpp"
#include "cl_type.hpp"
#include <Engine/Display/DisplayWindow.hpp>
#include "Cl/ClKernel.hpp"

ParticleAttractorModule::ParticleAttractorModule(AParticleEmitter &emitter) :
		AParticleModule(emitter),

		buffer_(ClContext::Get().context, CL_MEM_WRITE_ONLY, nbParticleMax_ * sizeof(ParticleAttractorModuleData))
{
	ClProgram::Get().addProgram(pathKernel_ / "Attractor.cl");

	kernelUpdate_.setKernel(emitter_, "attractor");
	kernelUpdate_.setArgsGPUBuffers(eParticleBuffer::kData);
}

void	ParticleAttractorModule::init() {
	if (debug_)
		printf("%s\n", __FUNCTION_NAME__);
}

void	ParticleAttractorModule::update(float deltaTime) {
	if (debug_)
		printf("%s\n", __FUNCTION_NAME__);

	glm::vec3 attractorPosition = MainGraphicExtendModel::Get().attractorPoint;
	unsigned int flag = KeyStateManager::Get().getKey(GLFW_KEY_N) ? 1 : 0;

	kernelUpdate_.beginAndSetUpdatedArgs(flag,
			glmVec3toClFloat3(attractorPosition),
			deltaTime);

    OpenCGL::RunKernelWithMem(queue_.getQueue(), kernelUpdate_, emitter_.getParticleOCGL_BufferData().mem, cl::NullRange, cl::NDRange(nbParticleMax_));
}

void    ParticleAttractorModule::reload()
{
	if (debug_)
		printf("%s\n", __FUNCTION_NAME__);
    buffer_ = cl::Buffer(ClContext::Get().context, CL_MEM_WRITE_ONLY, nbParticleMax_ * sizeof(ParticleAttractorModuleData));
    init();
}