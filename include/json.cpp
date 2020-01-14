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

cl_float3 jsonToFloat3(json &j) {
    return makeFloat3(j["x"].get<float>(), j["y"].get<float>(), j["z"].get<float>());
}
cl_float3 jsonToFloat3(json &&j) {
    return makeFloat3(j["x"].get<float>(), j["y"].get<float>(), j["z"].get<float>());
}

glm::vec3 jsonToVec3(json &j) {
    return glm::vec3(j["x"].get<float>(), j["y"].get<float>(), j["z"].get<float>());
}
glm::vec3 jsonToVec3(json &&j) {
    return glm::vec3(j["x"].get<float>(), j["y"].get<float>(), j["z"].get<float>());
}