#pragma once

class RenderBuffer {
public:
    explicit RenderBuffer(unsigned int width = 1024, unsigned int height = 720);

    void clear();
    void bind();
    void unbind();
    unsigned int getTextureBufferId() const;

    unsigned int width, height;

private:
    unsigned int renderBuffer_;
    unsigned int frameBuffer_;
    unsigned int textureBuffer_;
};
