#pragma once

#include "AShape.hpp"
#include <Engine/DebugGraphic.hpp>

struct cl_AABB : public cl_Shape {
    cl_float3 size;
};

class AABB : public AShape, public ADebugObject {
  public:
    AABB();

    void updateLines() override;

    void setSize(glm::vec3 const &vec);
    void setPosition(glm::vec3 position) override;

    glm::vec3 &getSize() { return size_; }

    cl_AABB *getCl_Shape() const override;


  private:
    glm::vec3 size_;
};