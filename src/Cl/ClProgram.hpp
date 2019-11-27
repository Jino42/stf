#pragma once

#include <map>
#include <exception>
#include <NTL.hpp>
#include <iostream>
#include <boost/filesystem.hpp>
#include "ClContext.hpp"
#include "ClError.hpp"
#include <memory>


class ClProgram {
public:

	class BuildException : public std::runtime_error {
	public:
		BuildException() noexcept = delete;
		explicit BuildException(std::string const &s) noexcept;
		const char *what() const noexcept override;
		~BuildException() noexcept override = default;
		BuildException(BuildException const &src) noexcept  = delete;
		BuildException &operator=(BuildException const &rhs) noexcept = delete;
	private:
		std::string error_;
	};

	ClProgram();

	void addInclude(boost::filesystem::path const &path);
	void addProgram(boost::filesystem::path const &path);
	void buildProgram();
	cl::Kernel &getKernel(std::string const &name);
	cl::Program const getProgram() const;


	static ClProgram &Get() {
		if (!instance_) {
			instance_ = std::make_unique<ClProgram>();
		}
		return *ClProgram::instance_;
	}

private:
	bool bBuilded_;
	std::vector< boost::filesystem::path > pathBuffer_;
	cl::Program::Sources sources_;
	std::vector< std::string > rawSources_;
	cl::Program program_;
	std::map<std::string, cl::Kernel> kernelBuffer_;

	void buildKernel_(std::string const &name);

	static std::unique_ptr<ClProgram> instance_;

};
