#pragma once

#include "AParticleModule.hpp"
#include "Cl/ClQueue.hpp"
#include "OCGL_Buffer.hpp"

class ParticleColorModule : public AParticleModule {
public:
	ParticleColorModule(AParticleEmitter &emitter);
	void	init();
	void	update(float deltaTime);
	void    reload() override;

private:
};
