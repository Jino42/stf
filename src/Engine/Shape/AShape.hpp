#pragma once
#include <NTL.hpp>

enum class eShape {
    kZero = 0,
    kNone = 1 << 0,
    kRender = 1 << 1,

    kMaskOptions = kRender,

    kSphere = 1 << 16,
    kAABB = 1 << 17,
    kPlane = 1 << 18,
    kTorus = 1 << 19,

    kMaskShape = kSphere | kAABB | kPlane | kTorus,
};

inline eShape operator|(eShape const lhs, eShape const rhs) {
    return static_cast<eShape>(static_cast<int>(lhs) | static_cast<int>(rhs));
}
inline eShape const &operator|=(eShape &lhs, eShape const &rhs) {
    lhs = static_cast<eShape>(static_cast<int>(lhs) | static_cast<int>(rhs));
    return (lhs);
}
inline eShape operator&(eShape const lhs, eShape const rhs) {
    return static_cast<eShape>(static_cast<int>(lhs) & static_cast<int>(rhs));
}
inline eShape operator&(int const lhs, eShape const rhs) {
    return static_cast<eShape>(lhs & static_cast<int>(rhs));
}
inline eShape operator&(eShape const lhs, int const rhs) {
    return static_cast<eShape>(static_cast<int>(lhs) & rhs);
}
inline eShape operator^(eShape const lhs, eShape const rhs) {
    return static_cast<eShape>(static_cast<int>(lhs) ^ static_cast<int>(rhs));
}
inline eShape operator^(int const lhs, eShape const rhs) {
    return static_cast<eShape>(lhs ^ static_cast<int>(rhs));
}
inline eShape operator^(eShape const lhs, int const rhs) {
    return static_cast<eShape>(static_cast<int>(lhs) ^ rhs);
}
inline eShape const &operator^=(eShape &lhs, eShape const &rhs) {
    lhs = static_cast<eShape>(static_cast<int>(lhs) ^ static_cast<int>(rhs));
    return (lhs);
}
inline eShape operator<<(eShape const lhs, eShape const rhs) {
    return static_cast<eShape>(static_cast<int>(lhs) << static_cast<int>(rhs));
}
inline eShape operator>>(eShape const lhs, eShape const rhs) {
    return static_cast<eShape>(static_cast<int>(lhs) >> static_cast<int>(rhs));
}

struct cl_Shape {
    //virtual ~cl_Shape() = default;
    cl_int flag;
    cl_float3 position;
};

class AShape {
  public:
    AShape();
    AShape(eShape k);
    virtual ~AShape() = default;

    glm::vec3 &getPosition();
    virtual void setPosition(glm::vec3 position);
    void setRender(bool b);

    virtual cl_Shape *getCl_Shape() const = 0;

    static void printShapeStructSize();

  protected:
    eShape flag_;
    glm::vec3 position_;
};