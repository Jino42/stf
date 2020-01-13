#pragma once

#include "AParticleModule.hpp"
#include "Cl/ClQueue.hpp"
#include "OCGL_Buffer.hpp"

class AParticleEmitter;

class ModulePhysicConstrainInShape : public AParticleModule {
  public:
    ModulePhysicConstrainInShape(AParticleEmitter &emitter, std::string nameShape);
    void init() override;
    void update(float deltaTime) override;
    void reload() override;

  private:
    std::string nameShape_;
    std::shared_ptr<cl::Buffer> gpuBufferParticles_Physic_;

    cl::Buffer gpuBufferLocal_ShapeConstrain_;

    void setShapeContrain_();
};
