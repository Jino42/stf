#pragma once

#include "Particle/ParticleSystem.hpp"

class MainGraphic;
class Camera;

class TestParticle {
public:
    void init();

    void update(float deltaTime);


    static TestParticle &Get();
private:
    ParticleSystem					particleSystem_[1];

    static std::unique_ptr<TestParticle> instance_;

};
