#pragma once

#include "Cl/ClQueue.hpp"
#include "Cl/ClKernel.hpp"
#include "NTL.hpp"
#include <Engine/Time.hpp>
#include <memory>
#include <string>
#include <vector>

class DebugClPrint {
  public:
    DebugClPrint();
    ~DebugClPrint() = default;
    static DebugClPrint &Get();

    void printAllStructSize();

    static void printStructSizeCPU();
    void printSubArrayParticle(AParticleEmitter &emitter, cl::CommandQueue &queue);
  private:
    ClQueue queue_;
    cl::Kernel kernelPrintBasic_;
    cl::Kernel kernelPrintShape_;

	static std::unique_ptr<DebugClPrint> instance_;
};