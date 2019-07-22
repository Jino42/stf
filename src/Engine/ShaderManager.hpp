#pragma once

#include <Engine/Shader.hpp>
#include <map>

class ShaderManager {
public:

    static ShaderManager &Get() {
        if (!instance_)
            instance_ = std::make_unique<ShaderManager>();
        return *ShaderManager::instance_;
    }

    void addShader(std::string const &name);
    Shader &getShader(std::string const &name);

    void reload();

private:
    std::map< std::string, Shader > mapShaders_;

    static std::unique_ptr<ShaderManager> instance_;
};