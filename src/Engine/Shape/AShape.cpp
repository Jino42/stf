#include "AShape.hpp"

AShape::AShape()
    : flag_(0),
      position_(0.0f) {
}


glm::vec3 &AShape::getPosition() {
    return position_;
}

void AShape::setPosition(glm::vec3 position) {
    position_ = position;
}