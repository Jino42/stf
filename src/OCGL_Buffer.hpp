#pragma once

#include <NTL.hpp>
#include <glad/glad.h>

class OCGL_Buffer {
public:
	OCGL_Buffer(size_t size);
    OCGL_Buffer(OCGL_Buffer const &deviceBuffer);
    OCGL_Buffer &operator=(OCGL_Buffer const &deviceBuffer);

	GLuint		vbo;
	cl::Memory	mem;
};
