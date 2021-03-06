#include "AParticleEmitter.hpp"
#include "Cl/ClKernel.hpp"
#include "Cl/ClProgram.hpp"
#include "OpenCGL_Tools.hpp"
#include "Particle/ParticleData.hpp"
#include "Particle/ParticleModule/AParticleModule.hpp"
#include "Particle/ParticleModule/ModuleRequired.hpp"
#include "Particle/ParticleSystem.hpp"
#include "cl_type.hpp"
#include <PathManager.hpp>

AParticleEmitter::AParticleEmitter(ParticleSystem &system, ClQueue &queue, std::string const &name, size_t nbParticle, size_t nbParticlePerSec)
    : ANommable(name),
      system_(system),
      queue_(queue),
      position_(glm::vec3(0.f)),
      nbParticleMax_(nbParticle),
      nbParticlePerSec_(nbParticlePerSec),
      nbParticleActive_(0),
      OCGLBufferEmitterParticles_(nbParticle * sizeof(ParticleData)),
      gpuBufferParam_Emitter_(ClContext::Get().context, CL_MEM_READ_WRITE, sizeof(EmitterParam)),
      gpuBufferParticles_Alive_(ClContext::Get().context, CL_MEM_READ_WRITE, nbParticle * sizeof(int)),
      gpuBufferParticles_Spawned_(ClContext::Get().context, CL_MEM_READ_WRITE, nbParticle * sizeof(int)),
      gpuBufferParticles_Death_(ClContext::Get().context, CL_MEM_READ_WRITE, nbParticle * sizeof(int)),
      gpuBufferParticles_SubLength_(ClContext::Get().context, CL_MEM_READ_WRITE, 3 * sizeof(int)),
      shouldBeSpawn_(0),
      at_(0),
      needReload_(false) {
    modules_.emplace_back(std::make_unique<ModuleRequired>(*this));
    ClProgram::Get().addProgram(PathManager::Get().getPath("particleKernels") / "Print.cl");
    updateCpuBufferParam_();
}

void AParticleEmitter::updateCpuBufferParam_() {
    cpuBufferParam_Emitter_.position = glmVec3toClFloat3(system_.getPosition());
    cpuBufferParam_Emitter_.nbMaxParticle = nbParticleMax_;
    cpuBufferParam_Emitter_.spawnParticlePerSec = nbParticlePerSec_;
}

    void AParticleEmitter::reload() {
    for (auto &module: modules_)
        module->reload();
    timerEmitter_.reset();
    timerEmitter_.start();
}

glm::vec3 &AParticleEmitter::getPosition() {
    return position_;
}
std::string const &AParticleEmitter::getName() const {
    return name_;
}
OCGL_Buffer &AParticleEmitter::getParticleOCGL_BufferData() {
    return OCGLBufferEmitterParticles_;
}
cl::Buffer &AParticleEmitter::getParticleBufferAlive() {
    return gpuBufferParticles_Alive_;
}
cl::Buffer &AParticleEmitter::getParticleBufferSpawned() {
    return gpuBufferParticles_Spawned_;
}
cl::Buffer &AParticleEmitter::getParticleBufferDeath() {
    return gpuBufferParticles_Death_;
}
cl::Buffer &AParticleEmitter::getParticleSubBuffersLength() {
    return gpuBufferParticles_SubLength_;
}
cl::Buffer &AParticleEmitter::getBufferEmitterParam() {
    return gpuBufferParam_Emitter_;
}
ParticleSystem &AParticleEmitter::getSystem() const {
    return system_;
}
size_t AParticleEmitter::getNbParticleActive_() const {
    return nbParticleActive_;
}

void AParticleEmitter::setShouldBeToSpawn(unsigned int nb, unsigned int at) {
    shouldBeSpawn_ = nb;
    at_ = at;
}

void AParticleEmitter::update(float deltaTime) {
    updateCpuBufferParam_();
    cpuBufferParam_Emitter_.position = glmVec3toClFloat3(system_.getPosition() + position_);
    cpuBufferParam_Emitter_.time = static_cast<float>(timerEmitter_.count<std::chrono::milliseconds>()) / 1000.f;
    cpuBufferParam_Emitter_.deltaTime = deltaTime;
    queue_.getQueue().enqueueWriteBuffer(gpuBufferParam_Emitter_, CL_TRUE, 0, sizeof(EmitterParam), &cpuBufferParam_Emitter_);
}

void AParticleEmitter::spawn() {
    if (shouldBeSpawn_) {
        for (auto &module: modules_) {
            module->spawn(shouldBeSpawn_, at_);
        }
    }
    shouldBeSpawn_ = 0;
    at_ += shouldBeSpawn_;
}

void AParticleEmitter::printParticleArray() {
    ClKernel kernel;
    kernel.setKernel(*this, "PrintParticle");

    kernel.beginAndSetUpdatedArgs(getParticleOCGL_BufferData().mem);
    std::cout << "printParticleArray : at : " << at_ << "for [" << nbParticleMax_ << "] particles" << std::endl;
    OpenCGL::RunKernelWithMem(queue_.getQueue(), kernel, getParticleOCGL_BufferData().mem, cl::NullRange, cl::NDRange(nbParticleMax_));
}

bool AParticleEmitter::debug_ = false;
