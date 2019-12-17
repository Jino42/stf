#pragma once

#include "AParticleEmitter.hpp"
#include "Engine/ModelEngine/ActorModel.hpp"

class Shader;

class ParticleEmitterPoint : public AParticleEmitter {
  public:
    ParticleEmitterPoint(ParticleSystem &system, ClQueue &queue, std::string const &name, size_t nbParticle);

    void init() override;

    void render() override;

    void update(float deltaTime) override;

    void supprMe(float scale);

  private:
    //unsigned int VAO;
    Model model_;
    ActorModel actor_;
};
