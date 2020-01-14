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
#include <Gui/Gui.hpp>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>

//
#include "Engine/Display/DisplayWindow.hpp"

ModuleSPH::ModuleSPH(AParticleEmitter &emitter)
    : AParticleModule(emitter),
      flag_(1 << 2),
      OCGLBufferParticles_SPH_Data_(nbParticleMax_ * sizeof(ParticleDataSPH)),
      gpuBufferParticles_CellIndex_(ClContext::Get().context, CL_MEM_READ_WRITE, nbParticleMax_ * sizeof(unsigned int)),
      gpuBufferParticles_Index_(ClContext::Get().context, CL_MEM_READ_WRITE, nbParticleMax_ * sizeof(unsigned int)),
      gpuBuffer_cellOffset_(ClContext::Get().context, CL_MEM_READ_WRITE, 128 * 128 * 64 * sizeof(unsigned int)),
      gpuBufferModuleParam_(ClContext::Get().context, CL_MEM_READ_WRITE, sizeof(ModuleParamSPH)),
      lastSmoothingRadius_(0.0f),
      focus_(0) {
    ClProgram::Get().addProgram(pathKernel_ / "SPH.cl");
    kernelInit_.setKernel(emitter_, "SPH_Init");

    queue_.addEvent("SPH_WriteModuleParam");
    queue_.addEvent("SPH_FillBuffer_cellOffset");
    queue_.addEvent("SPH_enqueueAcquireGLObjects");
    queue_.addEvent("SPH_enqueueReleaseGLObjects");

    kernelDensity_.setKernel(emitter_, "SPH_UpdateDensity");
    kernelDensity_.setArgsGPUBuffers(eParticleBuffer::kData);
    queue_.addEvent("SPH_UpdateDensity");

    kernelPressure_.setKernel(emitter_, "SPH_UpdatePressure");
    kernelPressure_.setArgsGPUBuffers(eParticleBuffer::kData);
    queue_.addEvent("SPH_UpdatePressure");

    kernelViscosity_.setKernel(emitter_, "SPH_UpdateViscosity");
    kernelViscosity_.setArgsGPUBuffers(eParticleBuffer::kData);
    queue_.addEvent("SPH_UpdateViscosity");

    kernelUpdateCellIndex_.setKernel(emitter_, "SPH_UpdateCellIndex");
    kernelUpdateCellIndex_.setArgsGPUBuffers(eParticleBuffer::kData);
    queue_.addEvent("SPH_UpdateCellIndex");

    kernelUpdateCellOffset_.setKernel(emitter_, "SPH_UpdateCellOffset");
    kernelUpdateCellOffset_.setArgsGPUBuffers(eParticleBuffer::kData);
    queue_.addEvent("SPH_UpdateCellOffset");

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

void ModuleSPH::updateConstant_() {
    if (lastSmoothingRadius_ != cpuBufferModuleParam_.smoothingRadius) {
        lastSmoothingRadius_ = cpuBufferModuleParam_.smoothingRadius;

        cpuBufferModuleParam_.h2 = powf(cpuBufferModuleParam_.smoothingRadius, 2.f);
        cpuBufferModuleParam_.h3 = powf(cpuBufferModuleParam_.smoothingRadius, 3.f);
        cpuBufferModuleParam_.h6 = powf(cpuBufferModuleParam_.smoothingRadius, 6.f);
        cpuBufferModuleParam_.h9 = powf(cpuBufferModuleParam_.smoothingRadius, 9.f);
        cpuBufferModuleParam_.Poly6 = (315.0f / (64.0f * M_PI * cpuBufferModuleParam_.h9));
        cpuBufferModuleParam_.Spiky = (-45.0f / (M_PI * cpuBufferModuleParam_.h6));
    }
}

void ModuleSPH::update(float deltaTime) {
    if (debug_)
        printf("%s\n", __FUNCTION_NAME__);

    updateConstant_();

    ClError err;
    err.err = queue_.getQueue().enqueueWriteBuffer(gpuBufferModuleParam_, CL_TRUE, 0, sizeof(ModuleParamSPH), &cpuBufferModuleParam_, nullptr, &queue_.getEvent("SPH_WriteModuleParam"));
    err.clCheckError();
    queue_.getEvent("SPH_WriteModuleParam").wait();

    dynamic_cast<ParticleEmitterSPH &>(emitter_).supprMe(cpuBufferModuleParam_.smoothingRadius / 2.f);

    glFinish();

    err.err = queue_.getQueue().enqueueAcquireGLObjects(&ocgl_, nullptr, &queue_.getEvent("SPH_enqueueAcquireGLObjects"));
    err.clCheckError();
    queue_.getEvent("SPH_enqueueAcquireGLObjects").wait();

    err.err |= queue_.getQueue().enqueueFillBuffer<cl_uint>(gpuBuffer_cellOffset_, UINT_MAX, 0, sizeof(cl_uint) * 128 * 128 * 64, nullptr, &queue_.getEvent("SPH_FillBuffer_cellOffset"));
    err.clCheckError();
    queue_.getEvent("SPH_FillBuffer_cellOffset").wait();

    kernelUpdateCellIndex_.beginAndSetUpdatedArgs(OCGLBufferParticles_SPH_Data_.mem, gpuBufferModuleParam_, gpuBufferParticles_CellIndex_, gpuBufferParticles_Index_, gpuBuffer_cellOffset_);
    err.err = queue_.getQueue().enqueueNDRangeKernel(kernelUpdateCellIndex_.getKernel(), cl::NullRange, cl::NDRange(nbParticleMax_), cl::NullRange, nullptr, &queue_.getEvent("SPH_UpdateCellIndex"));
    err.clCheckError();
    queue_.getEvent("SPH_UpdateCellIndex").wait();

    RadixCl radix;
    radix.radix(gpuBufferParticles_CellIndex_, gpuBufferParticles_Index_, nbParticleMax_);

    kernelUpdateCellOffset_.beginAndSetUpdatedArgs(OCGLBufferParticles_SPH_Data_.mem, gpuBufferModuleParam_, gpuBufferParticles_CellIndex_, gpuBufferParticles_Index_, gpuBuffer_cellOffset_);
    err.err = queue_.getQueue().enqueueNDRangeKernel(kernelUpdateCellOffset_.getKernel(), cl::NullRange, cl::NDRange(nbParticleMax_), cl::NullRange, nullptr, &queue_.getEvent("SPH_UpdateCellOffset"));
    err.clCheckError();
    queue_.getEvent("SPH_UpdateCellOffset").wait();

    kernelDensity_.beginAndSetUpdatedArgs(OCGLBufferParticles_SPH_Data_.mem, gpuBufferModuleParam_, gpuBufferParticles_CellIndex_, gpuBufferParticles_Index_, gpuBuffer_cellOffset_, focus_);
    err.err = queue_.getQueue().enqueueNDRangeKernel(kernelDensity_.getKernel(), cl::NullRange, cl::NDRange(nbParticleMax_), cl::NullRange, nullptr, &queue_.getEvent("SPH_UpdateDensity"));
    err.clCheckError();
    queue_.getEvent("SPH_UpdateDensity").wait();

    kernelPressure_.beginAndSetUpdatedArgs(OCGLBufferParticles_SPH_Data_.mem, gpuBufferModuleParam_, gpuBufferParticles_CellIndex_, gpuBufferParticles_Index_, gpuBuffer_cellOffset_);
    err.err = queue_.getQueue().enqueueNDRangeKernel(kernelPressure_.getKernel(), cl::NullRange, cl::NDRange(nbParticleMax_), cl::NullRange, nullptr, &queue_.getEvent("SPH_UpdatePressure"));
    err.clCheckError();
    queue_.getEvent("SPH_UpdatePressure").wait();

    kernelViscosity_.beginAndSetUpdatedArgs(OCGLBufferParticles_SPH_Data_.mem, gpuBufferModuleParam_, gpuBufferParticles_CellIndex_, gpuBufferParticles_Index_, gpuBuffer_cellOffset_, glmVec3toClFloat3(MainGraphicExtendModel::Get().attractorPoint));
    err.err = queue_.getQueue().enqueueNDRangeKernel(kernelViscosity_.getKernel(), cl::NullRange, cl::NDRange(nbParticleMax_), cl::NullRange, nullptr, &queue_.getEvent("SPH_UpdateViscosity"));
    err.clCheckError();
    queue_.getEvent("SPH_UpdateViscosity").wait();

    err.err = queue_.getQueue().enqueueReleaseGLObjects(&ocgl_, nullptr, &queue_.getEvent("SPH_enqueueReleaseGLObjects"));
    err.clCheckError();
    queue_.getQueue().finish();

    if (DisplayWindow::Get().getKeyState(GLFW_KEY_KP_ADD) == KeyState::kDown)
        focus_++;
    if (DisplayWindow::Get().getKeyState(GLFW_KEY_KP_SUBTRACT) == KeyState::kDown)
        focus_--;

    for (auto &pair: queue_.getEventMap()) {

        std::cout << "F[" << pair.first << "]" << std::endl;

        cl::Event &ev = queue_.getEvent(pair.first);

        cl_ulong start = 0, end = 0;
        cl_double time = 0;
        err.err = ev.getProfilingInfo<cl_ulong>(CL_PROFILING_COMMAND_START, &start);
        err.err |= ev.getProfilingInfo<cl_ulong>(CL_PROFILING_COMMAND_END, &end);
        err.clCheckError();

        time = (cl_double)(end - start) * (cl_double)(1e-06);
        std::cout << "Prof[ F[" << pair.first << "] time[" << (double)time << "] ]" << std::endl;
    }
}

void ModuleSPH::reload() {
    if (debug_)
        printf("%s\n", __FUNCTION_NAME__);
    init();
}

void ModuleSPH::shiftDebugViscosity() {
    flag_ = static_cast<int>(flag_ ^ eSph::kDebugViscosity);
}
void ModuleSPH::shiftDebugVelocity() {
    flag_ = static_cast<int>(flag_ ^ eSph::kDebugVelocity);
}
void ModuleSPH::shiftDebugPressure() {
    flag_ = static_cast<int>(flag_ ^ eSph::kDebugPressure);
}

void ModuleSPH::gui() {
    ImGui::DragFloat("Pressure", &getModuleParam().pressure, 0.1f, 0, 2500.f);
    ImGui::DragFloat("SmoothingRadius: ", &getModuleParam().smoothingRadius, 0.05f, 0, 200.f);
    ImGui::DragFloat("Viscosity: ", &getModuleParam().viscosity, 0.001f, 0, 1.f);
    ImGui::DragFloat("DensityRef: ", &getModuleParam().densityRef, 0.1f, 0, 10.f);

    Gui::beginColor(eColor::kRed, eColor::kMedium);
    if (ImGui::Button("D_Velocity")) {
        shiftDebugVelocity();
    }
    Gui::endColor();
    ImGui::SameLine();
    Gui::beginColor(eColor::kPurple, eColor::kMedium);
    if (ImGui::Button("D_Viscosity")) {
        shiftDebugViscosity();
    }
    Gui::endColor();
    ImGui::SameLine();
    Gui::beginColor(eColor::kGreen, eColor::kMedium);
    if (ImGui::Button("D_Pressure")) {
        shiftDebugPressure();
    }
    Gui::endColor();

    float *arr = new float[emitter_.getQueue().getEventMap().size()];
    unsigned int i = 0;
    float max = 10.0f;
    ClError err;
    for (auto &pair: emitter_.getQueue().getEventMap()) {
        cl::Event &ev = emitter_.getQueue().getEvent(pair.first);

        cl_ulong start = 0, end = 0;
        cl_double time = 0;
        err.err = ev.getProfilingInfo<cl_ulong>(CL_PROFILING_COMMAND_START, &start);
        err.err |= ev.getProfilingInfo<cl_ulong>(CL_PROFILING_COMMAND_END, &end);
        err.clCheckError();

        time = (cl_double)(end - start) * (cl_double)(1e-06);
        if (time > max) {
            max = time;
        }
        arr[i] = (float)time;
        std::cout << arr[i] << " | " << IM_ARRAYSIZE(arr) << std::endl;
        i++;
    }
    ImGui::PlotHistogram("Histogram", arr, emitter_.getQueue().getEventMap().size(),
                         0, NULL, 0.0f, max, ImVec2(0, 80));
}