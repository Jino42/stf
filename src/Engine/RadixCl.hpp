#pragma once

#include "NTL.hpp"

class ClKernel;

class RadixCl {
public:
    RadixCl();
    RadixCl(RadixCl const &) = delete;
    ~RadixCl() = default;

private:
    cl::Buffer *bufferArray_;
    cl::Buffer *bufferHisto_;
    cl::Buffer *bufferScan_;
    cl::Buffer *bufferBlocksum_;
    cl::Buffer *bufferOutput_;

    cl::LocalSpaceArg localHisto_;
    cl::LocalSpaceArg local_scan_;
    cl::LocalSpaceArg local_bloclsum_;
    cl::Kernel &kernelCount_;
    cl::Kernel &kernelScan_;
    cl::Kernel &kernelCoalesce_;
    cl::Kernel &kernelReorder_;
    cl::Kernel &kernelBlocksum_;
};
