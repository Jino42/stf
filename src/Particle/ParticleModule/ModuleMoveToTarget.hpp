#pragma once

#include "AParticleModule.hpp"

class AParticleEmitter;
class ModuleTarget;

class ModuleMoveToTarget : public AParticleModule {
public:
	ModuleMoveToTarget(AParticleEmitter &emitter);

	void update(float deltaTime) override;

private:
	std::shared_ptr<ModuleTarget> targetModule_;
};
