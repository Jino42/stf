#include "ModuleTarget.hpp"
#include "Particle/ParticleSystem.hpp"
#include "Particle/PaticleEmitter/AParticleEmitter.hpp"
#include "Engine/Random.hpp"
#include "Cl/ClContext.hpp"
#include "Cl/ClProgram.hpp"
#include "Cl/ClQueue.hpp"
#include "OpenCGL_Tools.hpp"
#include "cl_type.hpp"

ModuleTarget::ModuleTarget(AParticleEmitter &emitter) :
		AParticleModule(emitter) {
    gpuBufferParticles_Target_ = emitter_.addClBuffer<ModuleTarget>();
    *gpuBufferParticles_Target_ = cl::Buffer(ClContext::Get().context, CL_MEM_WRITE_ONLY, nbParticleMax_ * sizeof(ParticleDataTarget));
}