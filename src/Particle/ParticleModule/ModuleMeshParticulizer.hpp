#pragma once

#include "AParticleModule.hpp"
#include "Engine/ModelEngine/Model.hpp"
#include <memory>
#include "ModuleTarget.hpp"

class AParticleEmitter;

class ModuleMeshParticulizer : public AParticleModule {
  public:
    ModuleMeshParticulizer(AParticleEmitter &emitter);

    void init() override;

  private:
    Model model_;
    std::unique_ptr<ParticleDataTarget[]> cpuBufferParticles_Target_;
    std::shared_ptr<cl::Buffer> gpuBufferParticles_Target_;
};
