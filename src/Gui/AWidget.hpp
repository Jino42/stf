#pragma once

#include "imgui.h"
#include <exception>
#include <string>
#include <map>
#define NTL_IMGUI_WINDOW_NO_FLAGS 0
#define NTL_IMGUI_WINDOW_FLAGS_BASIC (ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)
#define NTL_IMGUI_WINDOW_FLAGS_NO_INPUT (ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMouseInputs)

class AWidget {
public:
    enum class eColor { kNone, kGrey, kRed, kGreen, kPurple, kHard, kMedium, kSoft, kLight};

    enum class eColorLog {
        kNone,
        kRed,
        kGreen,
        kBlue,
        kPink,
        kOrange,
        kYellow
    };

    AWidget(std::string const &winName, ImGuiWindowFlags winFlags);
    virtual ~AWidget() = default;
    AWidget() = delete;
    AWidget &operator=(const AWidget &) = default;
    AWidget(const AWidget &) = default;


    void render(bool renderContentInWindow);

    static void			    		beginHueColor_(float color, float power = 1.f);
    static void						beginColor(eColor color, eColor power = eColor::kHard);
    static void						endColor();

protected:
    bool active_;
    std::string winName_;
    ImGuiWindowFlags winFlags_;



private:
    virtual void update_();
    virtual void beginContent_() = 0;
    static bool 				            useColor_;
    static std::map< eColor, float > const  mapEColor_;
};