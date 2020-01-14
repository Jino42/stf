#pragma once

#include "AParticleModule.hpp"
#include "Cl/ClQueue.hpp"
#include "OCGL_Buffer.hpp"
#include "Range.hpp"

class AParticleEmitter;

struct ModuleParamSpawn {
    Rangef startLifeTime;
    Rangef startSpeed;
    Rangef startSize;
    Rangef startRotation;
    //cl_float3 startColor;
};

class ParticleSpawnModule : public AParticleModule {
  public:
    ParticleSpawnModule(AParticleEmitter &emitter);
    void init() override;
    void update(float deltaTime) override;
    void spawn(unsigned int nbToSpawn, unsigned int at) override;
    void reload() override;

    void jsonParse(json &itModule) override;
    void gui() override;

    void setRate(unsigned int rate);

    unsigned int &getNbParticlePerSec() {
        return nbParticlePerSec_;
    }

  private:
    cl::Buffer gpuBufferModuleParam_;
    ModuleParamSpawn cpuBufferModuleParam_;
    unsigned int spwaned_;

    unsigned int &nbParticlePerSec_;
};
