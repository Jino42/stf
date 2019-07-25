#include "ParticleEmitterSprite.hpp"
#include "Engine/Shader.hpp"
#include "Cl/ClQueue.hpp"
#include "Particle/ParticleData.hpp"
#include "Particle/ParticleModule/ParticleAttractorModule.hpp"
#include "Particle/ParticleModule/ParticleSpawnModule.hpp"
#include "Engine/MainGraphic.hpp"
#include "Cl/ClProgram.hpp"
#include "OpenCGL_Tools.hpp"
#include "cl_type.hpp"
#include <Engine/Camera.hpp>


ParticleEmitterSprite::ParticleEmitterSprite(ParticleSystem &system, ClQueue &queue, std::string const &name, size_t nbParticlePerSec, size_t nbParticleMax) :
		AParticleEmitter(system, queue, name, nbParticleMax, nbParticlePerSec),
		deviceBufferSpriteData_(nbParticleMax * sizeof(ParticleSpriteData)),
		atlas_("bloup.png", boost::filesystem::path(ROOT_PATH) / "resources" / "atlas/", 4),
		distBuffer_(ClContext::Get().context, CL_MEM_WRITE_ONLY, nbParticleMax * sizeof(CL_FLOAT)),
		nbParticleActiveOutpourBuffer_(ClContext::Get().context, CL_MEM_READ_WRITE, sizeof(int))
{

	modules_.emplace_back(std::make_unique<ParticleSpawnModule>(*this));

	ClProgram &program = ClProgram::Get();
	program.addProgram(boost::filesystem::path(ROOT_PATH) / "src" / "Kernel" / "Sort.cl");
	program.addProgram(boost::filesystem::path(ROOT_PATH) / "src" / "Kernel" / "Sprite.cl");

	shader_.attach((boost::filesystem::path(ROOT_PATH) / "shader" / "particleSprite.vert").generic_string());
	shader_.attach((boost::filesystem::path(ROOT_PATH) / "shader" / "particleSprite.frag").generic_string());
	shader_.link();

	float data[] = {
			// positions          // normal           // texture coords
			0.5f,  0.5f, 0.0f,   0.f, 0.f, 1.f,   1.0f, 1.0f, // top right
			0.5f, -0.5f, 0.0f,   0.f, 0.f, 1.f,   1.0f, 0.0f, // bottom right
			-0.5f, -0.5f, 0.0f,   0.f, 0.f, 1.f,   0.0f, 0.0f, // bottom left
			-0.5f,  0.5f, 0.0f,   0.f, 0.f, 1.f,   0.0f, 1.0f  // top left
	};
	unsigned int indices[] = {  // note that we start from 0!
			0, 1, 3,  // first Triangle
			1, 2, 3   // second Triangle
	};
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)nullptr);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	reload();
	glBindVertexArray(0);
}

void ParticleEmitterSprite::reload() {
	if (debug_)
		printf("%s\n", __FUNCTION_NAME__);
    AParticleEmitter::reload();

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, deviceBuffer_.vbo);

    glEnableVertexAttribArray(3); //Position
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleData), reinterpret_cast<const void *>(offsetof(ParticleData, position)));
    glEnableVertexAttribArray(4); //Velocity
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleData), reinterpret_cast<const void *>(offsetof(ParticleData, velocity)));
    glEnableVertexAttribArray(5); //Color
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(ParticleData), reinterpret_cast<const void *>(offsetof(ParticleData, color)));
    glEnableVertexAttribArray(6); //Rotate
    glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleData), reinterpret_cast<const void *>(offsetof(ParticleData, rotate)));
    glEnableVertexAttribArray(7); //Size
    glVertexAttribPointer(7, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleData), reinterpret_cast<const void *>(offsetof(ParticleData, size)));
    glEnableVertexAttribArray(8); //Age
    glVertexAttribPointer(8, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleData), reinterpret_cast<const void *>(offsetof(ParticleData, age)));
    glEnableVertexAttribArray(9); //LifeTime
    glVertexAttribPointer(9, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleData), reinterpret_cast<const void *>(offsetof(ParticleData, lifeTime)));

    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);
    glVertexAttribDivisor(7, 1);
    glVertexAttribDivisor(8, 1);
    glVertexAttribDivisor(9, 1);


    deviceBufferSpriteData_ = DeviceBuffer(nbParticleMax_ * sizeof(ParticleSpriteData));
    glBindBuffer(GL_ARRAY_BUFFER, deviceBufferSpriteData_.vbo);

    glEnableVertexAttribArray(10); //Offset1
    glVertexAttribPointer(10, 2, GL_FLOAT, GL_FALSE, sizeof(ParticleSpriteData), reinterpret_cast<const void *>(offsetof(ParticleSpriteData, offset1)));
    glEnableVertexAttribArray(11); //Offset2
    glVertexAttribPointer(11, 2, GL_FLOAT, GL_FALSE, sizeof(ParticleSpriteData), reinterpret_cast<const void *>(offsetof(ParticleSpriteData, offset2)));
    glEnableVertexAttribArray(12); //Blend
    glVertexAttribPointer(12, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleSpriteData), reinterpret_cast<const void *>(offsetof(ParticleSpriteData, blend)));

    glVertexAttribDivisor(10, 1);
    glVertexAttribDivisor(11, 1);
    glVertexAttribDivisor(12, 1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    distBuffer_ = cl::Buffer(ClContext::Get().context, CL_MEM_READ_WRITE, nbParticleMax_ * sizeof(CL_FLOAT));

}

void ParticleEmitterSprite::init() {
	if (debug_)
		printf("%s\n", __FUNCTION_NAME__);
	for (auto &module : modules_) {
		module->init();
	}
}

void	ParticleEmitterSprite::updateSpriteData() {
	if (debug_)
		printf("%s\n", __FUNCTION_NAME__);
	ClError err;
	std::vector<cl::Memory> cl_vbos;
	cl::Kernel &kernel = ClProgram::Get().getKernel("sprite");

	cl_vbos.push_back(deviceBuffer_.mem);
	cl_vbos.push_back(deviceBufferSpriteData_.mem);

	kernel.setArg(0, deviceBuffer_.mem);
	kernel.setArg(1, deviceBufferSpriteData_.mem);
	kernel.setArg(2, atlas_.getNumberOfRows());

	OpenCGL::RunKernelWithMem(queue_.getQueue(), kernel, cl_vbos, cl::NullRange, cl::NDRange(nbParticleMax_));
}

void ParticleEmitterSprite::update(float deltaTime) {
    if (debug_)
        printf("%s\n", __FUNCTION_NAME__);
    checkReload();
    updateSpriteData();
    getNbParticleActive_();
    for (auto &module : modules_) {
        module->update(deltaTime);
    }
    sortDeviceBuffer_();
    getNbParticleActive_();
}

void ParticleEmitterSprite::render() {
	if (debug_)
		printf("%s\n", __FUNCTION_NAME__);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDepthMask(GL_FALSE);
	shader_.activate();
	shader_.setMat4("projection", Camera::Get().getProjectionMatrix());
	shader_.setMat4("view", Camera::Get().getViewMatrix());
	shader_.setUInt("numberOfRows", atlas_.getNumberOfRows());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, atlas_.getAtlasId());

	glBindVertexArray(VAO);
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, nbParticleActive_);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_TRUE);
}


//Need sorted GPU Buffer for get NbParticleActive
void ParticleEmitterSprite::getNbParticleActive_() {

	if (debug_)
		printf("%s\n", __FUNCTION_NAME__);
	cl::Kernel &kernel = ClProgram::Get().getKernel("getNbParticleActiveSafe");
	kernel.setArg(0, deviceBuffer_.mem);
	kernel.setArg(1, distBuffer_);
	kernel.setArg(2, nbParticleActiveOutpourBuffer_);
	kernel.setArg(3, nbParticleMax_);

    OpenCGL::RunKernelWithMem(queue_.getQueue(), kernel, deviceBuffer_.mem, cl::NullRange, cl::NDRange(1));
    queue_.getQueue().enqueueReadBuffer(nbParticleActiveOutpourBuffer_, CL_TRUE, 0, sizeof(int), &nbParticleActive_);

}

void ParticleEmitterSprite::sortDeviceBufferCalculateDistanceParticle_() {
	if (debug_)
		printf("%s\n", __FUNCTION_NAME__);
	cl::Kernel &kernel = ClProgram::Get().getKernel("calculateDistanceBetweenParticleAndCamera");

	kernel.setArg(0, deviceBuffer_.mem);
	glm::vec3 cameraPosition = Camera::Get().getPosition();
	kernel.setArg(1, distBuffer_);
	kernel.setArg(2, glmVec3toClFloat3(cameraPosition));

    OpenCGL::RunKernelWithMem(queue_.getQueue(), kernel, deviceBuffer_.mem, cl::NullRange, cl::NDRange(nbParticleMax_));
}

void ParticleEmitterSprite::sortDeviceBuffer_() {
	sortDeviceBufferCalculateDistanceParticle_();
	if (debug_)
		printf("%s\n", __FUNCTION_NAME__);

	cl::Kernel &kernel = ClProgram::Get().getKernel("ParallelSelection");
	//cl::Kernel &kernel = programSort_.getKernel("ParallelSelection_Blocks");
	//cl::Kernel &kernel = programSort_.getKernel("ParallelMerge_Local");

	kernel.setArg(0, deviceBuffer_.mem);
	std::cout << "6" << std::endl;
	kernel.setArg(1, deviceBufferSpriteData_.mem);
	std::cout << "7" << std::endl;
	//size_t localWorkGroupeSize;
	//kernel.getWorkGroupInfo(ClContext::Get().deviceDefault, CL_KERNEL_WORK_GROUP_SIZE, &localWorkGroupeSize);
	//localWorkGroupeSize /= 2;
	//int blockFactor = 2;
	glm::vec3 cameraPosition = Camera::Get().getPosition();
	kernel.setArg(2, distBuffer_);
	std::cout << "8" << std::endl;
	cl_float3 temp;
	temp.x = cameraPosition.x;
	temp.y = cameraPosition.y;
	temp.z = cameraPosition.z;
	kernel.setArg(3, temp);
	std::cout << "9" << std::endl;
	//kernel.setArg(1, sizeof(ParticleData) * localWorkGroupeSize /** blockFactor*/, nullptr);
	//kernel.setArg(2, cameraPosition);
	//kernel.setArg(3, blockFactor);

	ClError err;
	std::vector<cl::Memory> cl_vbos;

	cl_vbos.push_back(deviceBuffer_.mem);
	cl_vbos.push_back(deviceBufferSpriteData_.mem);

	OpenCGL::RunKernelWithMem(queue_.getQueue(), kernel, cl_vbos, cl::NullRange, cl::NDRange(nbParticleMax_));
}