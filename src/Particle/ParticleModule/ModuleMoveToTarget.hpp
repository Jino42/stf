#pragma once

#include "AParticleModule.hpp"

class AParticleEmitter;
class ModuleTarget;

class ModuleMoveToTarget : public AParticleModule {
  public:
    ModuleMoveToTarget(AParticleEmitter &emitter);

    void update(float deltaTime) override;
    void init() override;

  private:
    std::shared_ptr<cl::Buffer> gpuBufferParticles_Target_;
};
