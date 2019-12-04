#pragma once

#include "AParticleModule.hpp"
#include "Cl/ClQueue.hpp"
#include "OCGL_Buffer.hpp"

class ModuleColor : public AParticleModule {
public:
	ModuleColor(AParticleEmitter &emitter);
	void	init() override;
	void	update(float deltaTime) override;
	void    reload() override;

private:
};
