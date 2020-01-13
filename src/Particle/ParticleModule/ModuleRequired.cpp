#include "ModuleRequired.hpp"
#include "Cl/ClKernel.hpp"
#include "Cl/ClProgram.hpp"
#include "Engine/Display/DisplayWindow.hpp"
#include "Engine/Random.hpp"
#include "Enum/eParticleBuffer.hpp"
#include "OpenCGL_Tools.hpp"
#include "Particle/ParticleData.hpp"
#include "Particle/ParticleSystem.hpp"
#include "Particle/PaticleEmitter/AParticleEmitter.hpp"
#include "cl_type.hpp"
#include <iostream>

ModuleRequired::ModuleRequired(AParticleEmitter &emitter)
    : AParticleModule(emitter) {
    gpuBufferModuleParam_ = emitter_.addClBuffer<ModuleParamRequired>();
    *gpuBufferModuleParam_ = cl::Buffer(ClContext::Get().context, CL_MEM_WRITE_ONLY, sizeof(ModuleParamRequired));

    ClProgram::Get().addProgram(pathKernel_ / "RequiredModule.cl");

    kernelInit_.setKernel(emitter_, "RequiredInitialisation");
    kernelInit_.setArgsGPUBuffers(eParticleBuffer::kAllBuffers);

    kernelUpdate_.setKernel(emitter_, "RequiredUpdate");
    kernelUpdate_.setArgsGPUBuffers(eParticleBuffer::kAllBuffers);
}

void ModuleRequired::init() {
    if (debug_)
        printf("%s\n", __FUNCTION_NAME__);

    queue_.getQueue().enqueueWriteBuffer(*gpuBufferModuleParam_, CL_TRUE, 0, sizeof(ModuleParamRequired), &cpuBufferModuleParam_);

    assert(!kernelInit_.beginAndSetUpdatedArgs(
        *gpuBufferModuleParam_,
        glmVec3toClFloat3(emitter_.getSystem().getPosition()),
        Random::Get().getRandomSeed(),
        emitter_.getNbParticleMax()));

    std::vector<cl::Memory> cl_vbos;
    cl_vbos.push_back(emitter_.getParticleOCGL_BufferData().mem);

    emitter_.indexSub_[0] = 0;
    emitter_.indexSub_[1] = 0;
    emitter_.indexSub_[2] = nbParticleMax_;

    queue_.getQueue().enqueueWriteBuffer(emitter_.gpuBufferParticles_SubLength_, CL_TRUE, 0, sizeof(int) * 3, &emitter_.indexSub_);
    OpenCGL::RunKernelWithMem(queue_.getQueue(), kernelInit_, cl_vbos, cl::NullRange, cl::NDRange(nbParticleMax_));
    queue_.getQueue().enqueueReadBuffer(emitter_.gpuBufferParticles_SubLength_, CL_TRUE, 0, sizeof(int) * 3, &emitter_.indexSub_);
    std::cout << emitter_.getNbParticleMax() << std::endl;
}

void ModuleRequired::update(float deltaTime) {
    if (debug_)
        printf("%s\n", __FUNCTION_NAME__);

    unsigned int flag = KeyStateManager::Get().getKey(GLFW_KEY_M) == KeyState::kDown ? 1 : 0;
    if (flag) {
        init();
    }

    //std::cout << "-------- Clean Alive" << std::endl;
    //printSubArrayParticle(emitter_, queue_.getQueue());
    //std::cout << "xxxxxxxx Clean Alive" << std::endl;

    assert(!kernelUpdate_.beginAndSetUpdatedArgs(deltaTime));

    std::vector<cl::Memory> cl_vbos;
    cl_vbos.push_back(emitter_.getParticleOCGL_BufferData().mem);

    emitter_.indexSub_[0] = 0;
    queue_.getQueue().enqueueWriteBuffer(emitter_.gpuBufferParticles_SubLength_, CL_TRUE, 0, sizeof(int) * 3, &emitter_.indexSub_);
    OpenCGL::RunKernelWithMem(queue_.getQueue(), kernelUpdate_, cl_vbos, cl::NullRange, cl::NDRange(nbParticleMax_));
    queue_.getQueue().enqueueReadBuffer(emitter_.gpuBufferParticles_SubLength_, CL_TRUE, 0, sizeof(int) * 3, &emitter_.indexSub_);
    emitter_.indexSub_[1] = 0;

    //std::cout << "-------- RequiredUpdate" << std::endl;
    //printSubArrayParticle(emitter_, queue_.getQueue());
    //std::cout << "xxxxxxxx RequiredUpdate" << std::endl;
}

void ModuleRequired::reload() {
    if (debug_)
        printf("%s\n", __FUNCTION_NAME__);
    init();
}