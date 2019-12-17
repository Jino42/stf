#pragma once

// A ParticleEmitter is a single particle effect that can be held in a
// ParticleSystem

#include <Engine/Time.hpp>
#include <memory>
#include <string>
#include <vector>

#include "Cl/ClQueue.hpp"
#include "Engine/Shader.hpp"
#include "ANommable.hpp"
#include "NTL.hpp"
#include "OCGL_Buffer.hpp"
#include "Particle/ParticleModule/AParticleModule.hpp"

class ParticleSystem;

struct EmitterParam {
    EmitterParam()
        : position({0, 0, 0}),
          nbMaxParticle(0),
          spawnParticlePerSec(0),
          time(0.f) {}
    cl_float3 position;
    cl_uint nbMaxParticle;
    cl_uint spawnParticlePerSec;
    cl_float deltaTime;
    cl_float time;
};

class AParticleEmitter : public ANommable {
  public:
    AParticleEmitter(ParticleSystem &system, ClQueue &queue,
                     std::string const &name, size_t nbParticle,
                     size_t nbParticlePerSec);

    virtual void init() = 0;

    virtual void render() = 0;

    virtual void update(float deltaTime);

    void spawn();

    std::string const &getName() const;
    OCGL_Buffer &getParticleOCGL_BufferData();
    cl::Buffer &getParticleBufferAlive();
    cl::Buffer &getParticleBufferSpawned();
    cl::Buffer &getParticleBufferDeath();
    cl::Buffer &getParticleSubBuffersLength();
    cl::Buffer &getBufferEmitterParam();

    ParticleSystem &getSystem() const;

    template <typename T>
    bool contain() const {
        for (auto &module: modules_) {
            if (dynamic_cast<T *>(module.get()))
                return true;
        }
        return false;
    }

    template <typename T>
    std::shared_ptr<T> getModule() const {
        for (auto &module: modules_) {
            if (dynamic_cast<T *>(module.get()))
                return std::dynamic_pointer_cast<T>(module);
        }
        return nullptr;
    }

    std::vector<std::shared_ptr<AParticleModule>> &getModules() {
        return modules_;
    }

    size_t getNbParticleActive_() const;
    void setShouldBeToSpawn(unsigned int nb, unsigned int at);

    template <typename T>
    void addModule() {
        modules_.emplace_back(std::make_shared<T>(*this));
    }

    virtual void reload();

    void checkReload() {
        if (!needReload_)
            return;
        needReload_ = false;
        reload();
        for (auto &module: modules_)
            module->reload();
        timerEmitter_.reset();
        timerEmitter_.start();
    }

    Timer &getTimer() { return timerEmitter_; }

    ClQueue &getQueue() const { return queue_; }

    unsigned int &getNbParticleMax() { return nbParticleMax_; }
    unsigned int &getNbParticlePerSec() { return nbParticlePerSec_; }
    unsigned int &getNbParticleActive() { return nbParticleActive_; }

    void setNbParticleMax(unsigned int value) { nbParticleMax_ = value; }
    void setNbParticlePerSec(unsigned int value) { nbParticlePerSec_ = value; }
    void setNbParticleActive(unsigned int value) { nbParticleActive_ = value; }

    void setReload(bool b) { needReload_ = b; }

    unsigned int getAt() const { return at_; }

    void printParticleArray();

  protected:
    ParticleSystem &system_;
    ClQueue &queue_;
    unsigned int nbParticleMax_;
    unsigned int nbParticlePerSec_;
    unsigned int nbParticleActive_;
    std::vector<std::shared_ptr<AParticleModule>> modules_;
    OCGL_Buffer OCGLBufferEmitterParticles_;
    EmitterParam cpuBufferParam_Emitter_;
    cl::Buffer gpuBufferParam_Emitter_;

  public:
    cl::Buffer gpuBufferParticles_Alive_;
    cl::Buffer gpuBufferParticles_Spawned_;
    cl::Buffer gpuBufferParticles_Death_;
    cl::Buffer gpuBufferParticles_SubLength_;
    int indexSub_[3];

  protected:
    Shader shader_;
    unsigned int shouldBeSpawn_;
    unsigned int at_; // remove
    bool needReload_;
    Timer timerEmitter_;

    static bool debug_;
};
