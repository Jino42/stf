#pragma once

#include "NTL.hpp"
#include "Cl/ClError.hpp"

class ClKernel;

#define BITS 32
//Number of buckets necessary
#define BUCK (1 << RADIX)
//Number of bits in the radix
#define RADIX 4

class RadixCl {
public:
    RadixCl();
    RadixCl(RadixCl const &) = delete;
    ~RadixCl() = default;

    void radix(cl::Buffer &arrayGpu, size_t sizeArrayGpu);
    void printIntArrayGpu(cl::Buffer &buffer, size_t size, std::string const &name);
	template < typename T>
	void printArrayCpu(T *array, size_t size, std::string const &name) const {
		std::cout << "-->Start --> Print ArrayCPU [" << name << "]" << std::endl;
		for (size_t i = 0; i < size; i++)
			std::cout << i << " : " << array[i] << std::endl;
		std::cout << "<--End <-- Print ArrayCPU [" << name << "]" << std::endl;
	}
private:
	cl::CommandQueue queue_;
	cl::Kernel &kernelCount_;
	cl::Kernel &kernelScan_;
	cl::Kernel &kernelBlocksum_;
	cl::Kernel &kernelCoalesce_;
	cl::Kernel &kernelReorder_;
    ClError		err_;

	static bool debug_;
};
