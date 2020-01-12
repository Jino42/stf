#include "ShapeManager.hpp"

ShapeManager &ShapeManager::Get() {
    if (!instance_)
        instance_ = std::make_unique<ShapeManager>();
    return *instance_;
}

std::shared_ptr<AShape> ShapeManager::getShape(std::string const &name) {
    return mapShapes_.at(name);
}

std::map<std::string, std::shared_ptr<AShape>> &ShapeManager::getMapShape() {
    return mapShapes_;
}

std::unique_ptr<ShapeManager> ShapeManager::instance_ = nullptr;
