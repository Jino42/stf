#pragma once

#include "AParticleModule.hpp"

class AParticleEmitter;
class ParticuleTargetModule;


class ParticleMoveToTarget : public AParticleModule {
public:
	ParticleMoveToTarget(AParticleEmitter &emitter);

	void update(float deltaTime) override;

private:
	cl::Buffer		gpuBufferTargetParticles_;
	std::shared_ptr<ParticuleTargetModule> targetModule_;
};
