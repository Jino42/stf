#include "PathManager.hpp"
#include <utility>
#include "json.hpp"

PathManager::PathManager() {
	boost::filesystem::path pathRoot(ROOT_PATH);
	addPath("rootPath", ROOT_PATH);

	json &&j = getJsonFromFileRelativeToRootPath("config/path.json");

	for (auto &it : j["paths"]) {
		std::cout << it["name"] << std::endl;
		std::cout << it["path"] << std::endl;
		addPath(it["name"].get<std::string>(), pathRoot / it["path"].get<std::string>());
	}
}

PathManager &PathManager::Get() {
	if (!instance_)
		instance_ = std::make_unique<PathManager>();
	return *instance_;
}

boost::filesystem::path &PathManager::getPath(std::string const &name) {
	return mapPaths_.at(name);
}

void PathManager::addPath(std::string const &name, boost::filesystem::path path) {
	if (mapPaths_.find(name) == mapPaths_.end())
		mapPaths_.insert(std::make_pair(name, path));
}

std::unique_ptr<PathManager> PathManager::instance_ = nullptr;
