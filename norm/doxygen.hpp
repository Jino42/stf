///
/// \file
/// \author
/// \version
/// \brief
/// \details
///

#pragma once

/// \namespace UJK
/// \brief
/// \details
namespace UJK {

    /// \enum eDirection
    /// \brief
    /// \details
    enum class eDirection { None, ///< No direction
                            East,
                            West,
                            South,
                            North
    };

    /// \struct Point
    /// \brief
    /// \details
    struct Point {
        int x;
        int y;
    };

    /// \class Actor
    /// \brief
    /// \details
    class Actor {
    public:
    private:
        Point position_; ///< Position of the Actor
    };

    /// \fn void print42(int nbToPrint)
    /// \brief
    /// \details
    /// \param nbToPrint : How many print
    /// \return
    void print42(int nbToPrint)
    {
        for (int i = 0; i < nbToPrint; i++)
            write(1, "42", 2);
    }
}

