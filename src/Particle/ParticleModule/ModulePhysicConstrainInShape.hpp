#pragma once

#include "AParticleModule.hpp"
#include "Cl/ClQueue.hpp"
#include "OCGL_Buffer.hpp"

class AParticleEmitter;

struct ModuleParamPhysicConstrainInShape {
    cl_float3 gravity;
};

class ModulePhysicConstrainInShape : public AParticleModule {
  public:
    ModulePhysicConstrainInShape(AParticleEmitter &emitter);
    void init() override;
    void update(float deltaTime) override;
    void reload() override;

    ModuleParamPhysicConstrainInShape &getCpuModuleParam();

  private:
    bool doAttractor_;
    std::shared_ptr<cl::Buffer> gpuBufferParticles_Physic_;

    std::shared_ptr<cl::Buffer> gpuBufferModuleParam_;
    ModuleParamPhysicConstrainInShape cpuBufferModuleParam_;
};
