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