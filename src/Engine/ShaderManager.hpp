#pragma once
///
/// \file       ShaderManager.hpp
/// \author     Noé
/// \version    1.0
/// \brief      Singleton, Store shader
/// \details    A Shader can be use anywhere, and permit to recompile one or all Shader easily
///

#include <Engine/Shader.hpp>
#include <map>

/// \brief Singleton
class ShaderManager {
public:

    static ShaderManager &Get() {
        if (!instance_)
            instance_ = std::make_unique<ShaderManager>();
        return *ShaderManager::instance_;
    }

    /// \brief Add Shader place
    /// \details Create a Shader, only, with default constructor, and store him to a Map, associated with his name.
    ///              He is not necessary. You can getShader without add Shader, that will create them
    /// \param Name of Shader
    /// \section Example
    /// \snippet snippetShaderManager.cpp ShaderManagerAddShader example
    void addShader(std::string const &name);

    /// \brief Get Shader reference
    /// \details Shearch in map, the Shader associated to this name
    ///             If the name are not found, addShader will called for add it
    /// \param Name of Shader
    /// \return Shader Reference
    /// \throw It throws out_of_range if "Name" is not the key of an element in the Shaders map.
    /// \section Example
    /// \snippet snippetShaderManager.cpp ShaderManagerGetShader example
    Shader &getShader(std::string const &name);

    /// \brief Recompile all Shader
    /// \details ForEach Shader stored, recompile
    /// \section Example
    /// \snippet snippetShaderManager.cpp ShaderManagerReload example
    void reload();

private:
    std::map< std::string, Shader > mapShaders_;

    static std::unique_ptr<ShaderManager> instance_;
};