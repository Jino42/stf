
#include "ClContext.hpp"
#ifdef WIN32
#include <Windows.h>
#endif
#include <exception>
#include <Engine/Display/DisplayWindow.hpp>
#include <NTL.hpp>
#include <sstream>

static const std::string NVIDIA_PLATFORM = "NVIDIA";
static const std::string AMD_PLATFORM = "AMD";
static const std::string MESA_PLATFORM = "Clover";
static const std::string INTEL_PLATFORM = "Intel";
static const std::string APPLE_PLATFORM = "Apple";

#if defined (__APPLE__) || defined(MACOSX)
static const std::string CL_GL_SHARING_EXT = "cl_APPLE_gl_sharing";
#else
static const std::string CL_GL_SHARING_EXT = "cl_khr_gl_sharing";
#endif

typedef std::vector<cl::Platform>::iterator PlatformIter;

bool checkExtnAvailability(cl::Device pDevice, std::string pName)
{
	bool ret_val = true;
	try {
		// find extensions required
		std::string exts = pDevice.getInfo<CL_DEVICE_EXTENSIONS>();
		std::stringstream ss(exts);
		std::string item;
		int found = -1;
		while (std::getline(ss,item,' ')) {
			if (item==pName) {
				found=1;
				break;
			}
		}
		if (found==1) {
			std::cout<<"Found CL_GL_SHARING extension: "<<item<<std::endl;
			ret_val = true;
		} else {
			std::cout<<"CL_GL_SHARING extension not found\n";
			ret_val = false;
		}

	} catch (cl::Error err) {
		std::cout << err.what() << "(" << err.err() << ")" << std::endl;
	}
	return ret_val;
}

cl::Platform getPlatform(std::string pName, cl_int &error)
{
	cl::Platform ret_val;
	error = 0;
	try {
		// Get available platforms
		std::vector<cl::Platform> platforms;
		cl::Platform::get(&platforms);
		int found = -1;
		for(PlatformIter it=platforms.begin(); it<platforms.end(); ++it) {
			std::string temp = it->getInfo<CL_PLATFORM_NAME>();
			if (temp.find(pName)!=std::string::npos) {
				found = it - platforms.begin();
				std::cout<<"Found platform: "<<temp<<std::endl;
				break;
			}
		}
		if (found==-1) {
			// Going towards + numbers to avoid conflict with OpenCl error codes
			error = +1; // requested platform not found
		} else {
			ret_val = platforms[found];
		}
	} catch(cl::Error err) {
		std::cout << err.what() << "(" << err.err() << ")" << std::endl;
		error = err.err();
	}
	return ret_val;
}

#define FIND_PLATFORM(name)             \
    plat = getPlatform(name, errCode);  \
    if (errCode == CL_SUCCESS)          \
        return plat;



cl::Platform getPlatform()
{
	cl_int errCode;
	cl::Platform plat;

	FIND_PLATFORM(NVIDIA_PLATFORM)
	FIND_PLATFORM(AMD_PLATFORM)
	FIND_PLATFORM(INTEL_PLATFORM)
	FIND_PLATFORM(APPLE_PLATFORM)
	FIND_PLATFORM(MESA_PLATFORM)

	// If no platforms are found
	exit(252);
}

ClContext::ClContext() {
	ClError err;

	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);
	if(platforms.empty())
		throw std::runtime_error("OpenCL:: No platforms found.");

	for (cl::Platform &platform : platforms) {
		platform.getInfo<CL_PLATFORM_NAME>();
	}

	cl::Platform defaultPlatform=platforms[0];
	defaultPlatform = getPlatform();
	std::cout << "Using platform: "<< defaultPlatform.getInfo<CL_PLATFORM_NAME>() << std::endl;

	defaultPlatform.getDevices(CL_DEVICE_TYPE_GPU, &devicesGpu_);
	if(devicesGpu_.empty())
		throw std::runtime_error("OpenCL:: No GPU device found.");
	//defaultPlatform.getDevices(CL_DEVICE_TYPE_CPU, &devicesCpu_);
	//if(devicesCpu_.empty())
	//	throw std::runtime_error("OpenCL:: No CPU device found.");

	deviceDefault = devicesGpu_[0];
	if (debug_)
		std::cout << "Using device: "<< deviceDefault.getInfo<CL_DEVICE_NAME>() << std::endl;

	auto    oCL_error_callback = [](const char *errinfo, const void *private_info,
									size_t cb, void *user_data)
	{
		static_cast<void>(private_info);
		static_cast<void>(cb);
		static_cast<void>(user_data);
		std::cerr << "OpenCL:: Context error :: " << errinfo << std::endl;
	};

#ifdef WIN32
	cl_context_properties properties[] = {
			CL_GL_CONTEXT_KHR, (cl_context_properties)glfwGetWGLContext(DisplayWindow::Get().getWindow()),
            CL_WGL_HDC_KHR, (cl_context_properties)GetDC(glfwGetWin32Window(DisplayWindow::Get().getWindow())),
            CL_CONTEXT_PLATFORM, (cl_context_properties)defaultPlatform(),
            0
        };
#elif __APPLE__
	CGLContextObj    gl_ctx        = CGLGetCurrentContext();
	CGLShareGroupObj gl_sharegroup = CGLGetShareGroup(gl_ctx);
	cl_context_properties properties[] = {
			CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE, (cl_context_properties) gl_sharegroup,
			0
	};
#endif

	std::vector<cl::Device> devices;
	defaultPlatform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
	// Get a list of devices on this platform
	for (unsigned d=0; d<devices.size(); ++d) {
		if (checkExtnAvailability(devices[d], CL_GL_SHARING_EXT)) {
			deviceDefault = devices[d];
			break;
		}
	}

	//context = cl::Context(deviceDefault, properties);//, oCL_error_callback, nullptr, &err.err);
	context = cl::Context(deviceDefault, properties);//, oCL_error_callback, nullptr, &err.err);
	err.clCheckError();

	size_t sizeBufferPrintf;
	deviceDefault.getInfo(CL_DEVICE_PRINTF_BUFFER_SIZE, &sizeBufferPrintf);
	std::cout << "sizeBufferPrintf : " << sizeBufferPrintf << std::endl;
}

std::unique_ptr<ClContext> ClContext::instance_ = nullptr;
bool ClContext::debug_ = true;

/*
#include "ClContext.hpp"

ClContext::ClContext() {
    ClError err;

    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    if(platforms.empty())
        throw std::runtime_error("OpenCL:: No platforms found.");

    cl::Platform defaultPlatform=platforms[0];
    std::cout << "Using platform: "<< defaultPlatform.getInfo<CL_PLATFORM_NAME>() << std::endl;

    defaultPlatform.getDevices(CL_DEVICE_TYPE_GPU, &devicesGpu_);
    if(devicesGpu_.empty())
        throw std::runtime_error("OpenCL:: No GPU device found.");
    defaultPlatform.getDevices(CL_DEVICE_TYPE_CPU, &devicesCpu_);
    if(devicesCpu_.empty())
        throw std::runtime_error("OpenCL:: No CPU device found.");

    deviceDefault = devicesGpu_[0];
    if (debug_)
        std::cout << "Using device: "<< deviceDefault.getInfo<CL_DEVICE_NAME>() << std::endl;

    auto    oCL_error_callback = [](const char *errinfo, const void *private_info,
                                    size_t cb, void *user_data)
    {
        static_cast<void>(private_info);
        static_cast<void>(cb);
        static_cast<void>(user_data);
        std::cerr << "OpenCL:: Context error :: " << errinfo << std::endl;
    };
    CGLContextObj    gl_ctx        = CGLGetCurrentContext();
    CGLShareGroupObj gl_sharegroup = CGLGetShareGroup(gl_ctx);

#ifdef __APPLE__
    cl_context_properties properties[] = {
            CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE, (cl_context_properties) gl_sharegroup,
            0
    };
#endif

    context = cl::Context(deviceDefault, properties, oCL_error_callback, nullptr, &err.err);
    err.clCheckError();
}

std::unique_ptr<ClContext> ClContext::instance_ = nullptr;
bool ClContext::debug_ = true;
*/