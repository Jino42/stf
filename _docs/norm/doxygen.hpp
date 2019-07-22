///
/// \file
/// \author
/// \version
/// \brief
/// \details
///

#pragma once

/// \brief
/// \details
namespace UJK {

    /// \brief
    /// \details
    enum class eDirection { None, ///< No direction
                            East,
                            West,
                            South,
                            North
    };

    /// \brief
    /// \details
    struct Point {
        int x;
        int y;
    };

    /// \brief
    /// \details
    class Actor {
    public:
    private:
        Point position_; ///< Position of the Actor
    };

    /// \brief
    /// \details
    /// \param
    /// \return
    /// \throw invalid_argument : If glGetUniformLocation dont find Uniform location
    /// \section Example
    /// \snippet snippetPrint42.cpp Print42 example
    void print42(int nbToPrint)
    {
        for (int i = 0; i < nbToPrint; i++)
            write(1, "42", 2);
    }
}

/// \example examplePrint42.cpp
/// This is an example of how to use the Shader class.