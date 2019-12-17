#include "EditorParticleSystems.hpp"

#include "Particle/ParticleModule/ModuleSPH.hpp"
#include <Gui/Gui.hpp>
#include <Gui/GuiSettings.hpp>
#include <Particle/ParticleSystemManager.hpp>
#include <iostream>

EditorParticleSystems::EditorParticleSystems()
    : indexEmitter_(0), indexModule_(0) {
    reload();
}

void EditorParticleSystems::render() {
    if (!ImGui::CollapsingHeader("ParticleSystem"))
        return;
    ImGui::Separator();
    ImGui::Separator();

    std::map<std::string, ParticleSystem> &mapSystem = ParticleSystemManager::Get().getMapParticleSystem();

    ImGui::Text("Name: ");
    ImGui::InputText("Systems :", inputName_, IM_ARRAYSIZE(inputName_));
    ImGui::SameLine();
    //	if (ImGui::Button("Add"))
    //		CameraManager::Get().addCamera(inputCameraName_);

    ParticleSystem *ptrSystem = nullptr;
    if (mapSystem.find(keySystem_) != mapSystem.end())
        ptrSystem = &mapSystem.find(keySystem_)->second;
    if (ImGui::BeginCombo("Systems", (ptrSystem ? ptrSystem->getName().c_str() : ""), NTL_IMGUI_COMBO_FLAGS)) {
        for (auto &[name, system]: mapSystem) {
            bool is_selected = (ptrSystem == &system);
            if (ImGui::Selectable(system.getName().c_str(), is_selected))
                keySystem_ = system.getName();
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    if (ptrSystem) {
        if (ImGui::DragFloat3("Position", reinterpret_cast<float *>(&ptrSystem->getPosition()), 0.05f)) {
            ; // SEF
        }
        std::unique_ptr<AParticleEmitter> *ptrEmitter = nullptr;
        if (ptrSystem->getEmiters().size() > 0 && indexEmitter_ < ptrSystem->getEmiters().size()) {
            ptrEmitter = &(ptrSystem->getEmiters()[indexEmitter_]);
        }

        if (ImGui::BeginCombo("Emitters", (ptrEmitter ? (*ptrEmitter)->getName().c_str() : ""), NTL_IMGUI_COMBO_FLAGS)) {
            for (auto it = ptrSystem->getEmiters().begin(); it != ptrSystem->getEmiters().end(); ++it) {
                auto &emitter = *it;
                bool is_selected = (ptrEmitter == &emitter);
                if (ImGui::Selectable(emitter->getName().c_str(), is_selected))
                    indexEmitter_ = it - ptrSystem->getEmiters().begin();
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }

        std::shared_ptr<AParticleModule> *ptrModule = nullptr;
        if (ptrEmitter) {
            if ((*ptrEmitter)->getModules().size() > 0 && indexModule_ < (*ptrEmitter)->getModules().size()) {
                ptrModule = &((*ptrEmitter)->getModules()[indexModule_]);
            }
            if (ImGui::BeginCombo("Modules", (ptrModule ? typeid(**ptrModule).name() : ""), NTL_IMGUI_COMBO_FLAGS)) {
                for (auto it = (*ptrEmitter)->getModules().begin(); it != (*ptrEmitter)->getModules().end(); ++it) {
                    auto &module = *it;
                    bool is_selected = (ptrModule == &module);
                    if (ImGui::Selectable(typeid(*module).name(), is_selected))
                        indexModule_ = it - (*ptrEmitter)->getModules().begin();
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
        }
        if (ptrModule && dynamic_cast<ModuleSPH *>(&(**ptrModule))) {
            ModuleSPH *sph = dynamic_cast<ModuleSPH *>(&(**ptrModule));
            ImGui::DragFloat("pressure: ", &sph->getModuleParam().pressure, 0.1f, 0, 2500.f);
            ImGui::DragFloat("smoothingRadius: ", &sph->getModuleParam().smoothingRadius, 0.05f, 0, 200.f);
            ImGui::DragFloat("viscosity: ", &sph->getModuleParam().viscosity, 0.001f, 0, 1.f);
            ImGui::DragFloat("densityRef: ", &sph->getModuleParam().densityRef, 0.1f, 0, 10.f);
        }
    }
}

void EditorParticleSystems::reload() {
    keySystem_ = "";
    strcpy(inputName_, "Name");
}
