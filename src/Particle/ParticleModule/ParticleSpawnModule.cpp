#include "ParticleSpawnModule.hpp"
#include "Cl/ClKernel.hpp"
#include "Cl/ClProgram.hpp"
#include "Engine/Display/DisplayWindow.hpp"
#include "Engine/Random.hpp"
#include "Engine/Time.hpp"
#include "Engine/TimerOnConstructOffDestruct.hpp"
#include "ModuleRequired.hpp"
#include "OpenCGL_Tools.hpp"
#include "Particle/ParticleData.hpp"
#include "Particle/ParticleSystem.hpp"
#include "Particle/PaticleEmitter/AParticleEmitter.hpp"
#include "cl_type.hpp"
#include <Gui/Gui.hpp>
#include <cmath>

ParticleSpawnModule::ParticleSpawnModule(AParticleEmitter &emitter)
    : AParticleModule(emitter),
      gpuBufferModuleParam_(ClContext::Get().context, CL_MEM_WRITE_ONLY, sizeof(ModuleParamSpawn)),
      spwaned_(0),
      nbParticlePerSec_(emitter_.getNbParticlePerSec()) {
    ClProgram::Get().addProgram(pathKernel_ / "Spawn.cl");

    kernelSpawn_.setKernel(emitter_, "spawnParticle");
    kernelSpawn_.setArgsGPUBuffers(eParticleBuffer::kAllBuffers | eParticleBuffer::kEmitterParam);

    cpuBufferModuleParam_.startLifeTime.isRange = true;
    cpuBufferModuleParam_.startLifeTime.rangeMin = 1.0f;
    cpuBufferModuleParam_.startLifeTime.rangeMax = 10.0f;

    cpuBufferModuleParam_.startSize.isRange = true;
    cpuBufferModuleParam_.startSize.rangeMin = 1.0f;
    cpuBufferModuleParam_.startSize.rangeMax = 2.0f;

    cpuBufferModuleParam_.startRotation.isRange = true;
    cpuBufferModuleParam_.startRotation.rangeMin = 0.0f;
    cpuBufferModuleParam_.startRotation.rangeMax = 100.0f;
}

void ParticleSpawnModule::init() {
    TimerOnConstructOffDestruct timer(__FUNCTION_NAME__);
}

void ParticleSpawnModule::update(float deltaTime) {
    TimerOnConstructOffDestruct timer(__FUNCTION_NAME__);

    int shouldBeAlreadySpawn = (int)(emitter_.getTimer().count() / (1000.f / nbParticlePerSec_));

    if (debug_)
        std::cout << shouldBeAlreadySpawn << " >=" << spwaned_ << std::endl;
    if (shouldBeAlreadySpawn < 0 || shouldBeAlreadySpawn < spwaned_)
        return;
    int shouldBeSpawn = shouldBeAlreadySpawn - spwaned_;

    if (debug_) {
        std::cout << "ShouldBeSpawned at this frame ||| " << shouldBeSpawn << std::endl;
        std::cout << "getNbParticleActive : " << emitter_.getNbParticleActive_() << std::endl;
    }

    if (shouldBeSpawn <= 0 || emitter_.getNbParticleActive_() >= nbParticleMax_)
        return;
    if (shouldBeSpawn + emitter_.getNbParticleActive_() > nbParticleMax_)
        shouldBeSpawn = nbParticleMax_ - emitter_.getNbParticleActive_();
    if (debug_)
        std::cout << "ShouldBeSpawned at this frame ||| " << shouldBeSpawn << std::endl;
    emitter_.setShouldBeToSpawn(shouldBeSpawn, emitter_.getNbParticleActive_());
}

void ParticleSpawnModule::spawn(unsigned int nbToSpawn, unsigned int at) {
    TimerOnConstructOffDestruct timer(__FUNCTION_NAME__);

    //std::cout << "BEFORE SPAWN" << std::endl;
    //printParticleArray();

    if (debug_)
        std::cout << "emitter_.getNbParticleActive_() : " << emitter_.getNbParticleActive_() << std::endl;

    queue_.getQueue().enqueueWriteBuffer(gpuBufferModuleParam_, CL_TRUE, 0, sizeof(ModuleParamSpawn),
                                         &cpuBufferModuleParam_);
    kernelSpawn_.beginAndSetUpdatedArgs(gpuBufferModuleParam_,
                                        Random::Get().getRandomSeed());

    std::vector<cl::Memory> cl_vbos;
    cl_vbos.push_back(emitter_.getParticleOCGL_BufferData().mem);

    {
        TimerOnConstructOffDestruct timerE("----------During");

        queue_.getQueue().enqueueWriteBuffer(emitter_.gpuBufferParticles_SubLength_, CL_TRUE, 0, sizeof(int) * 3, &emitter_.indexSub_);
        OpenCGL::RunKernelWithMem(queue_.getQueue(), kernelSpawn_, cl_vbos, cl::NullRange, cl::NDRange(nbToSpawn));
        queue_.getQueue().enqueueReadBuffer(emitter_.gpuBufferParticles_SubLength_, CL_TRUE, 0, sizeof(int) * 3, &emitter_.indexSub_);
        spwaned_ += nbToSpawn;
    }

    //std::cout << "-------- spawnParticle" << std::endl;
    //printSubArrayParticle(emitter_, queue_.getQueue());
    //std::cout << "xxxxxxxx spawnParticle" << std::endl;
}

void ParticleSpawnModule::reload() {
    TimerOnConstructOffDestruct timer(__FUNCTION_NAME__);
    gpuBufferModuleParam_ = cl::Buffer(ClContext::Get().context, CL_MEM_WRITE_ONLY, sizeof(ModuleParamSpawn));
    init();
    spwaned_ = 0;
}

void ParticleSpawnModule::setRate(unsigned int rate) {
    emitter_.setNbParticlePerSec(rate);
}

void ParticleSpawnModule::jsonParse(json &itModule) {

    if (itModule.find("options") != itModule.end()) {
        if (itModule["options"].find("rate") != itModule["options"].end()) {
            std::cout << "rate" << std::endl;
            json &rate = *itModule["options"].find("rate");
            std::cout << rate << std::endl;
            setRate(rate.get<unsigned int>());
        }
    }
}

void ParticleSpawnModule::gui() {
    static int speedDrag = 1;
    ImGui::DragInt("speedDrag", &speedDrag, 1, 0, 10000);
    ImGui::DragInt("rate", (int *)&nbParticlePerSec_, speedDrag, 0, 1000000);

    float speedRamge = 0.25f;

    ImGui::DragFloatRange2("LifeTime",
                           &cpuBufferModuleParam_.startLifeTime.rangeMin,
                           &cpuBufferModuleParam_.startLifeTime.rangeMax,
                           speedRamge, 0.1f, 100.0f, "Min[%.1f s]", "Max[%.1f s]");

    ImGui::DragFloatRange2("Size",
                           &cpuBufferModuleParam_.startSize.rangeMin,
                           &cpuBufferModuleParam_.startSize.rangeMax,
                           speedRamge, 0.0f, 100.0f, "Min[%.1f s]", "Max[%.1f s]");

    ImGui::DragFloatRange2("Rotation",
                           &cpuBufferModuleParam_.startRotation.rangeMin,
                           &cpuBufferModuleParam_.startRotation.rangeMax,
                           speedRamge, 0.0f, 100.0f, "Min[%.1f s]", "Max[%.1f s]");
}