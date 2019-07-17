#include "ClProgram.hpp"

ClProgram::ClProgram() :
	bBuilded_(false) {

};

void ClProgram::addProgram(boost::filesystem::path const &path) {
	bBuilded_ = false;
	std::cout << path << std::endl;
	if (std::find_if_not (pathBuffer_.begin(), pathBuffer_.end(), [path](boost::filesystem::path const &s){ return s != path;} ) == pathBuffer_.cend())
	    pathBuffer_.push_back(path);
	else
		std::cerr << path << " IS NOT FIND" << std::endl;
}

void ClProgram::buildProgram() {
	ClError err;
	std::cout << "--------------------------------" << std::endl;
	sources_.clear();
	kernelBuffer_.clear();
	rawSources_.clear();
	std::string temp;
	bool first = true;
	for (auto &path : pathBuffer_) {
        std::cout << "path :: [" << path << "]" << std::endl;
        //if (path != "D:/Base\\src\\Kernel\\RequiredModule.cl")
        //if (path != "/Users/ntoniolo/NTL/src/Kernel/RequiredModule.cl")
        //  continue;
        std::cout << "path :: [" << path << "]" << std::endl;
        std::fstream f{path.generic_string()};

        std::ifstream ifs(path.generic_string());
        std::string content((std::istreambuf_iterator<char>(ifs)),
                            (std::istreambuf_iterator<char>()));


        content += "\n";
        rawSources_.push_back(content);
        sources_.emplace_back(rawSources_.back().data(), rawSources_.back().size());

        if (first) {
            first = false;
            temp += content;
        }
	}
	std::cout << "KAPPA" << std::endl;
	std::cout << temp << std::endl;

//	for (std::string &s : rawSources_) {
//		std::cout << "-----------------" << std::endl;
//		std::cout << s << std::endl;
//	}

	std::string include("-I ");

	//cl::Program newProgram(ClContext::Get().context, sources_, &err.err);
	cl::Program newProgram(ClContext::Get().context, temp, false, &err.err);
	err.clCheckError();

	include += (boost::filesystem::path(ROOT_PATH) / "src" / "Kernel/").generic_string();

	try {
		if (newProgram.build({ClContext::Get().deviceDefault}, include.c_str()) != CL_SUCCESS) {
			std::string failLog = newProgram.getBuildInfo<CL_PROGRAM_BUILD_LOG>(ClContext::Get().deviceDefault, &err.err);
			err.clCheckError();
			throw BuildException(std::string("OpenCL:: Error during building program : ") + failLog);
		}
	} catch (cl::Error &e) {
		std::string failLog = newProgram.getBuildInfo<CL_PROGRAM_BUILD_LOG>(ClContext::Get().deviceDefault, &err.err);
		err.clCheckError();
		throw BuildException(std::string("OpenCL:: Error during building program : ") + failLog);
	}

	program_ = newProgram;
	bBuilded_ = true;
}

void ClProgram::buildKernel_(std::string const &name) {
	ClError err;

	cl::Kernel kernel(program_, name.c_str(), &err.err);
	err.clCheckError();
	kernelBuffer_.insert(std::make_pair(name, kernel));
}

cl::Kernel &ClProgram::getKernel(std::string const &name) {
	if (!bBuilded_)
		buildProgram();
	if (kernelBuffer_.find(name) != kernelBuffer_.end())
		return kernelBuffer_.at(name);

	buildKernel_(name);
	return kernelBuffer_.at(name);
}

cl::Program const ClProgram::getProgram() const {
	return program_;
}

std::unique_ptr<ClProgram> ClProgram::instance_ = nullptr;


ClProgram::BuildException::BuildException(std::string const &s) noexcept : std::runtime_error(s), error_(s) {}
const char *ClProgram::BuildException::what() const noexcept { return (error_.c_str()); }