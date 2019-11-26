#include "AParticleEmitter.hpp"
#include "Particle/ParticleModule/AParticleModule.hpp"
#include "Particle/ParticleModule/ParticleRequiredModule.hpp"
#include "Particle/ParticleData.hpp"
#include "Cl/ClProgram.hpp"
#include "OpenCGL_Tools.hpp"

AParticleEmitter::AParticleEmitter(ParticleSystem &system, ClQueue &queue, std::string const &name, size_t nbParticle, size_t nbParticlePerSec) :
		system_(system),
		name_(name),
		queue_(queue),
		nbParticleMax_(nbParticle),
        nbParticlePerSec_(nbParticlePerSec),
		nbParticleActive_(0),
		deviceBuffer_(nbParticle * sizeof(ParticleData)),
		deviceBufferAlive_(nbParticle * sizeof(int)),
		deviceBufferAlive2_(nbParticle * sizeof(int)),
		deviceBufferDeath_(nbParticle * sizeof(int)),
		deviceBufferLengthSub_(ClContext::Get().context, CL_MEM_READ_WRITE, 3 * sizeof(int)),
		shouldBeSpawn_(0),
		at_(0),
        needReload_(false)
{
	modules_.emplace_back(std::make_unique<ParticleRequiredModule>(*this));
    ClProgram::Get().addProgram(boost::filesystem::path(ROOT_PATH) / "src" / "Particle" / "Kernel" / "Print.cl");
}

void AParticleEmitter::reload() {
    deviceBuffer_ = DeviceBuffer(nbParticleMax_ * sizeof(ParticleData));
}

std::string const &AParticleEmitter::getName() const {
	return name_;
}

DeviceBuffer &AParticleEmitter::getDeviceBuffer() {
	return deviceBuffer_;
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

void AParticleEmitter::spawn() {
	if (shouldBeSpawn_) {
		for (auto &module : modules_) {
			module->spawn(shouldBeSpawn_, at_);
		}
	}
	shouldBeSpawn_ = 0;
    at_ += shouldBeSpawn_;
}

void AParticleEmitter::printParticleArray() {
    cl::Kernel &kernel = ClProgram::Get().getKernel("PrintParticle");

    kernel.setArg(0, getDeviceBuffer().mem);
    std::cout << "printParticleArray : at : " << at_ <<  "for [" << nbParticleMax_ << "] particles" << std::endl;
    OpenCGL::RunKernelWithMem(queue_.getQueue(), kernel, getDeviceBuffer().mem, cl::NullRange, cl::NDRange(nbParticleMax_));
}

bool AParticleEmitter::debug_ = false;