#include "AParticleEmitter.hpp"
#include "Particle/ParticleModule/AParticleModule.hpp"
#include "Particle/ParticleModule/ParticleRequiredModule.hpp"
#include "Particle/ParticleData.hpp"
#include "Cl/ClProgram.hpp"
#include "OpenCGL_Tools.hpp"
#include "Cl/ClKernel.hpp"

AParticleEmitter::AParticleEmitter(ParticleSystem &system, ClQueue &queue, std::string const &name, size_t nbParticle, size_t nbParticlePerSec) :
		system_(system),
		name_(name),
		queue_(queue),
		nbParticleMax_(nbParticle),
        nbParticlePerSec_(nbParticlePerSec),
		nbParticleActive_(0),
		particleOCGL_BufferData_(nbParticle * sizeof(ParticleData)),
		particleBufferAlive_(ClContext::Get().context, CL_MEM_READ_WRITE, nbParticle * sizeof(int)),
		particleBufferSpawned_(ClContext::Get().context, CL_MEM_READ_WRITE, nbParticle * sizeof(int)),
		particleBufferDeath_(ClContext::Get().context, CL_MEM_READ_WRITE, nbParticle * sizeof(int)),
		particleSubBuffersLength_(ClContext::Get().context, CL_MEM_READ_WRITE, 3 * sizeof(int)),
		shouldBeSpawn_(0),
		at_(0),
        needReload_(false)
{
	modules_.emplace_back(std::make_unique<ParticleRequiredModule>(*this));
    ClProgram::Get().addProgram(boost::filesystem::path(ROOT_PATH) / "src" / "Particle" / "Kernel" / "Print.cl");
}

void AParticleEmitter::reload() {
    particleOCGL_BufferData_ = OCGL_Buffer(nbParticleMax_ * sizeof(ParticleData));
}

std::string const &AParticleEmitter::getName() const { return name_; }
OCGL_Buffer &AParticleEmitter::getParticleOCGL_BufferData() { return particleOCGL_BufferData_; }
cl::Buffer &AParticleEmitter::getParticleBufferAlive() { return particleBufferAlive_; }
cl::Buffer &AParticleEmitter::getParticleBufferSpawned() { return particleBufferSpawned_; }
cl::Buffer &AParticleEmitter::getParticleBufferDeath() { return particleBufferDeath_; }
cl::Buffer &AParticleEmitter::getParticleSubBuffersLength() { return particleSubBuffersLength_; }
ParticleSystem &AParticleEmitter::getSystem() const { return system_; }
size_t AParticleEmitter::getNbParticleActive_() const { return nbParticleActive_; }

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
    ClKernel kernel("PrintParticle");

    kernel.setArgs(getParticleOCGL_BufferData().mem);
    std::cout << "printParticleArray : at : " << at_ <<  "for [" << nbParticleMax_ << "] particles" << std::endl;
    OpenCGL::RunKernelWithMem(queue_.getQueue(), kernel, getParticleOCGL_BufferData().mem, cl::NullRange, cl::NDRange(nbParticleMax_));
}

bool AParticleEmitter::debug_ = false;


