#include "ParticleSystem.hpp"
#include <exception>

void ParticleSystem::update(float deltaTime) {
	for (auto &emitter : emitterBuffer_) {
		emitter->update(deltaTime);
		emitter->spawn();
	}
}

void ParticleSystem::init() {
	for (auto &emitter : emitterBuffer_) {
		emitter->init();
	}
}

void ParticleSystem::render() {
	for (auto &emitter : emitterBuffer_) {
		emitter->render();
	}
}