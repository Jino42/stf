#pragma once

#include <vector>
#include <exception>
#include <iostream>
#include <NTL.hpp>
#include "ClError.hpp"

# ifdef __APPLE__
# include <OpenCL/cl_gl_ext.h>
# include <OpenGL/CGLDevice.h>
# include <OpenGL/CGLCurrent.h>
#endif


class ClContext {
public:
	ClContext();
	~ClContext() = default;



	static ClContext &Get() {
		if (!instance_ ) {
			instance_ = std::make_unique<ClContext>();
		}
		return *ClContext::instance_;
	}

	cl::Context context;
	cl::Device deviceDefault;
private:
	std::vector<cl::Device> devicesGpu_;
	std::vector<cl::Device> devicesCpu_;
	static std::unique_ptr<ClContext> instance_;

	static bool debug_;
};
