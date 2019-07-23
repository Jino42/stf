#include "ParticleSpawnModule.hpp"
#include <cmath>
#include "Particle/PaticleEmitter/AParticleEmitter.hpp"
#include "Particle/ParticleData.hpp"
#include "Engine/Display/DisplayWindow.hpp"
#include "Particle/ParticleSystem.hpp"
#include "Cl/ClProgram.hpp"
#include "Engine/Time.hpp"
#include "OpenCGL_Tools.hpp"
#include "ParticleRequiredModule.hpp"
#include "Engine/Random.hpp"
#include "cl_type.hpp"


ParticleSpawnModule::ParticleSpawnModule(AParticleEmitter &emitter) :
        AParticleModule(emitter),
        bufferModuleParams_(ClContext::Get().context, CL_MEM_WRITE_ONLY, sizeof(ModuleSpawnParams)),
        spwaned_(0)
{
    ClProgram::Get().addProgram(pathKernel_ / "Spawn.cl");
    moduleSpawnParams_.startLifeTime.isRange = true;
    moduleSpawnParams_.startLifeTime.rangeMin = 1.0f;
    moduleSpawnParams_.startLifeTime.rangeMax = 10.0f;

    moduleSpawnParams_.startSize.isRange = true;
    moduleSpawnParams_.startSize.rangeMin = 1.0f;
    moduleSpawnParams_.startSize.rangeMax = 2.0f;

    moduleSpawnParams_.startRotation.isRange = true;
    moduleSpawnParams_.startRotation.rangeMin = 0.0f;
    moduleSpawnParams_.startRotation.rangeMax = 100.0f;
}


void	ParticleSpawnModule::init() {
    if (debug_)
        printf("%s\n", __FUNCTION_NAME__);
}
void	ParticleSpawnModule::update(float deltaTime) {
    int shouldBeAlreadySpawn = (int)(emitter_.getTimer().count() / (1000.f / nbParticlePerSec_));

    std::cout << shouldBeAlreadySpawn << " >=" << spwaned_ << std::endl;
    if (shouldBeAlreadySpawn < 0 || shouldBeAlreadySpawn < spwaned_)
        return ;
    int shouldBeSpawn = shouldBeAlreadySpawn - spwaned_;
    std::cout << "ShouldBeSpawned at this frame ||| " << shouldBeSpawn << std::endl;
    std::cout << "getNbParticleActive : " << emitter_.getNbParticleActive_() << std::endl;
    if (shouldBeSpawn <= 0 || emitter_.getNbParticleActive_() >= nbParticleMax_)
        return ;
    if (shouldBeSpawn + emitter_.getNbParticleActive_() > nbParticleMax_)
        shouldBeSpawn = nbParticleMax_ - emitter_.getNbParticleActive_();
    std::cout << "ShouldBeSpawned at this frame ||| " << shouldBeSpawn << std::endl;
    emitter_.setShouldBeToSpawn(shouldBeSpawn, emitter_.getNbParticleActive_());
}

void	ParticleSpawnModule::spawn(unsigned int nbToSpawn, unsigned int at) {
    if (debug_)
        printf("%s\n", __FUNCTION_NAME__);

    //std::cout << "BEFORE SPAWN" << std::endl;
    //printParticleArray();

    cl::Kernel &kernel = ClProgram::Get().getKernel("spawnParticle");

    std::cout << "emitter_.getNbParticleActive_() : " << emitter_.getNbParticleActive_() << std::endl;
    queue_.getQueue().enqueueWriteBuffer(bufferModuleParams_, CL_TRUE, 0, sizeof(ModuleSpawnParams), &moduleSpawnParams_);
    kernel.setArg(0, bufferModuleParams_);
    kernel.setArg(1, emitter_.getDeviceBuffer().mem);
    kernel.setArg(2, glmVec3toClFloat3(emitter_.getSystem().getPosition()));
    kernel.setArg(3, Random::Get().getRandomSeed());
    std::cout << "SPPPAAWWNNN : at : " << at << " nb " << nbToSpawn << std::endl;
    OpenCGL::RunKernelWithMem(queue_.getQueue(), kernel, emitter_.getDeviceBuffer().mem, cl::NDRange(at), cl::NDRange(nbToSpawn));
    spwaned_ += nbToSpawn;
}

void    ParticleSpawnModule::reload()
{
    if (debug_)
        printf("%s\n", __FUNCTION_NAME__);
    bufferModuleParams_ = cl::Buffer(ClContext::Get().context, CL_MEM_WRITE_ONLY, sizeof(ModuleSpawnParams));
    init();
    spwaned_ = 0;
}