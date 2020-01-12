#pragma once

#include "AParticleModule.hpp"
#include "Engine/ModelEngine/Model.hpp"
#include "ModuleTarget.hpp"
#include <memory>

class AParticleEmitter;

class ModuleMeshParticulizer : public AParticleModule {
  public:
    ModuleMeshParticulizer(AParticleEmitter &emitter);

    void init() override;

    void reorganise();

  private:
    ClKernel kernelReorganise_;
    Model *model_;
    std::unique_ptr<ParticleDataTarget[]> cpuBufferParticles_Target_;
    std::shared_ptr<cl::Buffer> gpuBufferParticles_Target_;
};
