#pragma once

#include "NTL.hpp"

enum {
    FRUSTUM_PLANE_NEAR = 0,
    FRUSTUM_PLANE_FAR,
    FRUSTUM_PLANE_LEFT,
    FRUSTUM_PLANE_RIGHT,
    FRUSTUM_PLANE_UP,
    FRUSTUM_PLANE_DOWN
};

class Frustum {
public:

    void    build(glm::mat4 const &projection, glm::mat4 const &view);
    bool    pointIn(float x, float y, float z);

private:
    glm::mat4 projection_;
    glm::mat4 view_;
    glm::vec4 planes_[6];
};