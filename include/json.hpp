#pragma once

#include <nlohmann/json.hpp>
#include <iostream>
#include "cl_type.hpp"

using json = nlohmann::json;

json getJsonFromFileAbsolutePath(std::string const &path);
json getJsonFromFileRelativeToRootPath(std::string const &path);

cl_float3 jsonToFloat3(json &j);