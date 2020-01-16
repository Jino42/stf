#include "ModulePhysicNoise.hpp"
#include "Cl/ClKernel.hpp"
#include "Cl/ClProgram.hpp"
#include "ModuleResolvePhysic.hpp"
#include "OpenCGL_Tools.hpp"
#include "Particle/PaticleEmitter/AParticleEmitter.hpp"
#include "cl_type.hpp"
#include <iostream>

ModulePhysicNoise::ModulePhysicNoise(AParticleEmitter &emitter)
    : AParticleModule(emitter) {
    gpuBufferModuleParam_ = emitter_.addClBuffer<ModuleParamPhysicNoise>();
    *gpuBufferModuleParam_ = cl::Buffer(ClContext::Get().context, CL_MEM_WRITE_ONLY, sizeof(ModuleParamPhysicNoise));
    cpuBufferModuleParam_.gravity.x = 0.0f;
    cpuBufferModuleParam_.gravity.y = -0.1f;
    cpuBufferModuleParam_.gravity.z = 0.0f;

    ClProgram::Get().addProgram(pathKernel_ / "PhysicNoise.cl");

    kernelUpdate_.setKernel(emitter_, "PhysicNoiseUpdate");
}

void ModulePhysicNoise::init() {
    if (debug_)
        printf("%s\n", __FUNCTION_NAME__);

    queue_.getQueue().enqueueWriteBuffer(*gpuBufferModuleParam_, CL_TRUE, 0, sizeof(ModuleParamPhysicNoise), &cpuBufferModuleParam_);

    gpuBufferParticles_Physic_ = emitter_.getClBuffer<ParticleDataPhysic>();
}

void ModulePhysicNoise::update(float deltaTime) {
    if (!isActive_)
        return;
    if (debug_)
        printf("%s\n", __FUNCTION_NAME__);

    kernelUpdate_.beginAndSetUpdatedArgs(*gpuBufferParticles_Physic_, *gpuBufferModuleParam_);

    OpenCGL::RunKernelWithMem(queue_.getQueue(), kernelUpdate_, emitter_.getParticleOCGL_BufferData().mem, cl::NullRange, cl::NDRange(nbParticleMax_));
}

void ModulePhysicNoise::reload() {
    if (debug_)
        printf("%s\n", __FUNCTION_NAME__);
    init();
}

ModuleParamPhysicNoise &ModulePhysicNoise::getCpuModuleParam() {
    return cpuBufferModuleParam_;
}