#pragma once

#include "RenderBuffer.hpp"
#include <Engine/DebugGraphic.hpp>
#include <glm/glm.hpp>
#include <memory>
#include "cl_type.hpp"

class MainGraphic {
  public:
    MainGraphic();
    MainGraphic &operator=(MainGraphic const &rhs) = delete;
    MainGraphic(MainGraphic const &src) = delete;

    void init();
    void render();
    void update();
    void logic(float t);

    static MainGraphic &Get();

    RenderBuffer &getRenderBuffer();
    RenderBuffer &getRenderBufferRayMarch();
    DebugGraphic &getDebugGraphic();

  private:
    float deltaTime_;
    RenderBuffer renderBuffer_;
    RenderBuffer renderBufferRayMarch_;
    DebugGraphic debug_;
    bool doParticle_;

    unsigned int rayMarchTexture_;
    cl::ImageGL rayMarchImage_;
    unsigned int VBO, VAO, EBO;
    float vertices_[20];
    float indices_[6];

    static std::unique_ptr<MainGraphic> instance_;
};