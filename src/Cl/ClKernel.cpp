//
// Created by ntoniolo on 27/11/2019.
//

#include "ClKernel.hpp"
#include "ClProgram.hpp"

ClKernel::ClKernel(std::string const &name) :
	kernel_(ClProgram::Get().getKernel(name)),
	currentArgs_(0)
{ }

cl::Kernel &ClKernel::getKernel() {
	return kernel_;
}

cl::Kernel const &ClKernel::getKernel() const {
	return kernel_;
}
