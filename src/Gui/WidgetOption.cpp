#include "WidgetOption.hpp"
#include <Gui/Gui.hpp>
#include <Gui/GuiSettings.hpp>

WidgetOption::WidgetOption() :
        AWidget("Options", NTL_IMGUI_WINDOW_FLAGS_BASIC),
        sound_(true),
        rNoise_(false),
        rMusique_(true) {
}

void WidgetOption::beginContent_() {
    if (ImGui::InputText("Name", nameBuffer_,
                         IM_ARRAYSIZE(nameBuffer_) - 1,
                         ImGuiInputTextFlags_EnterReturnsTrue)) {
        std::cout << "Enter with : [" << nameBuffer_ << "]" << std::endl;
    }

    if (ImGui::Checkbox("Son", &sound_))
        sound_ = !sound_;
}
