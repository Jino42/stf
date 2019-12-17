#pragma once

//A ParticleSystem is a complete particle effect that contains any number of ParticleEmitters.

#include "ANommable.hpp"
#include "Cl/ClQueue.hpp"
#include "Particle/PaticleEmitter/AParticleEmitter.hpp"
#include <list>

#define EMITTER_MAX 8

class ParticleSystem : public ANommable {
  public:
    //Emit
    //Reload
    //...

    //bOrientZAxisTowardCamera
    ParticleSystem() = delete;

    explicit ParticleSystem(std::string const &name);

    template <typename T, typename... Args>
    void addEmitter(std::string const &name, Args... args) {
        if (emitterBuffer_.size() < EMITTER_MAX)
            emitterBuffer_.emplace_back(std::make_unique<T>(*this, queue_, name, args...));
        else
            throw std::runtime_error("ParticleSystem:: Cant add new emitter. Limit are already been reached");
    }

    AParticleEmitter &getEmitter(std::string const &nameToFind) {
        auto it = std::find_if(
            emitterBuffer_.begin(), emitterBuffer_.end(),
            [nameToFind](std::unique_ptr<AParticleEmitter> const &emitter)
                -> bool {
                return emitter->getName() == nameToFind;
            });
        if (it == emitterBuffer_.end())
            throw std::runtime_error("ParticleSystem:: Cant find emitter");
        return **it;
    }

    template <typename T>
    T &getEmitter(std::string const &nameToFind) {
        auto it = std::find_if(
            emitterBuffer_.begin(), emitterBuffer_.end(),
            [nameToFind](std::unique_ptr<AParticleEmitter> const &emitter)
                -> bool {
                return emitter->getName() == nameToFind;
            });
        if (it == emitterBuffer_.end())
            throw std::runtime_error("ParticleSystem:: Cant find emitter");
        return dynamic_cast<T &>(**it);
    }

    void init();
    void update(float deltaTime);
    void render();
    void reload();
    void setPosition(glm::vec3 const &position) {
        position_ = position;
    }
    glm::vec3 &getPosition() {
        return position_;
    }

    std::vector<std::unique_ptr<AParticleEmitter>> &getEmiters() {
        return emitterBuffer_;
    };

  private:
    std::vector<std::unique_ptr<AParticleEmitter>> emitterBuffer_;
    ClQueue queue_;
    glm::vec3 position_;

    static bool firstSystem_;
};
