#include "AABB.hpp"
#include <cl_type.hpp>
#include <iostream>

AABB::AABB()
    : AShape(eShape::kAABB),
      size_(glm::vec3(1.0f, 1.0f, 1.0f)) {
    setDebug();
}

void AABB::setPosition(glm::vec3 position) {
    AShape::setPosition(position);
    updateLines();
}

void AABB::setSize(glm::vec3 const &vec) {
    size_ = vec;
    updateLines();
}

cl_AABB *AABB::getCl_Shape() const {
    cl_AABB *shape = new cl_AABB();
    shape->flag = static_cast<int>(flag_);
    shape->position = glmVec3toClFloat3(position_);
    shape->size = glmVec3toClFloat3(size_);
    return shape;
}

void AABB::updateLines() {
    linesObject_.clear();
    linesObject_.reserve(24);

    glm::vec3 min = position_ - (size_ / 2.f);
    glm::vec3 max = position_ + (size_ / 2.f);

    linesObject_.emplace_back(min, color);
    linesObject_.emplace_back(glm::vec3(min.x, min.y, max.z), color);
    linesObject_.emplace_back(min, color);
    linesObject_.emplace_back(glm::vec3(min.x, max.y, min.z), color);
    linesObject_.emplace_back(min, color);
    linesObject_.emplace_back(glm::vec3(max.x, min.y, min.z), color);

    linesObject_.emplace_back(max, color);
    linesObject_.emplace_back(glm::vec3(max.x, max.y, min.z), color);
    linesObject_.emplace_back(max, color);
    linesObject_.emplace_back(glm::vec3(max.x, min.y, max.z), color);
    linesObject_.emplace_back(max, color);
    linesObject_.emplace_back(glm::vec3(min.x, max.y, max.z), color);

    linesObject_.emplace_back(glm::vec3(min.x, min.y, max.z), color);
    linesObject_.emplace_back(glm::vec3(min.x, max.y, max.z), color);
    linesObject_.emplace_back(glm::vec3(min.x, min.y, max.z), color);
    linesObject_.emplace_back(glm::vec3(max.x, min.y, max.z), color);

    linesObject_.emplace_back(glm::vec3(min.x, max.y, min.z), color);
    linesObject_.emplace_back(glm::vec3(max.x, max.y, min.z), color);

    linesObject_.emplace_back(glm::vec3(max.x, min.y, min.z), color);
    linesObject_.emplace_back(glm::vec3(max.x, min.y, max.z), color);

    linesObject_.emplace_back(glm::vec3(max.x, max.y, min.z), color);
    linesObject_.emplace_back(glm::vec3(max.x, min.y, min.z), color);

    linesObject_.emplace_back(glm::vec3(min.x, max.y, max.z), color);
    linesObject_.emplace_back(glm::vec3(min.x, max.y, min.z), color);

    updateDebug();
}
