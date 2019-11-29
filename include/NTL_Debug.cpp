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

	//cl::Kernel &kernel = ClProgram::Get().getKernel("PrintSubArrayParticle");
	ClKernel kernel;
	ClError err;

	kernel.setKernel(emitter, "PrintSubArrayParticle");
	kernel.setArgsGPUBuffers(eParticleBuffer::kAllSubBuffers);

	kernel.beginAndSetUpdatedArgs(emitter.getNbParticleMax());

	queue.enqueueWriteBuffer(emitter.getParticleSubBuffersLength(), CL_TRUE, 0, sizeof(int) * 3, &emitter.indexSub_);
	err.err = queue.enqueueNDRangeKernel(kernel.getKernel(), cl::NullRange, cl::NDRange(1), cl::NullRange);
	err.clCheckError();
	queue.finish();
	queue.enqueueReadBuffer(emitter.getParticleSubBuffersLength(), CL_TRUE, 0, sizeof(int) * 3, &emitter.indexSub_);

	printf("CPU INDEX : %i %i %i\n", emitter.indexSub_[0], emitter.indexSub_[1], emitter.indexSub_[2]);
}