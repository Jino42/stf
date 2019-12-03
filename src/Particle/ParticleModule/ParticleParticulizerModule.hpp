#pragma once

#include "AParticleModule.hpp"
#include "Engine/ModelEngine/Model.hpp"
#include <memory>

class AParticleEmitter;
class ParticuleTargetModule;

struct ParticleDataMeshParticulizer {
	cl_float3	position;
};

class ParticleParticulizerModule : public AParticleModule {
public:
	ParticleParticulizerModule(AParticleEmitter &emitter);

	void	init() override;

private:
	Model			model_;
	std::unique_ptr<ParticleDataMeshParticulizer[]> cpuBufferParticles_Position_;
	std::shared_ptr<ParticuleTargetModule> moduleTarget_;
};
