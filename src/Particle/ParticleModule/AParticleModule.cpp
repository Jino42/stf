#include "AParticleModule.hpp"
#include "Particle/PaticleEmitter/AParticleEmitter.hpp"

AParticleModule::AParticleModule(AParticleEmitter &emitter) :
emitter_(emitter),
queue_ (emitter.getQueue()),
nbParticleMax_ (emitter.getNbParticleMax()),
nbParticlePerSec_ (emitter.getNbParticlePerSec()),
nbParticleActive_ (emitter.getNbParticleActive())
{
    pathKernel_ = ROOT_PATH;
    pathKernel_ = pathKernel_ / "src" / "Particle" / "Kernel";
}

bool AParticleModule::debug_ = true;