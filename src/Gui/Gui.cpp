//
// Created by Noe TONIOLO on 2019-06-09.
//

#include "Gui.hpp"
#include "Engine/Display/DisplayWindow.hpp"

ImVec2   Gui::positionByPercent(ImVec2 const &percent) const {
    return (ImVec2(DisplayWindow::Get().getWidthWindow() * percent.x / 100.0f,
            DisplayWindow::Get().getHeightWindow() * percent.y / 100.0f));
}