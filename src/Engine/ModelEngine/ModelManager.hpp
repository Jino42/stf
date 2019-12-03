#pragma once

#include <memory>
#include "Model.hpp"
#include <boost/filesystem.hpp>

class ModelManager {
public:
	ModelManager();
	ModelManager(ModelManager const &) = delete;
	~ModelManager() = default;
	static ModelManager &Get();

	void addModel(std::string const &name, boost::filesystem::path const &path);

	Model &getModel(std::string const &name);

private:
	std::map< std::string, Model > mapModels_;

	static std::unique_ptr<ModelManager> instance_;
};