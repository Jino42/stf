#include "OpenCGL_Tools.hpp"
#include "Cl/ClKernel.hpp"
#include "Cl/ClError.hpp"


void OpenCGL::RunKernelWithMem(cl::CommandQueue const &queue, ClKernel const &kernel, std::vector<cl::Memory> const &cl_vbos, cl::NDRange at, cl::NDRange global, cl::NDRange local) {
	ClError err;

	cl::Event ev;
	glFinish();
	err.err = queue.enqueueAcquireGLObjects(&cl_vbos, nullptr, &ev);
	ev.wait();
	err.clCheckError();
	queue.finish();
	err.err = queue.enqueueNDRangeKernel(kernel.getKernel(), at, global, local);
	err.clCheckError();
	queue.finish();
	err.err = queue.enqueueReleaseGLObjects(&cl_vbos, nullptr, nullptr);
	err.clCheckError();
	queue.finish();
}

void OpenCGL::RunKernelWithMem(cl::CommandQueue const &queue, ClKernel const &kernel, cl::Memory const &mem, cl::NDRange at, cl::NDRange global, cl::NDRange local) {
	ClError err;
	std::vector<cl::Memory> cl_vbos;

	cl_vbos.push_back(mem);
	err.clCheckError();

	cl::Event ev;
	glFinish();
	err.err = queue.enqueueAcquireGLObjects(&cl_vbos, nullptr, &ev);
	ev.wait();
	err.clCheckError();
	queue.finish();
	err.err = queue.enqueueNDRangeKernel(kernel.getKernel(), at, global, local);
	err.clCheckError();
	queue.finish();
	err.err = queue.enqueueReleaseGLObjects(&cl_vbos, nullptr, nullptr);
	err.clCheckError();
	queue.finish();
}