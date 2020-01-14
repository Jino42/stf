#pragma once

#include "AParticleModule.hpp"
#include "Cl/ClQueue.hpp"
#include "OCGL_Buffer.hpp"

class AParticleEmitter;

struct ModuleParamPhysicGravity {
    cl_float3 gravity;
};

class ModulePhysicGravity : public AParticleModule {
  public:
    ModulePhysicGravity(AParticleEmitter &emitter);
    void init() override;
    void update(float deltaTime) override;
    void reload() override;

    void jsonParse(json &itModule) override;

    ModuleParamPhysicGravity &getCpuModuleParam();

  private:
    bool doAttractor_;
    std::shared_ptr<cl::Buffer> gpuBufferParticles_Physic_;

    std::shared_ptr<cl::Buffer> gpuBufferModuleParam_;
    ModuleParamPhysicGravity cpuBufferModuleParam_;
};
