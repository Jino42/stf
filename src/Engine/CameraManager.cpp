#include "CameraManager.hpp"
#include <iostream>

CameraManager::CameraManager() :
	focus_(nullptr){
}

void CameraManager::init() {
	addCamera("Default");
	setFocus("Default");
}

void CameraManager::update() {
	for (auto &pair : mapCameras_)
		pair.second.update();
}

void CameraManager::setFocus(Camera &camera) {
	focus_ = &camera;
	Camera::focus = focus_;
}

void CameraManager::setFocus(std::string const &name) {
	focus_ = &mapCameras_.at(name);
	Camera::focus = focus_;
}

Camera &CameraManager::getFocus() {
	return *focus_;
}

void CameraManager::addCamera(std::string const &name) {

	if (mapCameras_.find(name) == mapCameras_.end()) {
		mapCameras_.try_emplace(name);
	}
}

Camera &CameraManager::getCamera(std::string const &name) {
	return *focus_;
}

//void CameraManager::removeCamera(std::string const &name) {
//}


CameraManager &CameraManager::Get() {
	if (!instance_)
		instance_ = std::make_unique<CameraManager>();
	return *instance_;
}

std::unique_ptr<CameraManager> CameraManager::instance_ = nullptr;