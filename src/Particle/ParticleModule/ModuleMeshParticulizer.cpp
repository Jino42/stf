#include "ModuleMeshParticulizer.hpp"
#include "Particle/ParticleSystem.hpp"
#include "Particle/PaticleEmitter/AParticleEmitter.hpp"
#include "Engine/Random.hpp"
#include "Cl/ClContext.hpp"
#include "Cl/ClProgram.hpp"
#include "Cl/ClQueue.hpp"
#include "OpenCGL_Tools.hpp"
#include "cl_type.hpp"
#include "ModuleTarget.hpp"
#include <PathManager.hpp>

ModuleMeshParticulizer::ModuleMeshParticulizer(AParticleEmitter &emitter) :
	AParticleModule(emitter),
	cpuBufferParticles_Position_(std::make_unique<ParticleDataMeshParticulizer[]>(nbParticleMax_))
	{

	if (!emitter_.contain<ModuleTarget>()) {
		emitter_.addModule<ModuleTarget>();
	}
	moduleTarget_ = emitter_.getModule<ModuleTarget>();

	model_.setModel((PathManager::Get().getPath("objects") / "nanosuit" / "nanosuit.obj").generic_string());

		ClProgram::Get().addProgram(pathKernel_ / "MeshParticulizer.cl");

		kernelInit_.setKernel(emitter_, "initMeshParticulizer");
		kernelInit_.setArgsGPUBuffers(eParticleBuffer::kData);
}

void ModuleMeshParticulizer::init() {
	if (debug_)
		printf("%s\n", __FUNCTION_NAME__);
	for (int i = 0; i < nbParticleMax_; i++) {
		int sz;
		sz = Random::Get().getRandomSeed();

		Mesh const &mesh = model_.getMeshes()[sz % model_.getMeshes().size()];


		sz = Random::Get().getRandomSeed();
		int vexterStartTriangle = sz % (mesh.getIndice().size() / 3);

		glm::vec3 positionA = mesh.getVertice()[mesh.getIndice()[vexterStartTriangle * 3 + 0]].position;
		glm::vec3 positionB = mesh.getVertice()[mesh.getIndice()[vexterStartTriangle * 3 + 1]].position;
		glm::vec3 positionC = mesh.getVertice()[mesh.getIndice()[vexterStartTriangle * 3 + 2]].position;


		float a = Random::Get().getRandom01();
		float b = Random::Get().getRandom01();
		if (a + b > 1.f) {
			a = 1.f - a;
			b = 1.f - b;
		}
		glm::vec3 final = emitter_.getSystem().getPosition() + positionA + a * (positionB - positionA) + b * (positionC - positionA);

		cpuBufferParticles_Position_[i].position.x = final.x;
		cpuBufferParticles_Position_[i].position.y = final.y;
		cpuBufferParticles_Position_[i].position.z = final.z;
	}

//	kernelInit_.beginAndSetUpdatedArgs(bufferPositionParticles_, glmVec3toClFloat3(emitter_.getSystem().getPosition()));
//	std::vector<cl::Memory> cl_vbos;
//	cl_vbos.push_back(emitter_.getParticleOCGL_BufferData().mem);
//	OpenCGL::RunKernelWithMem(queue_.getQueue(), kernelInit_, cl_vbos, cl::NullRange, cl::NDRange(nbParticleMax_));


	for (int i = 0; i < nbParticleMax_; i++)
		moduleTarget_->getCpuBuffer()[i].target = cpuBufferParticles_Position_[i].position;
	moduleTarget_->writeDataToGPU_Buffer();
}




