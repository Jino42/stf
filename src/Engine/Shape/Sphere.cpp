#include "Sphere.hpp"
#include <cl_type.hpp>
#include <iostream>

Sphere::Sphere()
    : AShape(eShape::kSphere),
      radius_(1.0f),
      sectorCount_(36),
      stackCount_(18) {
    setDebug();
}

void Sphere::setRadius(float radius) {
    radius_ = radius;
    updateLines();
}
void Sphere::setPosition(glm::vec3 position) {
    AShape::setPosition(position);
    updateLines();
}

cl_Sphere *Sphere::getCl_Shape() const {
    cl_Sphere *shape = new cl_Sphere();
    shape->flag = static_cast<int>(flag_);
    shape->position = glmVec3toClFloat3(position_);
    shape->radius = radius_;
    return shape;
}

void Sphere::updateLines() {
    std::vector<glm::vec3> vertex;
    linesObject_.clear();

    float x, y, z, xy;                            // vertex position
    float nx, ny, nz, lengthInv = 1.0f / radius_; // vertex normal
    float s, t;                                   // vertex texCoord

    float sectorStep = 2 * 3.14159265f / sectorCount_;
    float stackStep = 3.14159265f / stackCount_;
    float sectorAngle, stackAngle;

    for (int i = 0; i <= stackCount_; ++i) {
        stackAngle = 3.14159265f / 2 - i * stackStep; // starting from pi/2 to -pi/2
        xy = radius_ * cosf(stackAngle);              // r * cos(u)
        z = radius_ * sinf(stackAngle);               // r * sin(u)

        // add (sectorCount+1) vertices per stack
        // the first and last vertices have same position and normal, but different tex coords
        for (int j = 0; j <= sectorCount_; ++j) {
            sectorAngle = j * sectorStep; // starting from 0 to 2pi

            // vertex position (x, y, z)
            x = xy * cosf(sectorAngle); // r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle); // r * cos(u) * sin(v)

            vertex.push_back(glm::vec3(x, y, z) + position_);
        }
    }

    int k1, k2;
    for (int i = 0; i < stackCount_; ++i) {
        k1 = i * (sectorCount_ + 1); // beginning of current stack
        k2 = k1 + sectorCount_ + 1;  // beginning of next stack

        for (int j = 0; j < sectorCount_; ++j, ++k1, ++k2) {
            // 2 triangles per sector excluding first and last stacks
            // k1 => k2 => k1+1
            if (i != 0) {
                linesObject_.push_back(DebugVertex(vertex[k1], color));
                linesObject_.push_back(DebugVertex(vertex[k2], color));
                linesObject_.push_back(DebugVertex(vertex[k1], color));
                linesObject_.push_back(DebugVertex(vertex[k1 + 1], color));
            }

            // k1+1 => k2 => k2+1
            if (i != (stackCount_ - 1)) {
                linesObject_.push_back(DebugVertex(vertex[k1 + 1], color));
                linesObject_.push_back(DebugVertex(vertex[k2], color));
                linesObject_.push_back(DebugVertex(vertex[k1 + 1], color));
                linesObject_.push_back(DebugVertex(vertex[k2 + 1], color));
            }
        }
    }

    updateDebug();
}