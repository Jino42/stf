#include "ModuleResolvePhysic.hpp"
#include "Cl/ClKernel.hpp"
#include "Cl/ClProgram.hpp"
#include "OpenCGL_Tools.hpp"
#include "Particle/PaticleEmitter/AParticleEmitter.hpp"
#include "cl_type.hpp"
#include <iostream>

ModuleResolvePhysic::ModuleResolvePhysic(AParticleEmitter &emitter)
    : AParticleModule(emitter),
      doAttractor_(true) {
    gpuBufferParticles_Physic_ = emitter_.addClBuffer<ParticleDataPhysic>();
    *gpuBufferParticles_Physic_ = cl::Buffer(ClContext::Get().context, CL_MEM_WRITE_ONLY, sizeof(ParticleDataPhysic) * nbParticleMax_);

    ClProgram::Get().addProgram(pathKernel_ / "ResolvePhysic.cl");

    kernelInit_.setKernel(emitter_, "ResolvePhysicInitialisation");

    kernelUpdate_.setKernel(emitter_, "ResolvePhysicUpdate");
    kernelUpdate_.setArgsGPUBuffers(eParticleBuffer::kData);

    kernelSpawn_.setKernel(emitter_, "ResolvePhysicSpawn");
}

void ModuleResolvePhysic::init() {
    if (debug_)
        printf("%s\n", __FUNCTION_NAME__);

    kernelInit_.beginAndSetUpdatedArgs(*gpuBufferParticles_Physic_);
    OpenCGL::RunKernelWithMem(queue_.getQueue(), kernelInit_, emitter_.getParticleOCGL_BufferData().mem, cl::NullRange, cl::NDRange(nbParticleMax_));
}

void ModuleResolvePhysic::update(float deltaTime) {
    if (debug_)
        printf("%s\n", __FUNCTION_NAME__);

    kernelUpdate_.beginAndSetUpdatedArgs(*gpuBufferParticles_Physic_, deltaTime);

    OpenCGL::RunKernelWithMem(queue_.getQueue(), kernelUpdate_, emitter_.getParticleOCGL_BufferData().mem, cl::NullRange, cl::NDRange(nbParticleMax_));
}

void ModuleResolvePhysic::spawn(unsigned int nbToSpawn, unsigned int) {
    if (debug_)
        printf("%s\n", __FUNCTION_NAME__);

    kernelSpawn_.beginAndSetUpdatedArgs(*gpuBufferParticles_Physic_, emitter_.getParticleBufferSpawned());
    OpenCGL::RunKernelWithMem(queue_.getQueue(), kernelSpawn_, cl::NullRange, cl::NDRange(nbToSpawn));
}

void ModuleResolvePhysic::reload() {
    if (debug_)
        printf("%s\n", __FUNCTION_NAME__);
    init();
}