#ifndef PERSO_HPP
#define PERSO_HPP

#include <NTL.hpp>
#include <Cl/ClError.hpp>
#include <Cl/ClContext.hpp>
#include "glad/glad.h"

namespace OpenCGL {
	void RunKernelWithMem(cl::CommandQueue const &queue, cl::Kernel const &kernel,
			std::vector<cl::Memory> const &cl_vbos,
			cl::NDRange at = cl::NullRange, cl::NDRange global = cl::NullRange, cl::NDRange local = cl::NullRange);
	void RunKernelWithMem(cl::CommandQueue const &queue, cl::Kernel const &kernel,
			cl::Memory const &mem,
			cl::NDRange at = cl::NullRange, cl::NDRange global = cl::NullRange, cl::NDRange local = cl::NullRange);
}

#endif

