#pragma once

#include "AParticleEmitter.hpp"
#include "Engine/ModelEngine/ActorModel.hpp"

class Shader;
class ModuleSPH;

class ParticleEmitterSPH : public AParticleEmitter {
  public:
    ParticleEmitterSPH(ParticleSystem &system, ClQueue &queue, std::string const &name, size_t nbParticle);

    void init() override;

    void render() override;

    void update(float deltaTime) override;

    void supprMe(float scale);

    float &getLineWidth();

  private:
    Model model_;
    ActorModel actor_;
    std::shared_ptr<ModuleSPH> moduleSph_;
    float lineWidth_;
};
