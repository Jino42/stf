#include "RadixCl.hpp"
#include "Cl/ClProgram.hpp"

RadixCl::RadixCl() :
bufferArray_(nullptr),
bufferHisto_(nullptr),
bufferScan_(nullptr),
bufferBlocksum_(nullptr),
bufferOutput_(nullptr),
kernelCount_(ClProgram::Get().getKernel("")),
kernelScan_(ClProgram::Get().getKernel("")),
kernelCoalesce_(ClProgram::Get().getKernel("")),
kernelReorder_(ClProgram::Get().getKernel("")),
kernelBlocksum_(ClProgram::Get().getKernel(""))
{

}
