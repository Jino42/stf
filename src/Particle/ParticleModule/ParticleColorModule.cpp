#include "ParticleColorModule.hpp"
#include "Particle/PaticleEmitter/ParticleEmitterMesh.hpp"
#include <iostream>
#include <Engine/ModelEngine/MainGraphicExtendModel.hpp>
#include "Cl/ClProgram.hpp"
#include "OpenCGL_Tools.hpp"
#include "cl_type.hpp"
#include "Cl/ClKernel.hpp"

ParticleColorModule::ParticleColorModule(AParticleEmitter &emitter) :
		AParticleModule(emitter)
{
	ClProgram::Get().addProgram(pathKernel_ / "Color.cl");

	kernelUpdate_.setKernel(emitter_, "color_radius_from_position");
	kernelUpdate_.setArgsGPUBuffers(eParticleBuffer::kData);
}


void	ParticleColorModule::init() {
	if (debug_)
		printf("%s\n", __FUNCTION_NAME__);
}

void	ParticleColorModule::update(float deltaTime) {
	if (debug_)
		printf("%s\n", __FUNCTION_NAME__);

	float radius = 200.f;
	kernelUpdate_.beginAndSetUpdatedArgs(glmVec3toClFloat3(MainGraphicExtendModel::Get().attractorPoint),
			radius,
			deltaTime);

	OpenCGL::RunKernelWithMem(queue_.getQueue(), kernelUpdate_, emitter_.getParticleOCGL_BufferData().mem, cl::NullRange, cl::NDRange(nbParticleMax_));
}

void    ParticleColorModule::reload()
{
	if (debug_)
		printf("%s\n", __FUNCTION_NAME__);
    init();
}