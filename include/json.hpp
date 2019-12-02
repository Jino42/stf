#pragma once

#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

json getJsonFromFileAbsolutePath(std::string const &path);
json getJsonFromFileRelativeToRootPath(std::string const &path);