#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <Engine/Display/DisplayWindow.hpp>

enum class eColor { kNone,
                    kGrey,
                    kRed,
                    kGreen,
                    kPurple,
                    kHard,
                    kMedium,
                    kSoft,
                    kLight };

class Gui {
public:
    Gui();
    ~Gui();


    void update();
    void render();
    void setDemo(bool b);
    ImVec2   positionByPercent(ImVec2 const &percent) const;
	static void HelpMarker(std::string const &desc);

    static void beginColor(eColor color, eColor power);
    static void beginHueColor(float color, float power);
    static void endColor();

private:
    bool show_demo_window;
    bool show_another_window;

    static bool useColor_;
    static std::map<eColor, float> const mapEColor_;
};