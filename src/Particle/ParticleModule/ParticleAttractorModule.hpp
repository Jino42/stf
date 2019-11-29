#pragma once

#include "AParticleModule.hpp"
#include "Cl/ClQueue.hpp"
#include "OCGL_Buffer.hpp"

class AParticleEmitter;

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
