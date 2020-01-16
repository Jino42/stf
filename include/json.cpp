#include <json.hpp>
#include <boost/filesystem.hpp>

json getJsonFromFileAbsolutePath(std::string const &path) {
	std::ifstream ifs(path.c_str());
	return json::parse(ifs);
}

json getJsonFromFileRelativeToRootPath(std::string const &path) {
	std::ifstream ifs((boost::filesystem::path(ROOT_PATH) / path).generic_string());
	return json::parse(ifs);
}

Rangef jsonToRangef(json &j) {
    Rangef range;
    range.rangeMin = j["min"].get<float>();
    range.rangeMax = j["max"].get<float>();
    return range;
}

static void setXYZ(json &j, float *x, float *y, float *z) {
    if (j.find("x") != j.end())
        *x = j["x"].get<float>();
    if (j.find("r") != j.end())
        *x = j["r"].get<float>();

    if (j.find("y") != j.end())
        *y = j["y"].get<float>();
    if (j.find("g") != j.end())
        *y = j["g"].get<float>();

    if (j.find("z") != j.end())
        *z = j["z"].get<float>();
    if (j.find("b") != j.end())
        *z = j["b"].get<float>();
}

cl_float3 jsonToFloat3(json &j) {
    glm::vec3 v;
    setXYZ(j, &v.x, &v.y, &v.z);
    return makeFloat3(v.x, v.y, v.z);
}
cl_float3 jsonToFloat3(json &&j) {
    glm::vec3 v;
    setXYZ(j, &v.x, &v.y, &v.z);
    return makeFloat3(v.x, v.y, v.z);
}

glm::vec3 jsonToVec3(json &j) {
    glm::vec3 v;
    setXYZ(j, &v.x, &v.y, &v.z);
    return v;
}
glm::vec3 jsonToVec3(json &&j) {
    glm::vec3 v;
    setXYZ(j, &v.x, &v.y, &v.z);
    return v;
}