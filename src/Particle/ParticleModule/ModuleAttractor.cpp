#include "ModuleAttractor.hpp"
#include "Cl/ClKernel.hpp"
#include "Cl/ClProgram.hpp"
#include "ModuleResolvePhysic.hpp"
#include "OpenCGL_Tools.hpp"
#include "Particle/PaticleEmitter/AParticleEmitter.hpp"
#include "cl_type.hpp"
#include <Engine/Display/DisplayWindow.hpp>
#include <Engine/ModelEngine/MainGraphicExtendModel.hpp>
#include <iostream>

ModuleAttractor::ModuleAttractor(AParticleEmitter &emitter)
    : AParticleModule(emitter),
      doAttractor_(true) {
    ClProgram::Get().addProgram(pathKernel_ / "Attractor.cl");

    kernelUpdate_.setKernel(emitter_, "attractor");
    kernelUpdate_.setArgsGPUBuffers(eParticleBuffer::kData);
}

void ModuleAttractor::init() {
    if (debug_)
        printf("%s\n", __FUNCTION_NAME__);
    gpuBufferParticles_Physic_ = emitter_.getClBuffer<ParticleDataPhysic>();
}

void ModuleAttractor::update(float deltaTime) {
    if (debug_)
        printf("%s\n", __FUNCTION_NAME__);

    glm::vec3 attractorPosition = MainGraphicExtendModel::Get().attractorPoint;
    unsigned int flag = KeyStateManager::Get().getKey(GLFW_KEY_N) ? 1 : 0;

    kernelUpdate_.beginAndSetUpdatedArgs(
        *gpuBufferParticles_Physic_,
        flag,
        glmVec3toClFloat3(attractorPosition),
        deltaTime);

    OpenCGL::RunKernelWithMem(queue_.getQueue(), kernelUpdate_, emitter_.getParticleOCGL_BufferData().mem, cl::NullRange, cl::NDRange(nbParticleMax_));
}

void ModuleAttractor::reload() {
    if (debug_)
        printf("%s\n", __FUNCTION_NAME__);
    init();
}