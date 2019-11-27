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
#include "Engine/TimerOnConstructOffDestruct.hpp"
#include "NTL_Debug.hpp"
#include "Cl/ClKernel.hpp"

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
		TimerOnConstructOffDestruct timer(__FUNCTION_NAME__);
}
void	ParticleSpawnModule::update(float deltaTime) {
		TimerOnConstructOffDestruct timer(__FUNCTION_NAME__);

    int shouldBeAlreadySpawn = (int)(emitter_.getTimer().count() / (1000.f / nbParticlePerSec_));

    if (debug_)
        std::cout << shouldBeAlreadySpawn << " >=" << spwaned_ << std::endl;
    if (shouldBeAlreadySpawn < 0 || shouldBeAlreadySpawn < spwaned_)
        return ;
    int shouldBeSpawn = shouldBeAlreadySpawn - spwaned_;

    if (debug_) {
        std::cout << "ShouldBeSpawned at this frame ||| " << shouldBeSpawn << std::endl;
        std::cout << "getNbParticleActive : " << emitter_.getNbParticleActive_() << std::endl;
    }

    if (shouldBeSpawn <= 0 || emitter_.getNbParticleActive_() >= nbParticleMax_)
        return ;
    if (shouldBeSpawn + emitter_.getNbParticleActive_() > nbParticleMax_)
        shouldBeSpawn = nbParticleMax_ - emitter_.getNbParticleActive_();
    if (debug_)
        std::cout << "ShouldBeSpawned at this frame ||| " << shouldBeSpawn << std::endl;
    emitter_.setShouldBeToSpawn(shouldBeSpawn, emitter_.getNbParticleActive_());
}

void	ParticleSpawnModule::spawn(unsigned int nbToSpawn, unsigned int at) {
		TimerOnConstructOffDestruct timer(__FUNCTION_NAME__);

    //std::cout << "BEFORE SPAWN" << std::endl;
    //printParticleArray();

    ClKernel kernel("spawnParticle");

    if (debug_)
        std::cout << "emitter_.getNbParticleActive_() : " << emitter_.getNbParticleActive_() << std::endl;

	queue_.getQueue().enqueueWriteBuffer(bufferModuleParams_, CL_TRUE, 0, sizeof(ModuleSpawnParams),
										 &moduleSpawnParams_);
	kernel.setArgs(bufferModuleParams_,
			emitter_.getDeviceBuffer().mem,
			emitter_.deviceBufferAlive_,
			emitter_.deviceBufferAlive2_,
			emitter_.deviceBufferDeath_,
			emitter_.deviceBufferLengthSub_,
			glmVec3toClFloat3(emitter_.getSystem().getPosition()));


	std::vector<cl::Memory> cl_vbos;
	cl_vbos.push_back(emitter_.getDeviceBuffer().mem);

	{
		TimerOnConstructOffDestruct *timerD = new TimerOnConstructOffDestruct("----------During ALEA");
		kernel.setArgs(Random::Get().getRandomSeed());
		delete timerD;
	}

	{
		TimerOnConstructOffDestruct timerE("----------During");


		queue_.getQueue().enqueueWriteBuffer(emitter_.deviceBufferLengthSub_, CL_TRUE, 0, sizeof(int) * 3, &emitter_.indexSub_);
		OpenCGL::RunKernelWithMem(queue_.getQueue(), kernel, cl_vbos, cl::NullRange, cl::NDRange(nbToSpawn));
		queue_.getQueue().enqueueReadBuffer(emitter_.deviceBufferLengthSub_, CL_TRUE, 0, sizeof(int) * 3, &emitter_.indexSub_);
		spwaned_ += nbToSpawn;
	}

	std::cout << "-------- spawnParticle" << std::endl;
	printSubArrayParticle(emitter_, queue_.getQueue());
	std::cout << "xxxxxxxx spawnParticle" << std::endl;


}

void    ParticleSpawnModule::reload()
{
		TimerOnConstructOffDestruct timer(__FUNCTION_NAME__);
    bufferModuleParams_ = cl::Buffer(ClContext::Get().context, CL_MEM_WRITE_ONLY, sizeof(ModuleSpawnParams));
    init();
    spwaned_ = 0;
}