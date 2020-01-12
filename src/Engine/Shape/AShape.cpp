#include "AShape.hpp"

AShape::AShape()
    : flag_(eShape::kZero),
      position_(0.0f) {
}

AShape::AShape(eShape k)
    : flag_(k),
      position_(0.0f) {
}

glm::vec3 &AShape::getPosition() {
    return position_;
}

void AShape::setPosition(glm::vec3 position) {
    position_ = position;
}

void AShape::setRender(bool b) {
    if (b)
        flag_ |= eShape::kRender;
    else if ((flag_ & eShape::kRender) == eShape::kRender)
        flag_ ^= eShape::kRender;
}