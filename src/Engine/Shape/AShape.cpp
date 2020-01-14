#include "AShape.hpp"
#include "Sphere.hpp"
#include "AABB.hpp"
#include <cl_type.hpp>


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

cl_Shape *AShape::getCl_Shape() const {
    cl_Shape *shape = new cl_Shape();
    shape->flag = static_cast<int>(flag_);
    shape->position = glmVec3toClFloat3(position_);
    return shape;
}

void AShape::printShapeStructSize() {
    printf(">>> CPU SIZE STRUCT SHAPE <<<\n");
    printf("[%5li] Shape\n", sizeof(cl_Shape));
    printf("[%5li] Sphere\n", sizeof(cl_Sphere));
    printf("[%5li] AABB\n", sizeof(cl_AABB));
}