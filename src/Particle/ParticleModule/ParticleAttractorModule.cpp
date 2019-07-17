#include "ParticleAttractorModule.hpp"
#include "Particle/PaticleEmitter/ParticleEmitterMesh.hpp"
#include <iostream>
#include <Engine/ModelEngine/MainGraphicExtendModel.hpp>
#include "Cl/ClProgram.hpp"
#include "OpenCGL_Tools.hpp"
#include "cl_type.hpp"
#include <Engine/Display/DisplayWindow.hpp>

ParticleAttractorModule::ParticleAttractorModule(AParticleEmitter &emitter) :
		AParticleModule(emitter),

		buffer_(ClContext::Get().context, CL_MEM_WRITE_ONLY, nbParticleMax_ * sizeof(ParticleAttractorModuleData))
{
	ClProgram::Get().addProgram(pathKernel_ / "Attractor.cl");
}

void	ParticleAttractorModule::init() {
	if (debug_)
		printf("%s\n", __FUNCTION_NAME__);
}

void	ParticleAttractorModule::update(float deltaTime) {
	if (debug_)
		printf("%s\n", __FUNCTION_NAME__);
	cl::Kernel &kernel = ClProgram::Get().getKernel("attractor");

	glm::vec3 attractorPosition = MainGraphicExtendModel::Get().attractorPoint;
	unsigned int flag = KeyStateManager::Get().getKey(GLFW_KEY_N) ? 1 : 0;

	kernel.setArg(0, emitter_.getDeviceBuffer().mem);
	kernel.setArg(1, flag);
	kernel.setArg(2,  glmVec3toClFloat3(attractorPosition));
	kernel.setArg(3, deltaTime);

    OpenCGL::RunKernelWithMem(queue_.getQueue(), kernel, emitter_.getDeviceBuffer().mem, cl::NullRange, cl::NDRange(nbParticleMax_));
}

void    ParticleAttractorModule::reload()
{
	if (debug_)
		printf("%s\n", __FUNCTION_NAME__);
    buffer_ = cl::Buffer(ClContext::Get().context, CL_MEM_WRITE_ONLY, nbParticleMax_ * sizeof(ParticleAttractorModuleData));
    init();
}