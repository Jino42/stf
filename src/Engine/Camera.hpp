#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <memory>
#include <Engine/Frustum.hpp>
#include <IGuiEntity.hpp>

#define NEAR_PLANE 0.1f
#define MAX_PLANE 300.f

class Camera : public IGuiEntity {
    friend class EditorCamera;
	friend class Frustum;
public:
	enum Movement {
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT,
		UP,
		DOWN
	};

	Camera() = delete;
    Camera(std::string const &name);
	~Camera() = default;

	Camera(Camera const &camera) = default;
	Camera &operator=(Camera const &camera) = default;

	void update();
	void setPosition(glm::vec3 const &pos);
	void setFront(glm::vec3 const &front);
	void setUp(glm::vec3 const &up);

	void processPosition(Camera::Movement direction, float deltaTime = 1.f);
	void processPosition(glm::vec3 const &, float deltaTime = 1.f);
	void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);

	glm::mat4 getViewMatrix();
	glm::mat4 getProjectionMatrix();

	glm::vec3 getPosition() const;
	glm::vec3 getFront() const;

	void	setDebugFrustum(bool b);
	bool	getDebugFrustum() const;

	static Camera *focus;

    void setFov(float fov);
    void setNear(float anear);
    void setFar(float afar);
    float getFov() const;
    float getNear() const;
    float getFar() const;

    void updateProjection();
	void updateView();

private:
    bool needUpdateViewMatrix_;
    bool needUpdateProjectionMatrix_;
	glm::vec3 position_;
	glm::vec3 front_;
	glm::vec3 up_;
	glm::vec3 right_;
	glm::vec3 worldUp_;
	float yaw_;
	float pitch_;
	float speed_;
	float sensitivity_;
	float zoom_;

	float fov_;
	float near_;
	float far_;

	glm::mat4	viewMatrix_;
	glm::mat4	projectionMatrix_;

	Frustum		frustum_;
	bool		needUpdateDebugFrustum_;

	void updateCameraVectors_();

};
