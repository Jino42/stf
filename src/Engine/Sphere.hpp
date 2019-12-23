#pragma once

#include <Engine/DebugGraphic.hpp>

class Sphere : public ADebugObject {
  public:
    Sphere();

    void updateLines() override;

    void setRadius(float radius);
    void setCenter(glm::vec3 const &center);

  private:
    float radius_;
    glm::vec3 center_;

    int sectorCount_;
    int stackCount_;
};