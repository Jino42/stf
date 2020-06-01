#pragma once
#include <NTL.hpp>
#include <iostream>
#include <Cl/ClContext.hpp>
#include <Cl/ClError.hpp>

namespace ClTools {
	template <typename T>
	void printIntArrayGpu(cl::Buffer &buffer, size_t length,
			std::string const &name, void (*print)(T*)) {
		ClError err;
		cl::CommandQueue queue(ClContext::Get().context, ClContext::Get().deviceDefault);
		T *arrayCpu = new T[length];
		err.err = queue.enqueueReadBuffer(buffer, CL_TRUE, 0, sizeof(T) * length, arrayCpu);
		err.clCheckError();
		queue.finish();

		std::cout << "start [" << name << "]" << std::endl;
		for (int i = 0; i < length; i++)
			print(&arrayCpu[i]);
		std::cout << "end [" << name << "]" << std::endl;
	}
};