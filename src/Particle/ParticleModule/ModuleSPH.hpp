#pragma once

#include "AParticleModule.hpp"
#include "Cl/ClQueue.hpp"
#include "Engine/AABB.hpp"
#include "OCGL_Buffer.hpp"
#include "Range.hpp"

enum class eSph { // Relation with shader SPHDebug
    kDebug = 1 << 0,
    kDebugNeighbor = 1 << 1,
    kDebugVelocity = 1 << 2,
    kDebugViscosity = 1 << 3,
    kDebugPressure = 1 << 4,
};

inline eSph operator|(eSph const lhs, eSph const rhs) {
    return static_cast<eSph>(static_cast<int>(lhs) | static_cast<int>(rhs));
}
inline eSph const &operator|=(eSph &lhs, eSph const &rhs) {
    lhs = static_cast<eSph>(static_cast<int>(lhs) | static_cast<int>(rhs));
    return (lhs);
}
inline eSph operator&(eSph const lhs, eSph const rhs) {
    return static_cast<eSph>(static_cast<int>(lhs) & static_cast<int>(rhs));
}
inline eSph operator&(int const lhs, eSph const rhs) {
    return static_cast<eSph>(lhs & static_cast<int>(rhs));
}
inline eSph operator&(eSph const lhs, int const rhs) {
    return static_cast<eSph>(static_cast<int>(lhs) & rhs);
}
inline eSph operator^(int const lhs, eSph const rhs) {
    return static_cast<eSph>(lhs ^ static_cast<int>(rhs));
}
inline eSph operator^(eSph const lhs, int const rhs) {
    return static_cast<eSph>(static_cast<int>(lhs) ^ rhs);
}
inline eSph operator<<(eSph const lhs, eSph const rhs) {
    return static_cast<eSph>(static_cast<int>(lhs) << static_cast<int>(rhs));
}
inline eSph operator>>(eSph const lhs, eSph const rhs) {
    return static_cast<eSph>(static_cast<int>(lhs) >> static_cast<int>(rhs));
}

class AParticleEmitter;

struct ModuleParamSPH {
    cl_float pressure;        //250.f
    cl_float densityRef;      //1.f
    cl_float smoothingRadius; //1.f
    cl_float viscosity;       //0.018f

    cl_float h2;
    cl_float h3;
    cl_float h6;
    cl_float h9;
    cl_float Poly6;
    cl_float Spiky;
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

    void shiftDebugViscosity();
    void shiftDebugVelocity();
    void shiftDebugPressure();

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
    int flag_;
    ClKernel kernelDensity_;
    ClKernel kernelPressure_;
    ClKernel kernelViscosity_;
    ClKernel kernelUpdateCellIndex_;
    ClKernel kernelUpdateCellOffset_;
    ClKernel kernelPrint_;

    OCGL_Buffer OCGLBufferParticles_SPH_Data_;
    cl::Buffer gpuBufferParticles_CellIndex_;
    cl::Buffer gpuBufferParticles_Index_;
    cl::Buffer gpuBuffer_cellOffset_;

    cl::Buffer gpuBufferModuleParam_;
    ModuleParamSPH cpuBufferModuleParam_;

    std::vector<cl::Memory> ocgl_;

    float lastSmoothingRadius_;

    int focus_;

    void updateConstant_();

};
