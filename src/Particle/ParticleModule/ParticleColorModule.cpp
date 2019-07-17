#include "ParticleColorModule.hpp"
#include "Particle/PaticleEmitter/ParticleEmitterMesh.hpp"
#include <iostream>
#include <Engine/ModelEngine/MainGraphicExtendModel.hpp>
#include "Cl/ClProgram.hpp"
#include "OpenCGL_Tools.hpp"
#include "cl_type.hpp"

ParticleColorModule::ParticleColorModule(AParticleEmitter &emitter) :
		AParticleModule(emitter),

		buffer_(ClContext::Get().context, CL_MEM_WRITE_ONLY, nbParticleMax_ * sizeof(ParticleColorModuleData))
{
	ClProgram::Get().addProgram(pathKernel_ / "Color.cl");
}


void	ParticleColorModule::init() {
	if (debug_)
		printf("%s\n", __FUNCTION_NAME__);
}

void	ParticleColorModule::update(float deltaTime) {
	if (debug_)
		printf("%s\n", __FUNCTION_NAME__);
	cl::Kernel &kernel = ClProgram::Get().getKernel("color_radius_from_position");

	kernel.setArg(0, emitter_.getDeviceBuffer().mem);
	kernel.setArg(1, glmVec3toClFloat3(MainGraphicExtendModel::Get().attractorPoint));
	float radius = 200.f;
	kernel.setArg(2, radius);
	kernel.setArg(3, deltaTime);

	OpenCGL::RunKernelWithMem(queue_.getQueue(), kernel, emitter_.getDeviceBuffer().mem, cl::NullRange, cl::NDRange(nbParticleMax_));
}

void    ParticleColorModule::reload()
{
	if (debug_)
		printf("%s\n", __FUNCTION_NAME__);
    buffer_ = cl::Buffer(ClContext::Get().context, CL_MEM_WRITE_ONLY, nbParticleMax_ * sizeof(ParticleColorModuleData));
    init();
}