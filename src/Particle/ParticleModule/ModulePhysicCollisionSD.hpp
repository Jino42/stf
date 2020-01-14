#pragma once

#include "AParticleModule.hpp"
#include "Cl/ClQueue.hpp"
#include "OCGL_Buffer.hpp"

class AParticleEmitter;

struct ModuleParamPhysicGravity {
    cl_float3 gravity;
};

class ModulePhysicCollisionSD : public AParticleModule {
  public:
    ModulePhysicCollisionSD(AParticleEmitter &emitter);
    void init() override;
    void update(float deltaTime) override;
    void reload() override;

    ModuleParamPhysicGravity &getCpuModuleParam();

  private:
    bool doAttractor_;
    std::shared_ptr<cl::Buffer> gpuBufferParticles_Physic_;

    std::shared_ptr<cl::Buffer> gpuBufferModuleParam_;
    ModuleParamPhysicGravity cpuBufferModuleParam_;
};