//
// Created by Noe TONIOLO on 2019-06-10.
//

#include "WidgetRender.hpp"
#include "Engine/MainGraphic.hpp"
#include "Gui/WidgetRender.hpp"

WidgetRender::WidgetRender() :
        AWidget("Render", NTL_IMGUI_WINDOW_NO_FLAGS) {
}

void WidgetRender::beginContent_() {

    ImGui::GetWindowDrawList()->AddImage(
            (void *)MainGraphic::Get().renderBuffer.getTextureBufferId(),
            ImVec2(ImGui::GetCursorScreenPos()),
            ImVec2(ImGui::GetCursorScreenPos().x + MainGraphic::Get().renderBuffer.width,
                   ImGui::GetCursorScreenPos().y + MainGraphic::Get().renderBuffer.height), ImVec2(0, 1), ImVec2(1, 0));
    widgetPosition = ImGui::GetWindowPos();
    widgetSize = ImGui::GetWindowSize();
}

WidgetRender &WidgetRender::Get() {
    if (!WidgetRender::instance_)
        WidgetRender::instance_ = std::make_unique<WidgetRender>();
    return *WidgetRender::instance_;
}

std::unique_ptr<WidgetRender> WidgetRender::instance_ = nullptr;
