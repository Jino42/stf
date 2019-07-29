#include <iostream>
#include "Camera.hpp"
#include <Engine/MainGraphic.hpp>

Camera::Camera(std::string const &name) :
    IGuiEntity(name),
    needUpdateViewMatrix_ (true),
    needUpdateProjectionMatrix_(true),
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
	viewMatrix_(1.0f),
	fov_(80.0f),
	near_(NEAR_PLANE),
	far_(MAX_PLANE),
	needUpdateDebugFrustum_(true),
	frustum_(*this) {

	// Temp fix to have voxels filled at startup
	// new edit: do not work anymore :(
	processMouseMovement(0.01, 0.01);
	updateCameraVectors_();
}

void 		Camera::update() {
	if (frustum_.isDebug()) {
		if (needUpdateDebugFrustum_)
			frustum_.update();
	}
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
		needUpdateDebugFrustum_ = true;
	}
	return viewMatrix_;
}

glm::mat4 Camera::getProjectionMatrix() {
    if (needUpdateProjectionMatrix_) {
        projectionMatrix_ = glm::perspective(glm::radians(fov_),
                                             static_cast<float>(MainGraphic::Get().getRenderBuffer().width) / static_cast<float>(MainGraphic::Get().getRenderBuffer().height),
                                             near_, far_);
        needUpdateProjectionMatrix_ = false;
        needUpdateDebugFrustum_ = true;
    }
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

void Camera::updateProjection() {
    needUpdateProjectionMatrix_ = true;
    getProjectionMatrix();
}


bool Camera::getDebugFrustum() const {
	return frustum_.isDebug();
}

void Camera::setDebugFrustum(bool b) {
    std::cout << __FUNCTION_NAME__ << std::endl;
    needUpdateDebugFrustum_ = b;
	frustum_.setDebug(b);
}

void Camera::setFov(float fov) {
    fov_ = fov;
    needUpdateProjectionMatrix_ = true;
}
void Camera::setNear(float near) {
    near_ = near;
    needUpdateProjectionMatrix_ = true;
}
void Camera::setFar(float far) {
    far_ = far;
    needUpdateProjectionMatrix_ = true;
}

float Camera::getFov() const {
    return fov_;
}
float Camera::getNear() const {
    return near_;
}
float Camera::getFar() const {
    return far_;
}

Camera *Camera::focus = nullptr;
