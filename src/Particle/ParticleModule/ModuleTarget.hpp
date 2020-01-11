#pragma once

#include "AParticleModule.hpp"

class AParticleEmitter;

struct ParticleDataTarget {
    cl_float3 target;
};

class ModuleTarget : public AParticleModule {
  public:
    ModuleTarget(AParticleEmitter &emitter);

  private:
    std::shared_ptr<cl::Buffer> gpuBufferParticles_Target_;
};
