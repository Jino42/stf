#include "DeviceBuffer.hpp"
#include "Cl/ClContext.hpp"

DeviceBuffer::DeviceBuffer(size_t size) {
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_STATIC_DRAW);
	mem = cl::BufferGL(ClContext::Get().context, CL_MEM_READ_WRITE, vbo);
}
DeviceBuffer::DeviceBuffer(DeviceBuffer const &deviceBuffer) {
    *this = deviceBuffer;
}

DeviceBuffer &DeviceBuffer::operator=(DeviceBuffer const &deviceBuffer) {
    if (this != &deviceBuffer) {
        vbo = deviceBuffer.vbo;
        mem = deviceBuffer.mem;
    }
    return *this;
}
