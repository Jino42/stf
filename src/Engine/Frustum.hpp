#pragma once

#include <Engine/DebugGraphic.hpp>

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

	void	init() override;
	void	updateLines() override;

    void    build(glm::mat4 const &projection, glm::mat4 const &view);
    bool    pointIn(float x, float y, float z);
    void    render();

    glm::mat4 projection;
    glm::mat4 view;

private:

    glm::vec4 planes_[6];
    unsigned int VAO;
    unsigned int VBO;
};