#include "DebugClPrint.hpp"
#include "PathManager.hpp"
#include "Engine/Shape/AShape.hpp"

#include <Particle/ParticleModule/ModuleRequired.hpp>
#include <Particle/ParticleModule/ModuleAttractor.hpp>
#include <Particle/ParticleModule/ModuleMovement.hpp>
#include <Particle/ParticleModule/ModuleColor.hpp>
#include <Particle/ParticleModule/ParticleSpawnModule.hpp>
#include <Particle/ParticleModule/ModuleSizeOverLifetime.hpp>
#include <Particle/PaticleEmitter/ParticleEmitterSprite.hpp>
#include <Particle/ParticleData.hpp>

DebugClPrint &DebugClPrint::Get() {
    if (!instance_)
        instance_ = std::make_unique<DebugClPrint>();
    return *instance_;
}

DebugClPrint::DebugClPrint() {

    ClProgram::Get().addProgram(PathManager::Get().getPath("particleKernels") / "Print.cl");
    kernelPrintBasic_ = cl::Kernel(ClProgram::Get().getKernel("PrintStructSize"));
    kernelPrintShape_ = cl::Kernel(ClProgram::Get().getKernel("PrintShapeSize"));
}

void DebugClPrint::printAllStructSize() {
    ClError err;

    printStructSizeCPU();
    err.err = queue_.getQueue().enqueueNDRangeKernel(kernelPrintBasic_, cl::NullRange, cl::NDRange(1));
    err.clCheckError();
    queue_.getQueue().finish();

    AShape::printShapeStructSize();
    err.err = queue_.getQueue().enqueueNDRangeKernel(kernelPrintShape_, cl::NullRange, cl::NDRange(1));
    err.clCheckError();
    queue_.getQueue().finish();
}

void DebugClPrint::printSubArrayParticle(AParticleEmitter &emitter, cl::CommandQueue &queue) {

    ClKernel kernel;
    ClError err;

    kernel.setKernel(emitter, "PrintSubArrayParticle");
    kernel.setArgsGPUBuffers(eParticleBuffer::kAllSubBuffers);

    kernel.beginAndSetUpdatedArgs(emitter.getNbParticleMax());

    queue.enqueueWriteBuffer(emitter.getParticleSubBuffersLength(), CL_TRUE, 0, sizeof(int) * 3, &emitter.indexSub_);
    err.err = queue.enqueueNDRangeKernel(kernel.getKernel(), cl::NullRange, cl::NDRange(1), cl::NullRange);
    err.clCheckError();
    queue.finish();
    queue.enqueueReadBuffer(emitter.getParticleSubBuffersLength(), CL_TRUE, 0, sizeof(int) * 3, &emitter.indexSub_);

    printf("CPU INDEX : %i %i %i\n", emitter.indexSub_[0], emitter.indexSub_[1], emitter.indexSub_[2]);
}

void DebugClPrint::printStructSizeCPU() {
    printf(">>> CPU SIZE STRUCT <<< \n");
    printf("[%5li] ModuleParamRequired \n", sizeof(ModuleParamRequired));
    printf("[%5li] ModuleParamSpawn \n", sizeof(ModuleParamSpawn));
    printf("[%5li] ModuleParamSizeOverLifetime \n", sizeof(ModuleParamSizeOverLifetime));
    printf("[%5li] Rangef \n", sizeof(Rangef));
    printf("[%5li] Rangei \n", sizeof(Rangei));
    printf("[%5li] ParticleData \n", sizeof(ParticleData));
    printf("[%5li] ParticleDataSprite \n", sizeof(ParticleDataSprite));
    printf("[%5li] ParticleDataMovement \n", sizeof(ParticleDataMovement));
}

std::unique_ptr<DebugClPrint> DebugClPrint::instance_ = nullptr;
