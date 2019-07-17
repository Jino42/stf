#pragma once

#include <NTL.hpp>
#include <glad/glad.h>

class DeviceBuffer {
public:
	DeviceBuffer(size_t size);
    DeviceBuffer(DeviceBuffer const &deviceBuffer);
    DeviceBuffer &operator=(DeviceBuffer const &deviceBuffer);

	GLuint		vbo;
	cl::Memory	mem;
};
