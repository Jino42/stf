
#include "Frustum.hpp"
#include <Engine/ShaderManager.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <Engine/Camera.hpp>

#include <Engine/MainGraphic.hpp>

void Frustum::build(glm::mat4 const &projection, glm::mat4 const &view)
{
    this->view = view;
    this->projection = projection;
    glm::mat4 mat = projection * view;

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




    /*
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lines), lines, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)nullptr);
    glEnableVertexAttribArray(0);
    */

	this->init();
}

void    Frustum::render() {
    ShaderManager::Get().getShader("debugWireFrame").activate();
    ShaderManager::Get().getShader("debugWireFrame").setMat4("projection", Camera::focus->getProjectionMatrix());
    ShaderManager::Get().getShader("debugWireFrame").setMat4("view", Camera::focus->getViewMatrix());

    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, 24);

}

bool		Frustum::pointIn(float x, float y, float z) {
    for(auto plane : planes_) {
        if (glm::dot(plane, glm::vec4(x, y, z, 1.0f)) < 0) {
            return false;
        }
    }
    return true;
}

void Frustum::init() {
	float ar = 1024.f / 720.f;
	float fov = 80.f;
	float distNear = 0.1f;
	float distFar = 300.f;
	float halfHeight = tanf((3.14159265358979323846f / 180.f) * (fov / 2.f));
	float halfWidth = halfHeight * ar;

	float xn = halfWidth * distNear;
	float xf = halfWidth * distFar;
	float yn = halfHeight * distNear;
	float yf = halfHeight * distFar;

/*
    glm::mat4 inverse = glm::inverse(view);

    glm::vec4 f[8u] =
            {
                    // near face
                    {xn, yn, -distNear, 1.f},
                    {-xn, yn, -distNear, 1.f},
                    {xn, -yn, -distNear, 1.f},
                    {-xn, -yn, -distNear , 1.f},

                    // far face
                    {xf, yf, -distFar, 1.f},
                    {-xf, yf, -distFar , 1.f},
                    {xf, -yf, -distFar , 1.f},
                    {-xf, -yf, -distFar, 1.f},
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
	glm::mat4 inv = glm::inverse(view) * glm::inverse(projection);
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

	linesObject_.push_back(DebugVertex(v[0], glm::vec4(0, 1, 0, 1)));
	linesObject_.push_back(DebugVertex(v[1], glm::vec4(0, 1, 0, 1)));

	linesObject_.push_back(DebugVertex(v[0], glm::vec4(0, 1, 0, 1)));
	linesObject_.push_back(DebugVertex(v[2], glm::vec4(0, 1, 0, 1)));

	linesObject_.push_back(DebugVertex(v[3], glm::vec4(0, 1, 0, 1)));
	linesObject_.push_back(DebugVertex(v[1], glm::vec4(0, 1, 0, 1)));

	linesObject_.push_back(DebugVertex(v[3], glm::vec4(0, 1, 0, 1)));
	linesObject_.push_back(DebugVertex(v[2], glm::vec4(0, 1, 0, 1)));

	linesObject_.push_back(DebugVertex(v[4], glm::vec4(0, 1, 0, 1)));
	linesObject_.push_back(DebugVertex(v[5], glm::vec4(0, 1, 0, 1)));

	linesObject_.push_back(DebugVertex(v[4], glm::vec4(0, 1, 0, 1)));
	linesObject_.push_back(DebugVertex(v[6], glm::vec4(0, 1, 0, 1)));

	linesObject_.push_back(DebugVertex(v[7], glm::vec4(0, 1, 0, 1)));
	linesObject_.push_back(DebugVertex(v[5], glm::vec4(0, 1, 0, 1)));

	linesObject_.push_back(DebugVertex(v[7], glm::vec4(0, 1, 0, 1)));
	linesObject_.push_back(DebugVertex(v[6], glm::vec4(0, 1, 0, 1)));

	linesObject_.push_back(DebugVertex(v[0], glm::vec4(0, 1, 0, 1)));
	linesObject_.push_back(DebugVertex(v[4], glm::vec4(0, 1, 0, 1)));

	linesObject_.push_back(DebugVertex(v[1], glm::vec4(0, 1, 0, 1)));
	linesObject_.push_back(DebugVertex(v[5], glm::vec4(0, 1, 0, 1)));

	linesObject_.push_back(DebugVertex(v[3], glm::vec4(0, 1, 0, 1)));
	linesObject_.push_back(DebugVertex(v[7], glm::vec4(0, 1, 0, 1)));

	linesObject_.push_back(DebugVertex(v[2], glm::vec4(0, 1, 0, 1)));
	linesObject_.push_back(DebugVertex(v[6], glm::vec4(0, 1, 0, 1)));

	MainGraphic::Get().getDebugGraphic().addDebugObject(*this);
}

void Frustum::updateLines() {
}


