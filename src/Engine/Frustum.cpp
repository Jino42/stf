
#include "Frustum.hpp"

void Frustum::build(glm::mat4 const &projection, glm::mat4 const &view)
{
    glm::mat4 mat = projection * view;

    for (int i = 4; i--; i)
        planes_[FRUSTUM_PLANE_LEFT][i] = mat[i][3] + mat[i][0];
    for (int i = 4; i--; i)
        planes_[FRUSTUM_PLANE_RIGHT][i] = mat[i][3] - mat[i][0];
    for (int i = 4; i--; i)
        planes_[FRUSTUM_PLANE_DOWN][i] = mat[i][3] + mat[i][1];
    for (int i = 4; i--; i)
        planes_[FRUSTUM_PLANE_UP][i] = mat[i][3] - mat[i][1];
    for (int i = 4; i--; i)
        planes_[FRUSTUM_PLANE_NEAR][i] = mat[i][3] + mat[i][2];
    for (int i = 4; i--; i)
        planes_[FRUSTUM_PLANE_FAR][i] = mat[i][3] - mat[i][2];
}

bool		Frustum::pointIn(float x, float y, float z) {
    for(auto plane : planes_) {
        if (glm::dot(plane, glm::vec4(x, y, z, 1.0f)) < 0) {
            return false;
        }
    }
    return true;
}