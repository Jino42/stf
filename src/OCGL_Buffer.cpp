#include "OCGL_Buffer.hpp"
#include "Cl/ClContext.hpp"

OCGL_Buffer::OCGL_Buffer(size_t size) {
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_STATIC_DRAW);
	mem = cl::BufferGL(ClContext::Get().context, CL_MEM_READ_WRITE, vbo);
}
OCGL_Buffer::OCGL_Buffer(OCGL_Buffer const &deviceBuffer) {
    *this = deviceBuffer;
}

OCGL_Buffer &OCGL_Buffer::operator=(OCGL_Buffer const &deviceBuffer) {
    if (this != &deviceBuffer) {
        vbo = deviceBuffer.vbo;
        mem = deviceBuffer.mem;
    }
    return *this;
}
