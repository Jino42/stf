#pragma once

#include "Particle/ParticleSystem.hpp"
#include "Engine/AABB.hpp"

class MainGraphic;
class Camera;

class TestParticle {
public:
    void init();

    void update(float deltaTime);


    static TestParticle &Get();
private:
    AABB box_;
    static std::unique_ptr<TestParticle> instance_;

};
