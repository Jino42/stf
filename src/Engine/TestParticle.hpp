#pragma once

#include "Particle/ParticleSystem.hpp"

class MainGraphic;
class Camera;

class TestParticle {
public:
    void init();

    void update(float deltaTime);

private:
    ParticleSystem					particleSystem_;

};
