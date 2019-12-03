#include "ModelManager.hpp"
#include <utility>
#include "json.hpp"

ModelManager::ModelManager() {
}

ModelManager &ModelManager::Get() {
	if (!instance_)
		instance_ = std::make_unique<ModelManager>();
	return *instance_;
}

Model &ModelManager::getModel(std::string const &name) {
	return mapModels_.at(name);
}

void ModelManager::addModel(std::string const &name, boost::filesystem::path const &path) {
	if (mapModels_.find(name) == mapModels_.end())
		mapModels_.try_emplace(name, path.generic_string());
}

std::unique_ptr<ModelManager> ModelManager::instance_ = nullptr;