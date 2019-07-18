#pragma once

#include "AParticleModule.hpp"
#include "Cl/ClQueue.hpp"
#include "DeviceBuffer.hpp"

struct ParticleColorModuleData {
};

class ParticleColorModule : public AParticleModule {
public:
	ParticleColorModule(AParticleEmitter &emitter);
	void	init();
	void	update(float deltaTime);
	void    reload() override;

private:
	cl::Buffer		buffer_;
};