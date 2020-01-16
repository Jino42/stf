//
// Created by Noe TONIOLO on 2019-07-17.
//

#include "TestParticle.hpp"
#include "Particle/ParticleModule/ModuleAttractor.hpp"
#include "Particle/ParticleModule/ModuleColor.hpp"
#include "Particle/ParticleModule/ModuleMeshParticulizer.hpp"
#include "Particle/ParticleModule/ModuleMoveToTarget.hpp"
#include "Particle/ParticleModule/ModuleMovement.hpp"
#include "Particle/ParticleModule/ModuleSizeOverLifetime.hpp"
#include "Particle/PaticleEmitter/ParticleEmitterMesh.hpp"
#include "Particle/PaticleEmitter/ParticleEmitterPoint.hpp"
#include "Particle/PaticleEmitter/ParticleEmitterSprite.hpp"

#include "Particle/ParticleSystemManager.hpp"

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

    //ParticleSystemManager::Get().addParticleSystem("SAwrrr");
    //particleSystem_ = &ParticleSystemManager::Get().getParticleSystem("SAwrrr");
    //Sprite Emitter

    //int i = 0;
    //for (auto &system : particleSystem_) {
    /*
		//system.addEmitter<ParticleEmitterSprite>("Lol", 30.f / 8.f * 0.9f, 30);
		//system.addEmitter<ParticleEmitterSprite>("Lol", 120000.f / 8.f * 0.9f, 5000);
		system.addEmitter<ParticleEmitterSprite>("Lol", 10000000.f / 8.f * 0.9f, 1000000);
		//system.addEmitter<ParticleEmitterSprite>("Lol", 50.0f / 5.0f * 0.9f, 50);
		system.setPosition(glm::vec3(60 * i, 258.0f, -30.0f));
		AParticleEmitter &emitter = system.getEmitter<ParticleEmitterSprite>("Lol");
		//emitter.addModule<ParticleAttractorModule>();
		emitter.addModule<ParticleMovementModule>();
		emitter.addModule<ModuleSizeOverLifetime>();
		//emitter.addModule<ParticleAttractorModule>();
*/

    /*
		system.addEmitter<ParticleEmitterPoint>("Lol", 500000);

		system.setPosition(glm::vec3(60 * i, 258.0f, -30.0f));
		AParticleEmitter &emitter = system.getEmitter<ParticleEmitterPoint>("Lol");

		//emitter.addModule<ParticleAttractorModule>();
		emitter.addModule<ParticleParticulizerModule>();
		emitter.addModule<ParticleMoveToTarget>();
		emitter.addModule<ParticleColorModule>();
*/
    /*
    particleSystem_->addEmitter<ParticleEmitterPoint>("Lolqwe", 50000);

    particleSystem_->setPosition(glm::vec3(60, 60.0f, 60.0f));
    AParticleEmitter &emitter = particleSystem_->getEmitter<ParticleEmitterPoint>("Lolqwe");

    //emitter.addModule<ParticleAttractorModule>();
    emitter.addModule<ModuleMeshParticulizer>();
    emitter.addModule<ModuleMoveToTarget>();
    emitter.addModule<ModuleColor>();
    */
    /*
		system.addEmitter<ParticleEmitterMesh>("Lol", 30.f / 8.f * 0.9f, 30);

		system.setPosition(glm::vec3(60 * i, 258.0f, -30.0f));
		AParticleEmitter &emitter = system.getEmitter<ParticleEmitterMesh>("Lol");

		emitter.addModule<ModuleMovement>();
		emitter.addModule<ModuleSizeOverLifetime>();

		system.init();
		*/
    //		i++;
    //	}
}

void TestParticle::update(float deltaTime) {
    //particleSystem_->update(deltaTime);
    //particleSystem_->render();
    //	for (auto &system : particleSystem_) {
    //		system.update(deltaTime);
    //		system.render();
    //	}
}

TestParticle &TestParticle::Get() {
    if (!instance_)
        instance_ = std::make_unique<TestParticle>();
    return *instance_;
}

std::unique_ptr<TestParticle> TestParticle::instance_ = nullptr;