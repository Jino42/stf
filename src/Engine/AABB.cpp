#include "AABB.hpp"
#include <iostream>

AABB::AABB()
    : min_(glm::vec3(-1.0f, -1.0f, -1.0f)),
      max_(glm::vec3(1.0f, 1.0f, 1.0f)) {
  setDebug();
}

void AABB::setMin(glm::vec3 const &vec) {
    min_ = vec;
    updateLines();
}
void AABB::setMax(glm::vec3 const &vec) {
    max_ = vec;
    updateLines();
}

void AABB::updateLines() {
  linesObject_.clear();
  linesObject_.reserve(24);

  linesObject_.emplace_back(min_, color);
  linesObject_.emplace_back(glm::vec3(min_.x, min_.y, max_.z), color);
  linesObject_.emplace_back(min_, color);
  linesObject_.emplace_back(glm::vec3(min_.x, max_.y, min_.z), color);
  linesObject_.emplace_back(min_, color);
  linesObject_.emplace_back(glm::vec3(max_.x, min_.y, min_.z), color);

  linesObject_.emplace_back(max_, color);
  linesObject_.emplace_back(glm::vec3(max_.x, max_.y, min_.z), color);
  linesObject_.emplace_back(max_, color);
  linesObject_.emplace_back(glm::vec3(max_.x, min_.y, max_.z), color);
  linesObject_.emplace_back(max_, color);
  linesObject_.emplace_back(glm::vec3(min_.x, max_.y, max_.z), color);



  linesObject_.emplace_back(glm::vec3(min_.x, min_.y, max_.z), color);
  linesObject_.emplace_back(glm::vec3(min_.x, max_.y, max_.z), color);
  linesObject_.emplace_back(glm::vec3(min_.x, min_.y, max_.z), color);
  linesObject_.emplace_back(glm::vec3(max_.x, min_.y, max_.z), color);

  linesObject_.emplace_back(glm::vec3(min_.x, max_.y, min_.z), color);
  linesObject_.emplace_back(glm::vec3(max_.x, max_.y, min_.z), color);

  linesObject_.emplace_back(glm::vec3(max_.x, min_.y, min_.z), color);
  linesObject_.emplace_back(glm::vec3(max_.x, min_.y, max_.z), color);

  linesObject_.emplace_back(glm::vec3(max_.x, max_.y, min_.z), color);
  linesObject_.emplace_back(glm::vec3(max_.x, min_.y, min_.z), color);

  linesObject_.emplace_back(glm::vec3(min_.x, max_.y, max_.z), color);
  linesObject_.emplace_back(glm::vec3(min_.x, max_.y, min_.z), color);

  updateDebug();
}
