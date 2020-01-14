#pragma once

#include <nlohmann/json.hpp>
#include <iostream>
#include "cl_type.hpp"
#include "Range.hpp"

using json = nlohmann::json;

json getJsonFromFileAbsolutePath(std::string const &path);
json getJsonFromFileRelativeToRootPath(std::string const &path);

Rangef jsonToRangef(json &j);

cl_float3 jsonToFloat3(json &j);
cl_float3 jsonToFloat3(json &&j);
glm::vec3 jsonToVec3(json &j);
glm::vec3 jsonToVec3(json &&j);