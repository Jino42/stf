#include "cl_type.hpp"

cl_float3 glmVec3toClFloat3(glm::vec3 vec) {
    cl_float3 ret;

    *reinterpret_cast<glm::vec3 *>(&ret) = vec;
    ret.w = 0.0f;
    return ret;
}

cl_float4 glmVec4toClFloat4(glm::vec4 vec) {
    cl_float4 ret;

    *reinterpret_cast<glm::vec4 *>(&ret) = vec;
    return ret;
}

cl_float3 makeFloat3(float x, float y, float z) {
    cl_float3 ret;

    ret.x = x;
    ret.y = y;
    ret.z = z;
    return ret;
}