#pragma once

#include "Engine/AABB.hpp"
#include "Engine/Sphere.hpp"
#include "Particle/ParticleSystem.hpp"

class MainGraphic;
class Camera;

class TestParticle {
  public:
    void init();

    void update(float deltaTime);

    static TestParticle &Get();

  private:
    AABB box_;
    Sphere sphere_;
    static std::unique_ptr<TestParticle> instance_;
};
