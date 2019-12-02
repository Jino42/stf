#pragma once

#include "AParticleModule.hpp"
#include "Engine/ModelEngine/Model.hpp"
#include <memory>

class AParticleEmitter;
class ParticuleTargetModule;


struct ModuleMeshParticulizerData {
	cl_float3	position;
};

class ParticleParticulizerModule : public AParticleModule {
public:
	ParticleParticulizerModule(AParticleEmitter &emitter);

	void	init() override;

private:
	cl::Buffer		bufferPositionParticles_;
	Model			model_;
	std::unique_ptr<ModuleMeshParticulizerData[]> dataPositionParticles_;
	std::shared_ptr<ParticuleTargetModule> targetModule_;
};
