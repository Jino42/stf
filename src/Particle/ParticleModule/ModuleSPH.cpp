#include "ModuleSPH.hpp"
#include "Cl/ClKernel.hpp"
#include "Cl/ClProgram.hpp"
#include "Cl/ClTools.hpp"
#include "Engine/RadixCl.hpp"
#include "OpenCGL_Tools.hpp"
#include "Particle/PaticleEmitter/AParticleEmitter.hpp"
#include "Particle/PaticleEmitter/ParticleEmitterSPH.hpp" // suppr
#include "cl_type.hpp"
#include <Engine/ModelEngine/MainGraphicExtendModel.hpp>
#include <iostream>

//
#include "Engine/Display/DisplayWindow.hpp"

ModuleSPH::ModuleSPH(AParticleEmitter &emitter)
    : AParticleModule(emitter),
      gpuBuffer_FocusPosition_(ClContext::Get().context, CL_MEM_READ_WRITE, sizeof(cl_float3)),
      gpuBuffer_HashedPosition_(ClContext::Get().context, CL_MEM_READ_WRITE, sizeof(cl_uint)),
      OCGLBufferParticles_SPH_Data_(nbParticleMax_ * sizeof(ParticleDataSPH)),
      gpuBufferParticles_CellIndex_(ClContext::Get().context, CL_MEM_READ_WRITE, nbParticleMax_ * sizeof(unsigned int)),
      gpuBufferParticles_Index_(ClContext::Get().context, CL_MEM_READ_WRITE, nbParticleMax_ * sizeof(unsigned int)),
      gpuBuffer_cellOffset_(ClContext::Get().context, CL_MEM_READ_WRITE, 128 * 128 * 64 * sizeof(unsigned int)),
      gpuBufferModuleParam_(ClContext::Get().context, CL_MEM_READ_WRITE, sizeof(ModuleParamSPH)),
      focus_(0) {
    ClProgram::Get().addProgram(pathKernel_ / "SPH.cl");
    kernelInit_.setKernel(emitter_, "SPH_Init");

    kernelDensity_.setKernel(emitter_, "SPH_UpdateDensity");
    kernelDensity_.setArgsGPUBuffers(eParticleBuffer::kData);

    kernelPressure_.setKernel(emitter_, "SPH_UpdatePressure");
    kernelPressure_.setArgsGPUBuffers(eParticleBuffer::kData);

    kernelViscosity_.setKernel(emitter_, "SPH_UpdateViscosity");
    kernelViscosity_.setArgsGPUBuffers(eParticleBuffer::kData);

    kernelUpdateCellIndex_.setKernel(emitter_, "SPH_UpdateCellIndex");
    kernelUpdateCellIndex_.setArgsGPUBuffers(eParticleBuffer::kData);

    kernelUpdateCellOffset_.setKernel(emitter_, "SPH_UpdateCellOffset");
    kernelUpdateCellOffset_.setArgsGPUBuffers(eParticleBuffer::kData);

    kernelGetHashedPosition_.setKernel(emitter_, "SPH_GetFocusPosition");
    kernelGetHashedPosition_.setArgsGPUBuffers(eParticleBuffer::kData | eParticleBuffer::kEmitterParam);

    kernelPrint_.setKernel(emitter_, "SPH_Print");
    kernelPrint_.setArgsGPUBuffers(eParticleBuffer::kData);

    cpuBufferModuleParam_.pressure = 250.f;
    cpuBufferModuleParam_.densityRef = 1.f;
    cpuBufferModuleParam_.smoothingRadius = 1.5f;
    cpuBufferModuleParam_.viscosity = 0.018f;

    ocgl_.push_back(emitter_.getParticleOCGL_BufferData().mem);
    ocgl_.push_back(OCGLBufferParticles_SPH_Data_.mem);
}

void ModuleSPH::init() {
    if (debug_)
        printf("%s\n", __FUNCTION_NAME__);
    queue_.getQueue().enqueueWriteBuffer(gpuBufferModuleParam_, CL_TRUE, 0, sizeof(ModuleParamSPH), &cpuBufferModuleParam_);
    kernelInit_.beginAndSetUpdatedArgs(OCGLBufferParticles_SPH_Data_.mem, gpuBufferModuleParam_, gpuBufferParticles_CellIndex_, gpuBufferParticles_Index_);
    OpenCGL::RunKernelWithMem(queue_.getQueue(), kernelInit_, ocgl_, cl::NullRange, cl::NDRange(nbParticleMax_));
}

void ModuleSPH::update(float deltaTime) {
    if (debug_)
        printf("%s\n", __FUNCTION_NAME__);

    queue_.getQueue().enqueueWriteBuffer(gpuBufferModuleParam_, CL_TRUE, 0, sizeof(ModuleParamSPH), &cpuBufferModuleParam_);

    /*
	size_t groups = 16;
	size_t globalWorkSize = localWorkSize * groups;

	const size_t rest = nbParticleMax_ % (groups * localWorkSize);
	globalWorkSize = (rest == 0 ? nbParticleMax_ : (nbParticleMax_ - rest + (groups * localWorkSize)));
*/

    dynamic_cast<ParticleEmitterSPH &>(emitter_).supprMe(cpuBufferModuleParam_.smoothingRadius / 2.f);

    size_t localWorkSize = 16;
    int divisor = ClContext::Get().deviceDefault.getInfo<CL_DEVICE_MAX_WORK_ITEM_SIZES>()[0];
    divisor = 640;
    for (; nbParticleMax_ % divisor != 0; divisor--)
        ;
    //std::cout << "nbParticleMax_[" << nbParticleMax_ << "] % divisor[" << divisor << "] =[" << nbParticleMax_ % divisor << "]" << std::endl;

    /*
    std::cout << "CL_DEVICE_LOCAL_MEM_SIZE : " << ClContext::Get().deviceDefault.getInfo<CL_DEVICE_LOCAL_MEM_SIZE>() << std::endl;
    std::cout << "ParticleDataSPH : " << sizeof(ParticleDataSPH) << std::endl;
    std::cout << "all : " << sizeof(ParticleDataSPH) * divisor << std::endl;
    */
    localWorkSize = divisor;
    /*
    std::cout << "nbParticleMax_ : " << divisor << std::endl;
    std::cout << "divisor : " << divisor << std::endl;
    std::cout << "localWorkSize : " << localWorkSize << std::endl;
    */
    cl::LocalSpaceArg bufferLocal = cl::Local(sizeof(ParticleDataSPH) * localWorkSize);

    ClError err;
    cl::Event ev;
    glFinish();

    err.err = queue_.getQueue().enqueueAcquireGLObjects(&ocgl_, nullptr, &ev);
    ev.wait();
    err.clCheckError();
    queue_.getQueue().finish();

    kernelUpdateCellIndex_.beginAndSetUpdatedArgs(OCGLBufferParticles_SPH_Data_.mem, gpuBufferModuleParam_, gpuBufferParticles_CellIndex_, gpuBufferParticles_Index_, gpuBuffer_cellOffset_);
    err.err = queue_.getQueue().enqueueNDRangeKernel(kernelUpdateCellIndex_.getKernel(), cl::NullRange, cl::NDRange(nbParticleMax_), cl::NDRange(localWorkSize));
    err.clCheckError();
    queue_.getQueue().finish();
    /*
    std::cout << "Before Sort : " << std::endl;
    kernelPrint_.beginAndSetUpdatedArgs(OCGLBufferParticles_SPH_Data_.mem, gpuBufferModuleParam_, gpuBufferParticles_CellIndex_, gpuBufferParticles_Index_, gpuBuffer_cellOffset_);
    err.err = queue_.getQueue().enqueueNDRangeKernel(kernelPrint_.getKernel(), cl::NullRange, cl::NDRange(nbParticleMax_), cl::NDRange(localWorkSize));
    err.clCheckError();
    queue_.getQueue().finish();
    */
    RadixCl radix;
    if (DisplayWindow::Get().getKeyState(GLFW_KEY_KP_8) == KeyState::kDown)
        radix.radix(gpuBufferParticles_CellIndex_, gpuBufferParticles_Index_, nbParticleMax_, true);
    else
        radix.radix(gpuBufferParticles_CellIndex_, gpuBufferParticles_Index_, nbParticleMax_);
    /*
    std::cout << "After Sort : " << std::endl;

    unsigned int *arrayCpuToCompare = new unsigned int[nbParticleMax_];
    unsigned int *arrayCpuToSort = new unsigned int[nbParticleMax_];

    err.err = queue_.getQueue().enqueueReadBuffer(gpuBufferParticles_CellIndex_, CL_TRUE, 0, nbParticleMax_ * sizeof(cl_uint), arrayCpuToCompare);
    err.clCheckError();
    queue_.getQueue().finish();
    err.err = queue_.getQueue().enqueueReadBuffer(gpuBufferParticles_Index_, CL_TRUE, 0, nbParticleMax_ * sizeof(cl_uint), arrayCpuToSort);
    err.clCheckError();
    queue_.getQueue().finish();

    std::cout << "PIWW" << std::endl;
    for (int i = 0; i < nbParticleMax_; i++) {
        std::cout << i << " : " << arrayCpuToCompare[arrayCpuToSort[i]] << std::endl;
    }
    
    kernelPrint_.beginAndSetUpdatedArgs(OCGLBufferParticles_SPH_Data_.mem, gpuBufferModuleParam_, gpuBufferParticles_CellIndex_, gpuBufferParticles_Index_, gpuBuffer_cellOffset_);
    err.err = queue_.getQueue().enqueueNDRangeKernel(kernelPrint_.getKernel(), cl::NullRange, cl::NDRange(nbParticleMax_), cl::NDRange(localWorkSize));
    err.clCheckError();
    queue_.getQueue().finish();
    */

    kernelUpdateCellOffset_.beginAndSetUpdatedArgs(OCGLBufferParticles_SPH_Data_.mem, gpuBufferModuleParam_, gpuBufferParticles_CellIndex_, gpuBufferParticles_Index_, gpuBuffer_cellOffset_);
    err.err = queue_.getQueue().enqueueNDRangeKernel(kernelUpdateCellOffset_.getKernel(), cl::NullRange, cl::NDRange(nbParticleMax_), cl::NDRange(localWorkSize));
    err.clCheckError();
    queue_.getQueue().finish();

    kernelDensity_.beginAndSetUpdatedArgs(OCGLBufferParticles_SPH_Data_.mem, gpuBufferModuleParam_, bufferLocal, gpuBufferParticles_CellIndex_, gpuBufferParticles_Index_, gpuBuffer_cellOffset_, focus_);
    err.err = queue_.getQueue().enqueueNDRangeKernel(kernelDensity_.getKernel(), cl::NullRange, cl::NDRange(nbParticleMax_), cl::NDRange(localWorkSize));
    err.clCheckError();
    queue_.getQueue().finish();

    kernelPressure_.beginAndSetUpdatedArgs(OCGLBufferParticles_SPH_Data_.mem, gpuBufferModuleParam_, bufferLocal);
    err.err = queue_.getQueue().enqueueNDRangeKernel(kernelPressure_.getKernel(), cl::NullRange, cl::NDRange(nbParticleMax_), cl::NDRange(localWorkSize));
    err.clCheckError();
    queue_.getQueue().finish();

    kernelViscosity_.beginAndSetUpdatedArgs(OCGLBufferParticles_SPH_Data_.mem, gpuBufferModuleParam_, bufferLocal, glmVec3toClFloat3(MainGraphicExtendModel::Get().attractorPoint));
    err.err = queue_.getQueue().enqueueNDRangeKernel(kernelViscosity_.getKernel(), cl::NullRange, cl::NDRange(nbParticleMax_), cl::NDRange(localWorkSize));
    err.clCheckError();
    queue_.getQueue().finish();

    kernelGetHashedPosition_.beginAndSetUpdatedArgs(OCGLBufferParticles_SPH_Data_.mem, gpuBuffer_FocusPosition_, gpuBuffer_HashedPosition_);
    err.err = queue_.getQueue().enqueueNDRangeKernel(kernelGetHashedPosition_.getKernel(), cl::NullRange, cl::NDRange(1), cl::NullRange);
    err.clCheckError();
    queue_.getQueue().finish();
    glm::vec4 position;
    err.err = queue_.getQueue().enqueueReadBuffer(gpuBuffer_FocusPosition_, CL_TRUE, 0, sizeof(cl_float3), &position);
    err.clCheckError();
    queue_.getQueue().finish();

    static bool h = false;
    if (!h) {
        h = true;
        aabb_.setDebug(true);
    }
    //aabb_.setMin(glm::vec3(glm::floor(position)) - glm::vec3((cpuBufferModuleParam_.smoothingRadius) / 2.f));
    //aabb_.setMax(glm::vec3(glm::floor(position)) + glm::vec3((cpuBufferModuleParam_.smoothingRadius) / 2.f));

    aabb_.setMin(glm::vec3(glm::floor(position)) - glm::vec3(1.f));
    aabb_.setMax(glm::vec3(glm::floor(position)) + glm::vec3(1.f));

    /*
    sphere_.setRadius(cpuBufferModuleParam_.smoothingRadius);
    sphere_.setCenter(position);
    */

    err.err = queue_.getQueue().enqueueReleaseGLObjects(&ocgl_, nullptr, nullptr);
    err.clCheckError();
    queue_.getQueue().finish();

    /*
	ClTools::printIntArrayGpu<ParticleDataSPH>(OCGLBufferParticles_SPH_Data_.mem, nbParticleMax_, "ParticleDataSPH", [](ParticleDataSPH *p){
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

    cl_uint lol;
    err.err = queue_.getQueue().enqueueReadBuffer(gpuBuffer_HashedPosition_, CL_TRUE, 0, sizeof(cl_uint), &lol);
    err.clCheckError();
    queue_.getQueue().finish();
    if (DisplayWindow::Get().getKeyState(GLFW_KEY_KP_5) == KeyState::kDown)
        std::cout << "Focus[" << focus_ << "] = [" << position.x << "][" << position.y << "][" << position.z << "] | Hash[" << lol << "]|" << std::endl;

    if (DisplayWindow::Get().getKeyState(GLFW_KEY_KP_ADD) == KeyState::kDown)
        focus_++;
    if (DisplayWindow::Get().getKeyState(GLFW_KEY_KP_SUBTRACT) == KeyState::kDown)
        focus_--;
}

void ModuleSPH::reload() {
    if (debug_)
        printf("%s\n", __FUNCTION_NAME__);
    init();
}