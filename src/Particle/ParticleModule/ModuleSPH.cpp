#include "ModuleSPH.hpp"
#include <iostream>
#include "Cl/ClProgram.hpp"
#include "OpenCGL_Tools.hpp"
#include "Cl/ClKernel.hpp"
#include "Particle/PaticleEmitter/AParticleEmitter.hpp"
#include "Cl/ClTools.hpp"

ModuleSPH::ModuleSPH(AParticleEmitter &emitter) :
		AParticleModule(emitter),
		gpuBufferParticles_SPH_(ClContext::Get().context, CL_MEM_READ_WRITE, sizeof(ParticleDataSPH) * nbParticleMax_),
		gpuBufferModuleParam_(ClContext::Get().context, CL_MEM_READ_WRITE, sizeof(ModuleParamSPH))
{
	ClProgram::Get().addProgram(pathKernel_ / "SPH.cl");
	kernelInit_.setKernel(emitter_, "SPH_Init");


	kernelDensity_.setKernel(emitter_, "SPH_UpdateDensity");
	kernelDensity_.setArgsGPUBuffers(eParticleBuffer::kData);

	kernelPressure_.setKernel(emitter_, "SPH_UpdatePressure");
	kernelPressure_.setArgsGPUBuffers(eParticleBuffer::kData);

	kernelViscosity_.setKernel(emitter_, "SPH_UpdateViscosity");
	kernelViscosity_.setArgsGPUBuffers(eParticleBuffer::kData);
}

void	ModuleSPH::init() {
	if (debug_)
		printf("%s\n", __FUNCTION_NAME__);
	queue_.getQueue().enqueueWriteBuffer(gpuBufferModuleParam_, CL_TRUE, 0, sizeof(ModuleParamSPH), &cpuBufferModuleParam_);
	kernelInit_.beginAndSetUpdatedArgs(gpuBufferParticles_SPH_, gpuBufferModuleParam_);
	OpenCGL::RunKernelWithMem(queue_.getQueue(), kernelInit_, emitter_.getParticleOCGL_BufferData().mem, cl::NullRange, cl::NDRange(nbParticleMax_));
}

void	ModuleSPH::update(float deltaTime) {
	if (debug_)
		printf("%s\n", __FUNCTION_NAME__);

	queue_.getQueue().enqueueWriteBuffer(gpuBufferModuleParam_, CL_TRUE, 0, sizeof(ModuleParamSPH), &cpuBufferModuleParam_);

/*
	size_t groups = 16;
	size_t globalWorkSize = localWorkSize * groups;

	const size_t rest = nbParticleMax_ % (groups * localWorkSize);
	globalWorkSize = (rest == 0 ? nbParticleMax_ : (nbParticleMax_ - rest + (groups * localWorkSize)));
*/
	size_t localWorkSize = 16;
	int divisor = ClContext::Get().deviceDefault.getInfo<CL_DEVICE_MAX_WORK_ITEM_SIZES>()[0];
	for ( ; nbParticleMax_ % divisor != 0 ; divisor--)
		;


	localWorkSize = divisor;
	std::cout << "divisor : " << divisor << std::endl;
	std::cout << "localWorkSize : " << localWorkSize << std::endl;

	cl::LocalSpaceArg bufferLocal = cl::Local(sizeof(ParticleDataSPH) * localWorkSize);


	kernelDensity_.beginAndSetUpdatedArgs(gpuBufferParticles_SPH_, gpuBufferModuleParam_, bufferLocal);

	ClError err;
	std::vector<cl::Memory> cl_vbos;

	cl_vbos.push_back(emitter_.getParticleOCGL_BufferData().mem);
	err.clCheckError();

	cl::Event ev;
	glFinish();
	err.err = queue_.getQueue().enqueueAcquireGLObjects(&cl_vbos, nullptr, &ev);
	ev.wait();
	err.clCheckError();
	queue_.getQueue().finish();
	err.err = queue_.getQueue().enqueueNDRangeKernel(kernelDensity_.getKernel(), cl::NullRange, cl::NDRange(nbParticleMax_), cl::NDRange(localWorkSize));
	err.clCheckError();
	queue_.getQueue().finish();
	err.err = queue_.getQueue().enqueueReleaseGLObjects(&cl_vbos, nullptr, nullptr);
	err.clCheckError();
	queue_.getQueue().finish();


	kernelPressure_.beginAndSetUpdatedArgs(gpuBufferParticles_SPH_, gpuBufferModuleParam_, bufferLocal);

	glFinish();
	err.err = queue_.getQueue().enqueueAcquireGLObjects(&cl_vbos, nullptr, &ev);
	ev.wait();
	err.clCheckError();
	queue_.getQueue().finish();
	err.err = queue_.getQueue().enqueueNDRangeKernel(kernelPressure_.getKernel(), cl::NullRange, cl::NDRange(nbParticleMax_), cl::NDRange(localWorkSize));
	err.clCheckError();
	queue_.getQueue().finish();
	err.err = queue_.getQueue().enqueueReleaseGLObjects(&cl_vbos, nullptr, nullptr);
	err.clCheckError();
	queue_.getQueue().finish();


	kernelViscosity_.beginAndSetUpdatedArgs(gpuBufferParticles_SPH_, gpuBufferModuleParam_, bufferLocal);

	glFinish();
	err.err = queue_.getQueue().enqueueAcquireGLObjects(&cl_vbos, nullptr, &ev);
	ev.wait();
	err.clCheckError();
	queue_.getQueue().finish();
	err.err = queue_.getQueue().enqueueNDRangeKernel(kernelViscosity_.getKernel(), cl::NullRange, cl::NDRange(nbParticleMax_), cl::NDRange(localWorkSize));
	err.clCheckError();
	queue_.getQueue().finish();
	err.err = queue_.getQueue().enqueueReleaseGLObjects(&cl_vbos, nullptr, nullptr);
	err.clCheckError();
	queue_.getQueue().finish();
/*
	ClTools::printIntArrayGpu<ParticleDataSPH>(gpuBufferParticles_SPH_, nbParticleMax_, "ParticleDataSPH", [](ParticleDataSPH *p){
		std::cout << "position[x:" << p->position.x << ", y:" << p->position.y << ", z:" << p->position.z << "]";
		std::cout << " - | - ";
		std::cout << "density[" << p->density << "]";
		std::cout << " - | - ";
		std::cout << "pressure[" << p->pressure << "]";
		std::cout << " - | - ";
		std::cout << "mass[" << p->mass << "]";
		std::cout << " - | - ";
		std::cout << "force1[x:" << p->force1.x << ", y:" << p->force1.y << ", z:" << p->force1.z << "]";
		std::cout << " - | - ";
		std::cout << "force2[x:" << p->force2.x << ", y:" << p->force2.y << ", z:" << p->force2.z << "]";
		std::cout << std::endl;

	});
	*/
	//exit(0);
}

void    ModuleSPH::reload()
{
	if (debug_)
		printf("%s\n", __FUNCTION_NAME__);
	queue_.getQueue().enqueueWriteBuffer(gpuBufferModuleParam_, CL_TRUE, 0, sizeof(ModuleParamSPH), &cpuBufferModuleParam_);
	init();
}