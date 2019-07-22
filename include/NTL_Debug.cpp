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
	cl::Kernel &kernel = ClProgram::Get().getKernel("printStructSize");

	ClError err;
	printf(">>> GPU SIZE STRUCT <<< \n");


    kernel.setArg(0, emitter.getDeviceBuffer().mem);
    OpenCGL::RunKernelWithMem(queue, kernel, emitter.getDeviceBuffer().mem, cl::NullRange, cl::NDRange(1));

    /*err.err = queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(1), cl::NullRange);
    err.clCheckError();
    queue.finish();
     */
}