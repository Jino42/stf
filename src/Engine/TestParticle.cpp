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

	int i = 0;
	for (auto &system : particleSystem_) {
		//system.addEmitter<ParticleEmitterSprite>("Lol", 30.f / 8.f * 0.9f, 30);
		system.addEmitter<ParticleEmitterSprite>("Lol", 120000.f / 8.f * 0.9f, 7050);
		//system.addEmitter<ParticleEmitterSprite>("Lol", 100.f / 8.f * 0.9f, 1000000);
		//system.addEmitter<ParticleEmitterSprite>("Lol", 50.0f / 5.0f * 0.9f, 50);
		system.setPosition(glm::vec3(60 * i, 258.0f, -30.0f));
		AParticleEmitter &emitter = system.getEmitter<ParticleEmitterSprite>("Lol");
		//emitter.addModule<ParticleAttractorModule>();
		emitter.addModule<ParticleMovementModule>();
		emitter.addModule<ModuleSizeOverLifetime>();
		//emitter.addModule<ParticleAttractorModule>();

		system.init();
		i++;
	}



}

void TestParticle::update(float deltaTime) {
	for (auto &system : particleSystem_) {
		system.update(deltaTime);
		system.render();
	}
}

TestParticle &TestParticle::Get() {
    if (!instance_)
        instance_ = std::make_unique<TestParticle>();
    return *instance_;
}

std::unique_ptr<TestParticle> TestParticle::instance_ = nullptr;