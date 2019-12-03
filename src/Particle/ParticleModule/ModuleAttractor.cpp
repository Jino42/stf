#include "ModuleAttractor.hpp"
#include <iostream>
#include <Engine/ModelEngine/MainGraphicExtendModel.hpp>
#include "Cl/ClProgram.hpp"
#include "OpenCGL_Tools.hpp"
#include "cl_type.hpp"
#include <Engine/Display/DisplayWindow.hpp>
#include "Cl/ClKernel.hpp"
#include "Particle/PaticleEmitter/AParticleEmitter.hpp"

ModuleAttractor::ModuleAttractor(AParticleEmitter &emitter) :
		AParticleModule(emitter)
{
	ClProgram::Get().addProgram(pathKernel_ / "Attractor.cl");

	kernelUpdate_.setKernel(emitter_, "attractor");
	kernelUpdate_.setArgsGPUBuffers(eParticleBuffer::kData);
}

void	ModuleAttractor::init() {
	if (debug_)
		printf("%s\n", __FUNCTION_NAME__);
}

void	ModuleAttractor::update(float deltaTime) {
	if (debug_)
		printf("%s\n", __FUNCTION_NAME__);

	glm::vec3 attractorPosition = MainGraphicExtendModel::Get().attractorPoint;
	unsigned int flag = KeyStateManager::Get().getKey(GLFW_KEY_N) ? 1 : 0;

	kernelUpdate_.beginAndSetUpdatedArgs(flag,
			glmVec3toClFloat3(attractorPosition),
			deltaTime);

    OpenCGL::RunKernelWithMem(queue_.getQueue(), kernelUpdate_, emitter_.getParticleOCGL_BufferData().mem, cl::NullRange, cl::NDRange(nbParticleMax_));
}

void    ModuleAttractor::reload()
{
	if (debug_)
		printf("%s\n", __FUNCTION_NAME__);
    init();
}