//
// Created by Noe TONIOLO on 2019-06-10.
//

#include "WidgetEditor.hpp"
#include "Engine/MainGraphic.hpp"
#include "Particle/ParticleModule/ModuleSizeOverLifetime.hpp"
#include <Engine/Time.hpp>
#include <sstream>
#include <Engine/Display/DisplayWindow.hpp>

WidgetEditor::WidgetEditor() :
        AWidget("Editor", NTL_IMGUI_WINDOW_NO_FLAGS) {
}

void WidgetEditor::beginContent_() {
    static Timer test;

    if (DisplayWindow::Get().getKey(GLFW_KEY_1) == KeyState::kDown)
        test.stop();
    if (DisplayWindow::Get().getKey(GLFW_KEY_2) == KeyState::kDown)
        test.start();

    std::stringstream ss;

    ss << test;

    std::string s = ss.str();
    ImGui::Text(s.c_str());

    const float   u32_zonce = -1.0f, u32_max = 10.f;
    static float    timerSpeed = test.getSpeed();
    ImGui::SliderScalar("Max",   ImGuiDataType_Float,  &timerSpeed, &u32_zonce, &u32_max);
    if (DisplayWindow::Get().getKey(GLFW_KEY_3) == KeyState::kDown) {
        test.setSpeed(timerSpeed);
    }

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
