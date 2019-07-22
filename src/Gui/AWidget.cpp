#include "AWidget.hpp"
#include <gui/Gui.hpp>

AWidget::AWidget(std::string const &winName, ImGuiWindowFlags winFlags) :
        active_(true),
        winName_(winName),
        winFlags_(winFlags) {
}

void AWidget::update_() {}

void AWidget::render(bool renderContentInWindow) {
    update_();
    if (active_) {
        if (!renderContentInWindow) {
            ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(100, 100, 100, 100));
            ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(100, 100, 100, 100));
            ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, IM_COL32(100, 100, 100, 100));
            ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(100, 100, 100, 100));
        }
        ImGui::Begin(winName_.c_str(), NULL, winFlags_);
        if (renderContentInWindow)
            beginContent_();
        if (!renderContentInWindow)
            ImGui::PopStyleColor(4);
        ImGui::End();
    }
}

/*
bool AWidget::button_(std::string const &text, int sizeTextX, eColor color, eColor power) const {
    if (color != eColor::kNone)
        AWidget::beginColor(color, power);
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - sizeTextX) / 2);
    bool ret =  ImGui::Button(text.c_str(), ImVec2(sizeTextX, sizeTextX + 2));
    if (color != eColor::kNone)
        AWidget::endColor();
    return ret;
}

bool AWidget::button_(std::string const &text,  eColor color, eColor power) const {
    return button_(text, ImGui::GetWindowSize().x * 0.75f, color, power);
}
*/

void					AWidget::beginHueColor_(float color, float power) {
    ImGui::PushStyleColor(ImGuiCol_Button, static_cast<ImVec4>(ImColor::HSV(color, 0.7f * power, 0.7f * power)));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, static_cast<ImVec4>(ImColor::HSV(color, 0.8f * power, 0.8f * power)));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, static_cast<ImVec4>(ImColor::HSV(color, 0.9f * power, 0.9f * power)));
}
void					AWidget::beginColor(eColor color, eColor power) {
    assert(!AWidget::useColor_);
    AWidget::useColor_ = true;
    if (color == eColor::kGrey) {
        ImGui::PushStyleColor(ImGuiCol_Button, static_cast<ImVec4>(ImColor::HSV(0.f, 0.f, 0.5f * AWidget::mapEColor_.at(power))));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, static_cast<ImVec4>(ImColor::HSV(0.f, 0.0f, 0.5f * AWidget::mapEColor_.at(power))));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, static_cast<ImVec4>(ImColor::HSV(0.f, 0.00f, 0.65f * AWidget::mapEColor_.at(power))));
    } else
        beginHueColor_(AWidget::mapEColor_.at(color), AWidget::mapEColor_.at(power));
}
void					AWidget::endColor() {
    ImGui::PopStyleColor(3);
    AWidget::useColor_ = false;
}

std::map< AWidget::eColor, float > const AWidget::mapEColor_ = { { eColor::kGrey, 0.f },
                                                    { eColor::kRed, 0.f },
                                                    { eColor::kGreen, 0.33f },
                                                    { eColor::kPurple, 0.77f },
                                                    { eColor::kHard, 1.f },
                                                    { eColor::kMedium, .75f },
                                                    { eColor::kSoft, .5f },
                                                    { eColor::kLight, .33f }};

bool 						AWidget::useColor_ = false;