#include "ShaderManager.hpp"


void ShaderManager::addShader(std::string const &name) {
    mapShaders_.try_emplace(name);
}

Shader &ShaderManager::getShader(std::string const &name) {
    return mapShaders_.at(name);
}

void ShaderManager::reload() {
    for (auto &shader: mapShaders_)
        shader.second.recompile();
}

std::unique_ptr<ShaderManager> ShaderManager::instance_ = nullptr;