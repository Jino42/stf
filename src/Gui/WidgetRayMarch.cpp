//
// Created by Noe TONIOLO on 2019-06-10.
//

#include "Engine/MainGraphic.hpp"
#include "Gui/WidgetRayMarch.hpp"
#include "WidgetRayMarch.hpp"
#include <Gui/GuiSettings.hpp>
#include <Engine/Display/DisplayWindow.hpp>

WidgetRayMarch::WidgetRayMarch()
    : AWidget("RayMarch", NTL_IMGUI_WINDOW_NO_FLAGS) {
}

void WidgetRayMarch::beginContent_() {

    ImGui::GetWindowDrawList()->AddImage(
        (void *)MainGraphic::Get().getRenderBufferRayMarch().getTextureBufferId(),
        ImVec2(ImGui::GetCursorScreenPos()),
        ImVec2(ImGui::GetCursorScreenPos().x + MainGraphic::Get().getRenderBufferRayMarch().width,
               ImGui::GetCursorScreenPos().y + MainGraphic::Get().getRenderBufferRayMarch().height),
        ImVec2(0, 1), ImVec2(1, 0));
    widgetPosition = ImGui::GetWindowPos();
    widgetSize = ImGui::GetWindowSize();
}

WidgetRayMarch &WidgetRayMarch::Get() {
    if (!WidgetRayMarch::instance_)
        WidgetRayMarch::instance_ = std::make_unique<WidgetRayMarch>();
    return *WidgetRayMarch::instance_;
}

std::unique_ptr<WidgetRayMarch> WidgetRayMarch::instance_ = nullptr;
