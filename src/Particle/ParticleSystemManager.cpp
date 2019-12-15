#include "ParticleSystemManager.hpp"
#include "json.hpp"


ParticleSystemManager &ParticleSystemManager::Get() {
	if (!instance_)
		instance_ = std::make_unique<ParticleSystemManager>();
	return *instance_;
}

ParticleSystem &ParticleSystemManager::getParticleSystem(std::string const &name) {
	return mapSystems_.at(name);
}

void ParticleSystemManager::addParticleSystem(std::string const &name) {
	if (mapSystems_.find(name) == mapSystems_.end())
		mapSystems_.try_emplace(name, name);
}

void ParticleSystemManager::initAllParticleSystem() {
	for (auto &pair : mapSystems_) {
		pair.second.init();
	}
}

void ParticleSystemManager::updateAllParticleSystem(float deltaTime) {
	for (auto &pair : mapSystems_) {
		pair.second.update(deltaTime);
	}
}

void ParticleSystemManager::renderAllParticleSystem() {
	for (auto &pair : mapSystems_) {
		pair.second.render();
	}
}

std::unique_ptr<ParticleSystemManager> ParticleSystemManager::instance_ = nullptr;

std::map<std::string, ParticleSystem> &ParticleSystemManager::getMapParticleSystem() {
	return mapSystems_;
}
