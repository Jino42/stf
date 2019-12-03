#pragma once

#include "AParticleModule.hpp"
#include "Engine/ModelEngine/Model.hpp"
#include <memory>

class AParticleEmitter;
class ModuleTarget;

struct ParticleDataMeshParticulizer {
	cl_float3	position;
};

class ModuleMeshParticulizer : public AParticleModule {
public:
	ModuleMeshParticulizer(AParticleEmitter &emitter);

	void	init() override;

private:
	Model			model_;
	std::unique_ptr<ParticleDataMeshParticulizer[]> cpuBufferParticles_Position_;
	std::shared_ptr<ModuleTarget> moduleTarget_;
};
