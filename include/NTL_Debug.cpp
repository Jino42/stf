#include "NTL_Debug.hpp"
#include <Cl/ClProgram.hpp>
#include "OpenCGL_Tools.hpp"

void printStructSizeCPU() {
	printf(">>> CPU SIZE STRUCT <<< \n");
	printf("[%5li] ModuleParamRequired \n", sizeof(ModuleParamRequired));
	printf("[%5li] ModuleParamSpawn \n", sizeof(ModuleParamSpawn));
	printf("[%5li] ModuleParamSizeOverLifetime \n", sizeof(ModuleParamSizeOverLifetime));
	printf("[%5li] Rangef \n", sizeof(Rangef));
	printf("[%5li] Rangei \n", sizeof(Rangei));
	printf("[%5li] ParticleData \n", sizeof(ParticleData));
	printf("[%5li] ParticleDataSprite \n", sizeof(ParticleDataSprite));
	printf("[%5li] ParticleDataMovement \n", sizeof(ParticleDataMovement));
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