//
// Created by Noe TONIOLO on 2019-06-10.
//

#include "WidgetEditor.hpp"
#include "Engine/MainGraphic.hpp"
#include "Particle/ParticleModule/ModuleSizeOverLifetime.hpp"

WidgetEditor::WidgetEditor() :
        AWidget("Editor", NTL_IMGUI_WINDOW_NO_FLAGS) {
}

void WidgetEditor::beginContent_() {
    /*
    if (ImGui::CollapsingHeader("Required")) {
        //static float begin = 1, end = 1000;

        ImGui::DragFloatRange2("range",
                &MainGraphic::Get().particleSystem_.getEmitter("Lol").getModule<ModuleSizeOverLifetime>()->getSizeMin(),
                &MainGraphic::Get().particleSystem_.getEmitter("Lol").getModule<ModuleSizeOverLifetime>()->getSizeMax(),
                0.25f, 0.0f, 100.0f, "Min: %.1f %%", "Max: %.1f %%");
        const unsigned int   u32_zonce = 1, u32_max = 11240;
        static unsigned int newMax = MainGraphic::Get().particleSystem_.getEmitter("Lol").getNbParticleMax();
        static unsigned int newPerSec = MainGraphic::Get().particleSystem_.getEmitter("Lol").getNbParticlePerSec();
        ImGui::SliderScalar("Max",   ImGuiDataType_U32,  &newMax, &u32_zonce, &u32_max);
        //NeedToReset = true
        ImGui::SliderScalar("Sec",   ImGuiDataType_U32,  &newPerSec, &u32_zonce, &u32_max);
        if (DisplayWindow::Get().getKey(GLFW_KEY_T)) {
            MainGraphic::Get().particleSystem_.getEmitter("Lol").setNbParticleMax(newMax);
            MainGraphic::Get().particleSystem_.getEmitter("Lol").setNbParticlePerSec(newPerSec);
            MainGraphic::Get().particleSystem_.getEmitter("Lol").setReload(true);
        }
    }
     */
}