#include "AParticleModule.hpp"
#include "Engine/Debug.hpp"
#include "Particle/PaticleEmitter/AParticleEmitter.hpp"
#include <PathManager.hpp>
#include <Gui/Gui.hpp>

AParticleModule::AParticleModule(AParticleEmitter &emitter)
    : emitter_(emitter),
      queue_(emitter.getQueue()),
      nbParticleMax_(emitter.getNbParticleMax()),
      nbParticlePerSec_(emitter.getNbParticlePerSec()),
      nbParticleActive_(emitter.getNbParticleActive()),
      pathKernel_(PathManager::Get().getPath("particleKernels")),
      isActive_(true) {
    Debug::Get().setDebug("ParticleModule", AParticleModule::debug_);
}

std::string AParticleModule::getModuleName() const {
    return typeid(this).name();
}

void AParticleModule::gui() {
    ImGui::Checkbox("Active", &isActive_);
}

bool AParticleModule::debug_ = false;