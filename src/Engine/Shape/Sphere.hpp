#pragma once

#include "AShape.hpp"
#include <Engine/DebugGraphic.hpp>

struct cl_Sphere : public cl_Shape {
    cl_float radius;
};


class Sphere : public AShape, public ADebugObject {
  public:
    Sphere();

    void updateLines() override;

    void setRadius(float radius);
    void setPosition(glm::vec3 position) override;

    float &getRadius() { return radius_; };
    int &getSectorCount() { return sectorCount_; };
    int &getStackCount() { return stackCount_; };

    cl_Sphere *getCl_Shape() const override;

  private:
    float radius_;

    int sectorCount_;
    int stackCount_;
};