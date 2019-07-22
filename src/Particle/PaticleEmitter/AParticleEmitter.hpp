#pragma once

//A ParticleEmitter is a single particle effect that can be held in a ParticleSystem

#include <string>
#include <vector>
#include <memory>
#include "Cl/ClQueue.hpp"
#include "DeviceBuffer.hpp"
#include "Particle/ParticleModule/AParticleModule.hpp"
#include "Engine/Shader.hpp"
#include <chrono>
#include "NTL.hpp"

class ParticleSystem;

class AParticleEmitter {
public:
	AParticleEmitter(ParticleSystem &system, ClQueue &queue, std::string const &name, size_t nbParticle, size_t nbParticlePerSec);

	virtual void init() = 0;

	virtual void render() = 0;

	virtual void update(float deltaTime) = 0;

	void spawn();

	std::string const &getName() const;
	DeviceBuffer &getDeviceBuffer();
	ParticleSystem &getSystem() const;
	template < typename T >
    std::shared_ptr<T> getModule() const {
        for (auto &module : modules_)
        {
            if (dynamic_cast<T *>(module.get()))
                return std::dynamic_pointer_cast<T>(module);
        }
        return nullptr;
    }
	size_t getNbParticleActive_() const;
	void setShouldBeToSpawn(unsigned int nb, unsigned int at);

	template < typename T >
	void addModule() {
		modules_.emplace_back(std::make_shared<T>(*this));
	}

	virtual void reload();

	void checkReload() {
        if (!needReload_)
            return ;
        needReload_ = false;
        reload();
        for (auto &module : modules_)
            module->reload();
        startEmitter_ = std::chrono::steady_clock::now();
	}


	ClQueue &getQueue() const {
	    return queue_;
	}

    unsigned int &getNbParticleMax() {
        return nbParticleMax_;
    }
    unsigned int &getNbParticlePerSec() {
        return nbParticlePerSec_;
    }
    unsigned int &getNbParticleActive() {
        return nbParticleActive_;
    }

    std::chrono::time_point<std::chrono::steady_clock> &getStartPoint() {
        return startEmitter_;
    }

    void setNbParticleMax(unsigned int value) {
        nbParticleMax_ = value;
    }
    void setNbParticlePerSec(unsigned int value) {
        nbParticlePerSec_ = value;
    }
    void setNbParticleActive(unsigned int value) {
        nbParticleActive_ = value;
    }

    void setReload(bool b) {
        needReload_ = b;
    }

    unsigned int getAt() const {
        return at_;
    }

    void printParticleArray();

protected:
	ParticleSystem									&system_;
	std::string										name_;
	ClQueue											&queue_;
	unsigned int 											nbParticleMax_;
    unsigned int 											nbParticlePerSec_;
	unsigned int											nbParticleActive_;
	std::vector<std::shared_ptr<AParticleModule>>	modules_;
	DeviceBuffer									deviceBuffer_;
	Shader											shader_;
	unsigned int									shouldBeSpawn_;
	unsigned int									at_;
	bool                                            needReload_;
    std::chrono::time_point<std::chrono::steady_clock> startEmitter_;

	static bool debug_;
};

