#pragma once
#include <NTL.hpp>

struct cl_Shape {
    cl_int flag;
    cl_float3 position;
};

class AShape {
  public:
    AShape();
    virtual ~AShape() = default;

    glm::vec3 &getPosition();
    virtual void setPosition(glm::vec3 position);

    virtual cl_Shape *getCl_Shape() const = 0;

  protected:
    int flag_;
    glm::vec3 position_;
};