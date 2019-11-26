#include "NTL_Debug.hpp"
#include <Cl/ClProgram.hpp>
#include "OpenCGL_Tools.hpp"

void printStructSizeCPU() {
	printf(">>> CPU SIZE STRUCT <<< \n");
	printf("[%5li] ModuleRequiredParams \n", sizeof(ModuleRequiredParams));
	printf("[%5li] ModuleSpawnParams \n", sizeof(ModuleSpawnParams));
	printf("[%5li] ModuleSizeOverLifetimeParams \n", sizeof(ModuleSizeOverLifetimeParams));
	printf("[%5li] Rangef \n", sizeof(Rangef));
	printf("[%5li] Rangei \n", sizeof(Rangei));
	printf("[%5li] ParticleData \n", sizeof(ParticleData));
	printf("[%5li] ParticleSpriteData \n", sizeof(ParticleSpriteData));
	printf("[%5li] ParticleMovementModuleData \n", sizeof(ParticleMovementModuleData));
}

void printStructSizeGPU(AParticleEmitter &emitter, cl::CommandQueue &queue) {

	printf("CPU SIDE DEBUG >>> GPU SIZE STRUCT <<< \n");
	ClError err;


/*
	cl::Kernel &kernel = ClProgram::Get().getKernel("printStructSize");



    kernel.setArg(0, emitter.getDeviceBuffer().mem);
    OpenCGL::RunKernelWithMem(queue, kernel, emitter.getDeviceBuffer().mem, cl::NullRange, cl::NDRange(1));
	queue.finish();
*/
    cl::Kernel &kernel = ClProgram::Get().getKernel("printStructSizeFunc");
    err.err = queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(1), cl::NullRange);
    err.clCheckError();
    queue.finish();
}

void printStructSizeGPUBase(cl::CommandQueue &queue) {

	printf("CPU SIDE DEBUG >>> GPU SIZE STRUCT <<< \n");
	ClError err;


/*
	cl::Kernel &kernel = ClProgram::Get().getKernel("printStructSize");



    kernel.setArg(0, emitter.getDeviceBuffer().mem);
    OpenCGL::RunKernelWithMem(queue, kernel, emitter.getDeviceBuffer().mem, cl::NullRange, cl::NDRange(1));
	queue.finish();
*/
	cl::Kernel &kernel = ClProgram::Get().getKernel("printStructSizeFunc");
	err.err = queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(1), cl::NullRange);
	err.clCheckError();
	queue.finish();
}

void printSubArrayParticle(AParticleEmitter &emitter, cl::CommandQueue &queue) {
/*
	cl::Kernel &kernel = ClProgram::Get().getKernel("PrintSubArrayParticle");
	kernel.setArg(0, emitter.deviceBufferAlive_.mem);
	kernel.setArg(1, emitter.deviceBufferAlive2_.mem);
	kernel.setArg(2, emitter.deviceBufferDeath_.mem);
	kernel.setArg(3, emitter.deviceBufferLengthSub_);
	kernel.setArg(4, emitter.getNbParticleMax());

	std::vector<cl::Memory> cl_vbos2;
	cl_vbos2.push_back(emitter.deviceBufferAlive_.mem);
	cl_vbos2.push_back(emitter.deviceBufferAlive2_.mem);
	cl_vbos2.push_back(emitter.deviceBufferDeath_.mem);


	queue.enqueueWriteBuffer(emitter.deviceBufferLengthSub_, CL_TRUE, 0, sizeof(int) * 3, &emitter.indexSub_);
	OpenCGL::RunKernelWithMem(queue, kernel, cl_vbos2, cl::NullRange, cl::NDRange(1));
	queue.enqueueReadBuffer(emitter.deviceBufferLengthSub_, CL_TRUE, 0, sizeof(int) * 3, &emitter.indexSub_);
*/
	printf("CPU INDEX : %i %i %i\n", emitter.indexSub_[0], emitter.indexSub_[1], emitter.indexSub_[2]);
}