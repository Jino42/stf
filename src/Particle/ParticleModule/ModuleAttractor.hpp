#pragma once

#include "AParticleModule.hpp"
#include "Cl/ClQueue.hpp"
#include "OCGL_Buffer.hpp"

class AParticleEmitter;

class ModuleAttractor : public AParticleModule {
  public:
    ModuleAttractor(AParticleEmitter &emitter);
    void init() override;
    void update(float deltaTime) override;
    void reload() override;

    void jsonParse(json &itModule) override;
    void gui() override;


    void addNameShapeAttractor(std::string name);


  private:
    bool mouse_;
    bool doAttractor_;
    std::vector<std::string> namesShapeAttractor_;
    std::shared_ptr<cl::Buffer> gpuBufferParticles_Physic_;
    cl::Buffer gpuBufferLocal_ShapeAttractor_;

    void updateAttractor_();
};
