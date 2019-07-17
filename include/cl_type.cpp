#include "cl_type.hpp"

cl_float3 glmVec3toClFloat3(glm::vec3 vec)
{
	cl_float3 ret;

	*reinterpret_cast<glm::vec3*>(&ret) = vec;
	ret.w = 0.0f;
	return ret;
}