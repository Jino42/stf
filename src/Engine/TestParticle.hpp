#pragma once

#include "Engine/AABB.hpp"
#include "Engine/Sphere.hpp"
#include "Particle/ParticleSystem.hpp"

class MainGraphic;
class Camera;

class TestParticle {
  public:
    TestParticle() = default; 
    ~TestParticle() = default;
    TestParticle(TestParticle &) = delete;

    void init();

    void update(float deltaTime);

    static TestParticle &Get();

  private:
    AABB box_;
    Sphere sphere_;
    static std::unique_ptr<TestParticle> instance_;
    ParticleSystem *particleSystem_;
};
