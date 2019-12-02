#include "AParticleModule.hpp"
#include "Particle/PaticleEmitter/AParticleEmitter.hpp"
#include "Engine/Debug.hpp"
#include <PathManager.hpp>

AParticleModule::AParticleModule(AParticleEmitter &emitter) :
emitter_(emitter),
queue_ (emitter.getQueue()),
nbParticleMax_ (emitter.getNbParticleMax()),
nbParticlePerSec_ (emitter.getNbParticlePerSec()),
nbParticleActive_ (emitter.getNbParticleActive()),
pathKernel_(PathManager::Get().getPath("particleKernels"))
{
    Debug::Get().setDebug("ParticleModule", AParticleModule::debug_);
}

bool AParticleModule::debug_ = true;