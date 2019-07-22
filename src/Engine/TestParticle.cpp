//
// Created by Noe TONIOLO on 2019-07-17.
//

#include "TestParticle.hpp"
#include "Particle/ParticleModule/ParticleAttractorModule.hpp"
#include "Particle/ParticleModule/ParticleColorModule.hpp"
#include "Particle/PaticleEmitter/ParticleEmitterSprite.hpp"
#include "Particle/PaticleEmitter/ParticleEmitterPoint.hpp"
#include "Particle/ParticleModule/ParticleMovementModule.hpp"
#include "Particle/ParticleModule/ModuleSizeOverLifetime.hpp"

void TestParticle::init() {
    //Cloud Emitter
/*
	particleSystem_.addEmitter<ParticleEmitterPoint>("Lol", 1024*1000);
	ParticleEmitterPoint &emitter = dynamic_cast<ParticleEmitterPoint&>(particleSystem_.getEmitter("Lol"));
	ParticleEmitterPoint &emitter2 = particleSystem_.getEmitter<ParticleEmitterPoint>("Lol");
	(void)emitter;
	(void)emitter2;
	emitter.addModule<ParticleAttractorModule>();
	emitter.addModule<ParticleColorModule>();
*/

//Sprite Emitter

	particleSystem_.addEmitter<ParticleEmitterSprite>("Lol", 10240.f / 5 * 0.9f, 10240);
    //particleSystem_.addEmitter<ParticleEmitterSprite>("Lol", 50.0f / 5.0f * 0.9f, 50);
	particleSystem_.setPosition(glm::vec3(0.0f, 258.0f, -30.0f));
	AParticleEmitter &emitter = particleSystem_.getEmitter<ParticleEmitterSprite>("Lol");
	//emitter.addModule<ParticleAttractorModule>();
	emitter.addModule<ParticleMovementModule>();
    emitter.addModule<ModuleSizeOverLifetime>();
	//emitter.addModule<ParticleAttractorModule>();

    particleSystem_.init();

}

void TestParticle::update(float deltaTime) {
    particleSystem_.update(deltaTime);
    particleSystem_.render();
}

TestParticle &TestParticle::Get() {
    if (!instance_)
        instance_ = std::make_unique<TestParticle>();
    return *instance_;
}

std::unique_ptr<TestParticle> TestParticle::instance_ = nullptr;