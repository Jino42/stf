#include "glad/glad.h"
#include "RenderBuffer.hpp"
#include <stdexcept>

RenderBuffer::RenderBuffer(unsigned int width, unsigned int height) :
    width(width),
    height(height) {
    glGenFramebuffers(1, &frameBuffer_);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer_);
    
    // Create a color attachment texture
    glGenTextures(1, &textureBuffer_);
    glBindTexture(GL_TEXTURE_2D, textureBuffer_);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //Attack textureBuffer_ to frameBuffer_
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureBuffer_, 0);

    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    glGenRenderbuffers(1, &renderBuffer_);
    glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer_);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer_); // now actually attach it
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void RenderBuffer::bind() {
    glViewport(0, 0, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer_);
}

void RenderBuffer::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderBuffer::clear() {
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer_);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);// | GL_STENCIL_BUFFER_BIT);
}

unsigned int RenderBuffer::getTextureBufferId() const {
    return textureBuffer_;
}