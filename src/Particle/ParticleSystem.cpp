#include "ParticleSystem.hpp"
#include <exception>
#include "Cl/ClProgram.hpp"
#include <PathManager.hpp>

ParticleSystem::ParticleSystem(std::string const &name)
    : ANommable(name),
		position_(glm::vec3(0))
{
	if (ParticleSystem::firstSystem_) {
		ParticleSystem::firstSystem_ = false;

		boost::filesystem::path &path = PathManager::Get().getPath("particleKernelIncludes");

		ClProgram::Get().addInclude(path / "Base_NTL.hl");
		ClProgram::Get().addInclude(path / "Range.hl");
		ClProgram::Get().addInclude(path / "Random.hl");
		ClProgram::Get().addInclude(path / "ModuleStruct.hl");
		ClProgram::Get().addInclude(path / "ParticleData.hl");
		ClProgram::Get().addInclude(path / "NTL.hl");
		ClProgram::Get().addProgram(PathManager::Get().getPath("particleKernels") / "Tools.cl");
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

void ParticleSystem::reload() {
  for (auto &emitter : emitterBuffer_) {
    emitter->reload();
  }
}

bool ParticleSystem::firstSystem_ = true;