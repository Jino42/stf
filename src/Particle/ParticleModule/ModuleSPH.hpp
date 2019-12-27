#pragma once

#include "AParticleModule.hpp"
#include "Cl/ClQueue.hpp"
#include "OCGL_Buffer.hpp"
#include "Range.hpp"

class AParticleEmitter;

struct ModuleParamSPH {
    cl_float pressure;        //250.f
    cl_float densityRef;      //1.f
    cl_float smoothingRadius; //1.f
    cl_float viscosity;       //0.018f
};

struct ParticleDataSPH {
    cl_float3 position;
    cl_float density;
    cl_float pressure;
    cl_float mass;
    cl_float3 force1;
    cl_float3 force2;
    cl_float3 velocity;
    cl_int flag;
};

class ModuleSPH : public AParticleModule {
    friend class ParticleEmitterSPH;

  public:
    ModuleSPH(AParticleEmitter &emitter);
    void init() override;
    void update(float deltaTime) override;
    void reload() override;

    ModuleParamSPH &getModuleParam() {
        return cpuBufferModuleParam_;
    }

    cl::Buffer &getGpuBufferModuleParam() {
        return gpuBufferModuleParam_;
    }

    OCGL_Buffer &getOCGL_BufferParticles_SPH_Data() {
        return OCGLBufferParticles_SPH_Data_;
    }

  private:
    ClKernel kernelDensity_;
    ClKernel kernelPressure_;
    ClKernel kernelViscosity_;
    ClKernel kernelUpdateCellIndex_;
    ClKernel kernelUpdateCellOffset_;
    OCGL_Buffer OCGLBufferParticles_SPH_Data_;
    cl::Buffer gpuBufferParticles_CellIndex_;
    cl::Buffer gpuBufferParticles_Index_;
    cl::Buffer gpuBuffer_cellOffset_;

    cl::Buffer gpuBufferModuleParam_;
    ModuleParamSPH cpuBufferModuleParam_;

    std::vector<cl::Memory> ocgl_;
};
