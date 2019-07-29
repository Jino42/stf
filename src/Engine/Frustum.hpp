#pragma once

#include <Engine/DebugGraphic.hpp>

class Camera;

enum {
    FRUSTUM_PLANE_NEAR = 0,
    FRUSTUM_PLANE_FAR,
    FRUSTUM_PLANE_LEFT,
    FRUSTUM_PLANE_RIGHT,
    FRUSTUM_PLANE_UP,
    FRUSTUM_PLANE_DOWN
};

class Frustum : public ADebugObject {
public:
    Frustum() = delete;
	Frustum(Camera &camera);
	void	updateLines() override;
    void    initDebug() override;
    void    update();
    bool    pointIn(float x, float y, float z);

private:
    Camera &camera_;
    glm::vec4 planes_[6];
};