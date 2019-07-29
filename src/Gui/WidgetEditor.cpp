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

    if (ImGui::CollapsingHeader("Camera"))
    {
        ImGui::Text("PROGRAMMER GUIDE:");
        ImGui::Separator();

        ImGui::Text("USER GUIDE:");
        ImGui::LabelText("label", "Value");
/*
        {
            if (ImGui::BeginCombo("Sound", core_.univers.getSoundManager().getNextLibraryInfo().title.c_str(), 0)) {

                for (unsigned long n = 0; n < SoundDynamicLibrary::libraryInfo.size(); n++) {
                    bool is_selected = (core_.univers.getSoundManager().getNextLibraryInfo().kLibrary == SoundDynamicLibrary::libraryInfo[n].kLibrary);
                    if (ImGui::Selectable(SoundDynamicLibrary::libraryInfo[n].title.c_str(), is_selected)) {
                        if (sound_) {
                            sound_ = false;
                            sound_ = soundManagement_();
                        }
                        if (!sound_)
                            core_.univers.getSoundManager().setNextKInstance(SoundDynamicLibrary::libraryInfo[n].kLibrary);
                    }
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();

            }

            static ImGuiComboFlags flags = 0;
            ImGui::CheckboxFlags("ImGuiComboFlags_PopupAlignLeft", (unsigned int*)&flags, ImGuiComboFlags_PopupAlignLeft);
            ImGui::SameLine();
            if (ImGui::CheckboxFlags("ImGuiComboFlags_NoArrowButton", (unsigned int*)&flags, ImGuiComboFlags_NoArrowButton))
                flags &= ~ImGuiComboFlags_NoPreview;     // Clear the other flag, as we cannot combine both
            if (ImGui::CheckboxFlags("ImGuiComboFlags_NoPreview", (unsigned int*)&flags, ImGuiComboFlags_NoPreview))
                flags &= ~ImGuiComboFlags_NoArrowButton; // Clear the other flag, as we cannot combine both

            const char* items[] = { "AAAA", "BBBB", "CCCC", "DDDD", "EEEE", "FFFF", "GGGG", "HHHH", "IIII", "JJJJ", "KKKK", "LLLLLLL", "MMMM", "OOOOOOO" };
            static const char* item_current = items[0];            // Here our selection is a single pointer stored outside the object.
            if (ImGui::BeginCombo("combo 1", item_current, flags)) // The second parameter is the label previewed before opening the combo.
            {
                for (int n = 0; n < IM_ARRAYSIZE(items); n++)
                {
                    bool is_selected = (item_current == items[n]);
                    if (ImGui::Selectable(items[n], is_selected))
                        item_current = items[n];
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();   // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
                }
                ImGui::EndCombo();
            }
        }
        */
    }

    /*
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
*/
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
