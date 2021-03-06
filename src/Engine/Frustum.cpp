
#include "Frustum.hpp"
#include <Engine/ShaderManager.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <Engine/Camera.hpp>
#include <Engine/MainGraphic.hpp>
#include <NTL.hpp>

Frustum::Frustum(Camera &camera) :
camera_(camera) {
    setDebug();
}

void Frustum::update()
{
    glm::mat4 mat = camera_.getProjectionMatrix() * camera_.getViewMatrix();

    for (int i = 4; i--; )
        planes_[FRUSTUM_PLANE_LEFT][i] = mat[i][3] + mat[i][0];
    for (int i = 4; i--; )
        planes_[FRUSTUM_PLANE_RIGHT][i] = mat[i][3] - mat[i][0];
    for (int i = 4; i--; )
        planes_[FRUSTUM_PLANE_DOWN][i] = mat[i][3] + mat[i][1];
    for (int i = 4; i--; )
        planes_[FRUSTUM_PLANE_UP][i] = mat[i][3] - mat[i][1];
    for (int i = 4; i--; )
        planes_[FRUSTUM_PLANE_NEAR][i] = mat[i][3] + mat[i][2];
    for (int i = 4; i--; )
        planes_[FRUSTUM_PLANE_FAR][i] = mat[i][3] - mat[i][2];

    updateLines();
}

bool		Frustum::pointIn(float x, float y, float z) {
    for(auto plane : planes_) {
        if (glm::dot(plane, glm::vec4(x, y, z, 1.0f)) < 0) {
            return false;
        }
    }
    return true;
}

void Frustum::initDebug() {
}

void Frustum::updateLines() {
	float ar = 1024.f / 720.f;
	float halfHeight = tanf((3.1415926f / 180.f) * (camera_.fov_ / 2.f));
	float halfWidth = halfHeight * ar;

	float xn = halfWidth * camera_.near_;
	float xf = halfWidth * camera_.far_;
	float yn = halfHeight * camera_.near_;
	float yf = halfHeight * camera_.far_;

/*
    glm::mat4 inverse = glm::inverse(view);

    glm::vec4 f[8u] =
            {
                    // near face
                    {xn, yn, -camera_.near_, 1.f},
                    {-xn, yn, -camera_.near_, 1.f},
                    {xn, -yn, -camera_.near_, 1.f},
                    {-xn, -yn, -camera_.near_ , 1.f},

                    // far face
                    {xf, yf, -camera_.far_, 1.f},
                    {-xf, yf, -camera_.far_ , 1.f},
                    {xf, -yf, -camera_.far_ , 1.f},
                    {-xf, -yf, -camera_.far_, 1.f},
            };

    glm::vec3 v[8];
    for (int i = 0; i < 8; i++)
    {
        glm::vec4 ff = inverse * f[i];
        v[i].x = ff.x / ff.w;
        v[i].y = ff.y / ff.w;
        v[i].z = ff.z / ff.w;
    }

    glm::vec3 lines[] = {
            v[0], v[1],
            v[0], v[2],
            v[3], v[1],
            v[3], v[2],
            v[4], v[5],
            v[4], v[6],
            v[7], v[5],
            v[7], v[6],
            v[0], v[4],
            v[1], v[5],
            v[3], v[7],
            v[2], v[6]
    };
*/

	/// Converstion du Clip Space en World Space
	/// ClipSpace   - > View Space  = ClipSpace   *  INV(Projection Matrix)
	/// View Space  - > World Space = View Space  *  INV(View Matrix)
	///
	glm::mat4 inv = glm::inverse(camera_.getViewMatrix()) * glm::inverse(camera_.getProjectionMatrix());
	glm::vec4 ndc[8] =
			{
					// near face
					{ 1, 1, -1, 1.f },
					{ -1, 1, -1, 1.f },
					{ 1, -1, -1, 1.f },
					{ -1, -1, -1, 1.f },

					// far face
					{ 1, 1, 1, 1.f},
					{ -1, 1, 1, 1.f},
					{ 1, -1, 1, 1.f},
					{ -1, -1, 1, 1.f},
			};

	glm::vec3 v[8];
	for (int i = 0; i < 8; i++)
	{
		glm::vec4 wc = inv * ndc[i];

		///Homogenous to cartesian conversion
		v[i].x = wc.x / wc.w;
		v[i].y = wc.y / wc.w;
		v[i].z = wc.z / wc.w;
	}

    linesObject_.clear();
    linesObject_.reserve(24);
	linesObject_.emplace_back(v[0], color);
	linesObject_.emplace_back(v[1], color);
	linesObject_.emplace_back(v[0], color);
	linesObject_.emplace_back(v[2], color);
	linesObject_.emplace_back(v[3], color);
	linesObject_.emplace_back(v[1], color);
	linesObject_.emplace_back(v[3], color);
	linesObject_.emplace_back(v[2], color);
	linesObject_.emplace_back(v[4], color);
	linesObject_.emplace_back(v[5], color);
	linesObject_.emplace_back(v[4], color);
	linesObject_.emplace_back(v[6], color);
	linesObject_.emplace_back(v[7], color);
	linesObject_.emplace_back(v[5], color);
	linesObject_.emplace_back(v[7], color);
	linesObject_.emplace_back(v[6], color);
	linesObject_.emplace_back(v[0], color);
	linesObject_.emplace_back(v[4], color);
	linesObject_.emplace_back(v[1], color);
	linesObject_.emplace_back(v[5], color);
	linesObject_.emplace_back(v[3], color);
	linesObject_.emplace_back(v[7], color);
	linesObject_.emplace_back(v[2], color);
	linesObject_.emplace_back(v[6], color);


    updateDebug();
}


