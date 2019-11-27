#include "ParticleSystem.hpp"
#include <exception>
#include "Cl/ClProgram.hpp"

ParticleSystem::ParticleSystem() :
		position_(glm::vec3(0))
{
	if (ParticleSystem::firstSystem_) {
		ParticleSystem::firstSystem_ = false;
		boost::filesystem::path path(ROOT_PATH);
		path = path / "src" / "Particle" / "Kernel" / "include";
		ClProgram::Get().addInclude(path / "Base_NTL.hl");
		ClProgram::Get().addInclude(path / "Range.hl");
		ClProgram::Get().addInclude(path / "Random.hl");
		ClProgram::Get().addInclude(path / "ModuleStruct.hl");
		ClProgram::Get().addInclude(path / "ParticleData.hl");
		ClProgram::Get().addInclude(path / "NTL.hl");
		ClProgram::Get().addProgram(boost::filesystem::path(ROOT_PATH) / "src" / "Particle" / "Kernel" / "Tools.cl");

	}
}

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

bool ParticleSystem::firstSystem_ = true;