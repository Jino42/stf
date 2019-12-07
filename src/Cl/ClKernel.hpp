#pragma once

#include "NTL.hpp"
#include "Enum/eParticleBuffer.hpp"
#include "Cl/ClProgram.hpp"

class AParticleEmitter;

class ClKernel {
public:
	explicit ClKernel();
	ClKernel(ClKernel const &clKernel) = default;
	~ClKernel() = default;

	cl::Kernel const &getKernel() const;
	cl::Kernel	&getKernel();
	void		setKernel(AParticleEmitter &emitter, std::string const &name);

	cl_int beginAndSetUpdatedArgs();

	template <class ...Args>
	cl_int beginAndSetUpdatedArgs(const Args & ... args)
	{
		kernel_ = &ClProgram::Get().getKernel(name_);
		currentArgs_ = 0;
		cl_int ret;

		if((ret = updateArgsGPUBuffers_()))
			return ret;
		return do_for_([&](const auto &arg)->cl_int {
			cl_int r = kernel_->setArg(this->currentArgs_, arg);
			this->currentArgs_++;
			return r;
		}, args...);
	}

	void setArgsGPUBuffers(eParticleBuffer buffers);


private:
	cl::Kernel	*kernel_;
	AParticleEmitter *emitter_;
	std::string name_;
	cl_uint currentArgs_;
	eParticleBuffer enum_;

	cl_int updateArgsGPUBuffers_();

	template <class ...Args>
	cl_int setArgs_(const Args & ... args)
	{
		return do_for_([&](const auto &arg)->cl_int {
			cl_int r = kernel_->setArg(this->currentArgs_, arg);
			this->currentArgs_++;
			return r;
		}, args...);
	}

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