#include "Cl/ClKernel.hpp"
#include "Cl/ClProgram.hpp"
#include "ModulePhysicCollisionSD.hpp"
#include "ModuleResolvePhysic.hpp"
#include "OpenCGL_Tools.hpp"
#include "Particle/PaticleEmitter/AParticleEmitter.hpp"
#include "cl_type.hpp"
#include <iostream>

ModulePhysicCollisionSD::ModulePhysicCollisionSD(AParticleEmitter &emitter)
    : AParticleModule(emitter),
      doAttractor_(true) {
    gpuBufferModuleParam_ = emitter_.addClBuffer<ModuleParamPhysicGravity>();
    *gpuBufferModuleParam_ = cl::Buffer(ClContext::Get().context, CL_MEM_WRITE_ONLY, sizeof(ModuleParamPhysicGravity));

    ClProgram::Get().addProgram(pathKernel_ / "PhysicGravity.cl");

    kernelUpdate_.setKernel(emitter_, "PhysicGravityUpdate");
}

void ModulePhysicCollisionSD::init() {
    if (debug_)
        printf("%s\n", __FUNCTION_NAME__);

    queue_.getQueue().enqueueWriteBuffer(*gpuBufferModuleParam_, CL_TRUE, 0, sizeof(ModuleParamPhysicGravity), &cpuBufferModuleParam_);

    gpuBufferParticles_Physic_ = emitter_.getClBuffer<ParticleDataPhysic>();
}

void ModulePhysicCollisionSD::update(float deltaTime) {
    if (debug_)
        printf("%s\n", __FUNCTION_NAME__);

    kernelUpdate_.beginAndSetUpdatedArgs(*gpuBufferParticles_Physic_, *gpuBufferModuleParam_);

    OpenCGL::RunKernelWithMem(queue_.getQueue(), kernelUpdate_, emitter_.getParticleOCGL_BufferData().mem, cl::NullRange, cl::NDRange(nbParticleMax_));
}

void ModulePhysicCollisionSD::reload() {
    if (debug_)
        printf("%s\n", __FUNCTION_NAME__);
    init();
}

ModuleParamPhysicGravity &ModulePhysicCollisionSD::getCpuModuleParam() {
    return cpuBufferModuleParam_;
}