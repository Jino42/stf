#pragma once

#include "AParticleModule.hpp"
#include "Cl/ClQueue.hpp"
#include "OCGL_Buffer.hpp"
#include "Range.hpp"

class AParticleEmitter;

struct ModuleParamSPH {
    cl_float pressure;   //250.f
    cl_float densityRef; //1.f
    cl_float smoothingRadius; //1.f
    cl_float viscosity;  //0.018f
};

struct ParticleDataSPH {
    cl_float3 position;
    cl_float density;
    cl_float pressure;
    cl_float mass;
    cl_float3 force1;
    cl_float3 force2;
    cl_float3 velocity;
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

  private:
    ClKernel kernelDensity_;
    ClKernel kernelPressure_;
    ClKernel kernelViscosity_;
    OCGL_Buffer OCGLBufferParticles_SPH_Data_;

    cl::Buffer gpuBufferModuleParam_;
    ModuleParamSPH cpuBufferModuleParam_;

	std::vector<cl::Memory> ocgl_;
};
