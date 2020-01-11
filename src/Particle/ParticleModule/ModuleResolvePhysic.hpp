#pragma once

#include "AParticleModule.hpp"
#include "Cl/ClQueue.hpp"
#include "OCGL_Buffer.hpp"

class AParticleEmitter;
struct ParticleDataPhysic {
    cl_float3 velocity;
    cl_float mass;
};

class ModuleResolvePhysic : public AParticleModule {
  public:
    ModuleResolvePhysic(AParticleEmitter &emitter);
    void init() override;
    void update(float deltaTime) override;
    void reload() override;

  private:
    bool doAttractor_;
    std::shared_ptr<cl::Buffer> gpuBufferParticles_Physic_;
};
