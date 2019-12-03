#pragma once

#include "AParticleModule.hpp"
#include "Cl/ClQueue.hpp"
#include "OCGL_Buffer.hpp"

class AParticleEmitter;

class ModuleAttractor : public AParticleModule {
public:
	ModuleAttractor(AParticleEmitter &emitter);
	void	init() override;
	void	update(float deltaTime) override;
    void    reload() override;

private:
};
