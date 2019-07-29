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
    for (auto &camera : bufferCameras_)
        camera.update();
}

void CameraManager::setFocus(Camera &camera) {
    focus_ = &camera;
    Camera::focus = focus_;
}

void CameraManager::setFocus(std::string const &name) {

    auto it = std::find_if(bufferCameras_.begin(), bufferCameras_.end(),
                           [name](const Camera & camera) -> bool {
                               std::cout << "camera.name[" << camera.name << "]" "== name[" << name << "] =" << (camera.name == name) << std::endl;
                               return camera.name == name;
                           });
    if (it != bufferCameras_.end()) {
        focus_ = &(*it);
        Camera::focus = focus_;
    }

}

Camera &CameraManager::getFocus() {
    return *focus_;
}

void CameraManager::addCamera(std::string const &name) {


    auto it = std::find_if(bufferCameras_.begin(), bufferCameras_.end(),
                           [name](const Camera &camera) -> bool { return camera.name == name; });
    if (it != bufferCameras_.end())
        throw INommable::NameAlreadyTaken(std::string("Name [") + name + "] is already taken");

    bufferCameras_.emplace_back(name);

}

Camera &CameraManager::getCamera(std::string const &name) {
    auto it = std::find_if(bufferCameras_.begin(), bufferCameras_.end(),
                           [name](const Camera &camera) -> bool { return camera.name == name; });
    if (it == bufferCameras_.end())
        return *focus_;
    return *it;
}

//void CameraManager::removeCamera(std::string const &name) {
//}


CameraManager &CameraManager::Get() {
    if (!instance_)
        instance_ = std::make_unique<CameraManager>();
    return *instance_;
}

std::list<Camera> &CameraManager::getListCameras() {
    return bufferCameras_;
}

std::unique_ptr<CameraManager> CameraManager::instance_ = nullptr;