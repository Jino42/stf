#pragma once

#include "NTL.hpp"
#include "Enum/eParticleBuffer.hpp"

class AParticleEmitter;

class ClKernel {
public:
	ClKernel() = delete;
	explicit ClKernel(std::string const &name);
	ClKernel(ClKernel const &clKernel) = default;
	~ClKernel() = default;

	cl::Kernel const &getKernel() const;
	cl::Kernel &getKernel();

	template <class ...Args>
	cl_int setArgs(const Args & ... args)
	{
		return do_for_([&](const auto &arg)->cl_int {
			cl_int r = kernel_.setArg(this->currentArgs_, arg);
			this->currentArgs_++;
			return r;
		}, args...);
	}

	cl_int setArgsGPUBuffers(AParticleEmitter &emitter, eParticleBuffer buffers);

private:
	cl::Kernel	&kernel_;
	cl_uint currentArgs_;

	template <class F, class... Args>
	int do_for_(F f, const Args &... args) {
		cl_int x[] = {f(args)...};
		for (const int p : x) {
			if (p != 0)
				return p;
		}
		return 0;
	}

};