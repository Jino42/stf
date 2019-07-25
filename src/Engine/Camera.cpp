#include <iostream>
#include "Camera.hpp"
#include <Engine/MainGraphic.hpp>

Camera::Camera() :
		needUpdateViewMatrix_ (false),
	position_(glm::vec3(0.0f, 0.0f, 0.0f)),
	front_(glm::vec3(0.0f, 0.0f, -1.0f)),
	up_(glm::vec3(0.0f, 1.0f, 0.0f)),
	worldUp_(up_),
	yaw_(-90.0f),
	pitch_(0.0f),
	speed_(2.5f),
	sensitivity_(0.1f),
	zoom_(45.0f),
	projectionMatrix_(1.0f),
	viewMatrix_(1.0f) {
	
	// Temp fix to have voxels filled at startup
	// new edit: do not work anymore :(
	processMouseMovement(0.01, 0.01);

	projectionMatrix_ = glm::perspective(glm::radians(80.0f),
								   static_cast<float>(MainGraphic::Get().getRenderBuffer().width) / static_cast<float>(MainGraphic::Get().getRenderBuffer().height),
								   NEAR_PLANE, MAX_PLANE);

	updateCameraVectors_();
}

void 		Camera::update() {
}

void		Camera::processPosition(Camera::Movement direction, float deltaTime) {
	float velocity = speed_ * deltaTime;

	if (direction == Camera::Movement::FORWARD)
		position_ += front_ * velocity;
	else if (direction == Camera::Movement::BACKWARD)
		position_ -= front_ * velocity;
	else if (direction == Camera::Movement::LEFT)
		position_ -= right_ * velocity;
	else if (direction == Camera::Movement::RIGHT)
		position_ += right_ * velocity;
	else if (direction == Camera::Movement::DOWN)
		position_ -= up_ * velocity;
	else if (direction == Camera::Movement::UP)
		position_ += up_ * velocity;
	needUpdateViewMatrix_ = true;
}

void		Camera::setPosition(glm::vec3 const &pos) {
	position_ = pos;
}

void		Camera::processPosition(glm::vec3 const &move, float deltaTime) {
	float velocity = speed_ * deltaTime;

	position_ += front_ * (move * velocity);
}

void		Camera::processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch) {
	xoffset *= sensitivity_;
	yoffset *= sensitivity_;

	yaw_   += xoffset;
	pitch_ += yoffset;

	if (constrainPitch) {
		if (pitch_ > 89.0f)
			pitch_ = 89.0f;
		if (pitch_ < -89.0f)
			pitch_ = -89.0f;
	}
	updateCameraVectors_();
}

void Camera::setFront(glm::vec3 const &front) {
	front_ = front;
	front_ = glm::normalize(front_);
	yaw_ = atan2(front.z, front.x);
	pitch_ = asin(front.y);
}
void Camera::setUp(glm::vec3 const &up) {
	up_ = up;
}

glm::mat4	Camera::getViewMatrix() {
	if (needUpdateViewMatrix_) {
		viewMatrix_ = glm::lookAt(position_, position_ + front_, up_);
		needUpdateViewMatrix_ = false;
	}
	return viewMatrix_;
}

glm::mat4 Camera::getProjectionMatrix() const {
	return projectionMatrix_;
}

glm::vec3	Camera::getPosition() const {
	return position_;
}
glm::vec3	Camera::getFront() const {
	return front_;
}
void		Camera::updateCameraVectors_() {
	glm::vec3 front;
	front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
	front.y = sin(glm::radians(pitch_));
	front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));

	front_ = glm::normalize(front);
	right_ = glm::normalize(glm::cross(front_, worldUp_));
	up_    = glm::normalize(glm::cross(right_, front_));
	needUpdateViewMatrix_ = true;
}

Camera *Camera::focus = nullptr;