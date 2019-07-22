#pragma once

#include "AParticleModule.hpp"
#include "Cl/ClQueue.hpp"
#include "DeviceBuffer.hpp"

class ParticleEmitterMesh;

struct ParticleAttractorModuleData {
};

class ParticleAttractorModule : public AParticleModule {
public:
	ParticleAttractorModule(AParticleEmitter &emitter);
	void	init() override;
	void	update(float deltaTime) override;
    void    reload() override;

private:
	cl::Buffer		buffer_;
};
