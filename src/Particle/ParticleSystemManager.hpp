#pragma once

#include "ParticleSystem.hpp"
#include <memory>

class ParticleSystemManager {
  public:
    ParticleSystemManager() = default;
    ParticleSystemManager(ParticleSystemManager const &) = delete;
    ~ParticleSystemManager() = default;
    static ParticleSystemManager &Get();

    void addParticleSystem(std::string const &name);

    ParticleSystem &getParticleSystem(std::string const &name);

    std::map<std::string, ParticleSystem> &getMapParticleSystem();

    void initAllParticleSystem();
    void updateAllParticleSystem(float deltaTime);
    void renderAllParticleSystem();
    void deleteAllParticleSystem();

  private:
    std::map<std::string, ParticleSystem> mapSystems_;

    static std::unique_ptr<ParticleSystemManager> instance_;
};
