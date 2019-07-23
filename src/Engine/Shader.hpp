#pragma once
///
/// \file       Shader.hpp
/// \author     Noé
/// \version    1.0
/// \brief      Generate Shader program from sources files
/// \details    Sources files accepted : .vert .geom .frag
///

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

/// \brief Shader class
/// \section HowToUse
/// \snippet snippetShader.cpp Shader example
class Shader {
public:

    class LinkException : public std::invalid_argument {
    public:
        LinkException() noexcept = delete;
        explicit LinkException(std::string const &s) noexcept : invalid_argument(s) {};
        ~LinkException() noexcept override = default;
        LinkException(LinkException const &src) noexcept = delete;
        LinkException &operator=(LinkException const &rhs) noexcept = delete;
    };

    class CreateException : public std::invalid_argument {
    public:
        CreateException() noexcept = delete;
        explicit CreateException(std::string const &s) noexcept : invalid_argument(s) {};
        ~CreateException() noexcept override = default;
        CreateException(CreateException const &src) noexcept = delete;
        CreateException &operator=(CreateException const &rhs) noexcept = delete;
    };
	
    Shader() noexcept;
	~Shader() noexcept;
    Shader(Shader const &shader) = delete;
    Shader &operator=(Shader const &shader) = delete;


    /// \brief Attach a file
    /// \details Dont compile the Shader, only store the path of the attached file
    ///             You need make multiple attach if you want put the Vertex + Geometry + Fragment
    /// \param Path to the shader
    /// \return Itself reference
    /// \section Example
    /// \snippet snippetShader.cpp ShaderAttach example
    /// \snippet snippetShader.cpp ShaderAttach exampleChaining
	Shader		&attach(std::string const &filename);

    /// \brief Compile file and Link the Program Shader
    /// \details Process when you are attach all your file to the Shader.
    /// \return Itself reference
    /// \throw CreateException : If compilation doesnt work
    /// \throw LinkException : If Link doesnt work
    /// \section Example
    /// \snippet snippetShader.cpp ShaderLink example
    /// \snippet snippetShader.cpp ShaderLink exampleChaining
	Shader		&link();

    /// \brief Recompile the Shader
    /// \details Try to Compile and link to a new Shader,
    ///             If will success, destroy the old Shader to replace by the newest
    /// \return Itself reference for chaining
    /// \section Example
    /// \snippet snippetShader.cpp ShaderRecompile example
	Shader      &recompile();

    /// \brief Activate the Program Shader
    /// \return Itself reference for chaining
    /// \section Example
    /// \snippet snippetShader.cpp ShaderActivate example
    Shader		&activate() noexcept;

    /// \name Set Uniform
    /// \snippet snippetShader.cpp ShaderSetUniform example
    /// \snippet snippetShader.cpp ShaderSetUniform exampleChaining
    /// \param name : Name of Uniform Shader
    /// \param value : Value to set to the Uniform target
    /// \return Itself const reference for chaining
    /// \throw invalid_argument : If glGetUniformLocation dont find Uniform location
    /// @{
    Shader		const &setFloat(const std::string &name, float value) const;
	Shader		const &setMat4(const std::string &name, const glm::mat4 &value) const;
	Shader		const &setInt(const std::string &name, int value) const;
	Shader		const &setUInt(const std::string &name, unsigned int value) const;
	Shader		const &setVec3(const std::string &name, const glm::vec3 &value) const;
	/// @}

private:

    std::vector< std::string > files_;
	GLuint program_;
	GLint  status_;
	GLint  length_;

    GLuint		create_(std::string const &filename);
    void        compileShader_(std::string const &filename);

	static bool				debug_;
};

/// \example exampleShader.cpp
/// This is an example of how to use the Shader class.
