#pragma once

#include "AParticleModule.hpp"
#include "Cl/ClQueue.hpp"
#include "OCGL_Buffer.hpp"

class ModuleColor : public AParticleModule {
public:
	ModuleColor(AParticleEmitter &emitter);
	void	init();
	void	update(float deltaTime);
	void    reload() override;

private:
};
