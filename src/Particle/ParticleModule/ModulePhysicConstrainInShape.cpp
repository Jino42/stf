#include "Cl/ClKernel.hpp"
#include "Cl/ClProgram.hpp"
#include "ModulePhysicConstrainInShape.hpp"
#include "ModuleResolvePhysic.hpp"
#include "OpenCGL_Tools.hpp"
#include "Particle/PaticleEmitter/AParticleEmitter.hpp"
#include "cl_type.hpp"
#include <iostream>

ModulePhysicConstrainInShape::ModulePhysicConstrainInShape(AParticleEmitter &emitter)
    : AParticleModule(emitter),
      doAttractor_(true) {
    gpuBufferModuleParam_ = emitter_.addClBuffer<ModuleParamPhysicConstrainInShape>();
    *gpuBufferModuleParam_ = cl::Buffer(ClContext::Get().context, CL_MEM_WRITE_ONLY, sizeof(ModuleParamPhysicConstrainInShape));

    cpuBufferModuleParam_.gravity.x = 0.0f;
    cpuBufferModuleParam_.gravity.y = -0.1f;
    cpuBufferModuleParam_.gravity.z = 0.0f;

    ClProgram::Get().addProgram(pathKernel_ / "PhysicConstrainInShape.cl");
    kernelUpdate_.setKernel(emitter_, "PhysicConstrainInShapeUpdate");
}

void ModulePhysicConstrainInShape::init() {
    if (debug_)
        printf("%s\n", __FUNCTION_NAME__);

    queue_.getQueue().enqueueWriteBuffer(*gpuBufferModuleParam_, CL_TRUE, 0, sizeof(ModuleParamPhysicConstrainInShape), &cpuBufferModuleParam_);

    gpuBufferParticles_Physic_ = emitter_.getClBuffer<ParticleDataPhysic>();
}

void ModulePhysicConstrainInShape::update(float deltaTime) {
    if (debug_)
        printf("%s\n", __FUNCTION_NAME__);

    kernelUpdate_.beginAndSetUpdatedArgs(*gpuBufferParticles_Physic_, *gpuBufferModuleParam_);

    OpenCGL::RunKernelWithMem(queue_.getQueue(), kernelUpdate_, emitter_.getParticleOCGL_BufferData().mem, cl::NullRange, cl::NDRange(nbParticleMax_));
}

void ModulePhysicConstrainInShape::reload() {
    if (debug_)
        printf("%s\n", __FUNCTION_NAME__);
    init();
}

ModuleParamPhysicConstrainInShape &ModulePhysicConstrainInShape::getCpuModuleParam() {
    return cpuBufferModuleParam_;
}