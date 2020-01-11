#pragma once

#include "AParticleModule.hpp"
#include "Cl/ClQueue.hpp"
#include "OCGL_Buffer.hpp"

class AParticleEmitter;

struct ModuleParamPhysicNoise {
    cl_float3 gravity;
};

class ModulePhysicNoise : public AParticleModule {
  public:
    ModulePhysicNoise(AParticleEmitter &emitter);
    void init() override;
    void update(float deltaTime) override;
    void reload() override;

    ModuleParamPhysicNoise &getCpuModuleParam();

  private:
    std::shared_ptr<cl::Buffer> gpuBufferParticles_Physic_;

    std::shared_ptr<cl::Buffer> gpuBufferModuleParam_;
    ModuleParamPhysicNoise cpuBufferModuleParam_;
};
