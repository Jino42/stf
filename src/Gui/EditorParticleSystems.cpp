#include "EditorParticleSystems.hpp"

#include "Particle/ParticleModule/ModuleSPH.hpp"
#include "Particle/PaticleEmitter/ParticleEmitterSPH.hpp"

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

    ImGui::SetNextItemOpen(true);
    if (ImGui::TreeNode("System")) {

        if (ptrSystem) {
            if (ImGui::DragFloat3("Position", reinterpret_cast<float *>(&ptrSystem->getPosition()), 0.05f)) {
                ; // SEF
            }

            if (ptrSystem->getEmiters().size() > 0 && indexEmitter_ < ptrSystem->getEmiters().size()) {
                std::unique_ptr<AParticleEmitter> &ptrEmitter = ptrSystem->getEmiters()[indexEmitter_];

                if (ImGui::BeginCombo("Emitters", (ptrEmitter ? ptrEmitter->getName().c_str() : ""), NTL_IMGUI_COMBO_FLAGS)) {
                    for (auto it = ptrSystem->getEmiters().begin(); it != ptrSystem->getEmiters().end(); ++it) {
                        auto &emitter = *it;
                        bool is_selected = (ptrEmitter == emitter);
                        if (ImGui::Selectable(emitter->getName().c_str(), is_selected))
                            indexEmitter_ = it - ptrSystem->getEmiters().begin();
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }

                    ImGui::EndCombo();
                }

                ImGui::SetNextItemOpen(true);
                if (ImGui::TreeNode("Emitter")) {
                    std::shared_ptr<AParticleModule> ptrModule;
                    if (ptrEmitter) {
                        if (ptrEmitter->getModules().size() > 0 && indexModule_ < ptrEmitter->getModules().size()) {
                            ptrModule = ptrEmitter->getModules()[indexModule_];
                        }
                        if (ptrEmitter && dynamic_cast<ParticleEmitterSPH *>(&(*ptrEmitter))) {
                            ParticleEmitterSPH *emitterSph = dynamic_cast<ParticleEmitterSPH *>(&(*ptrEmitter));
                            ImGui::DragFloat("Line Width", &emitterSph->getLineWidth(), 0.05f, 0.f, 5.f);
                        }
                        if (ImGui::BeginCombo("Modules", (ptrModule ? typeid(*ptrModule).name() : ""), NTL_IMGUI_COMBO_FLAGS)) {
                            for (auto it = ptrEmitter->getModules().begin(); it != ptrEmitter->getModules().end(); ++it) {
                                auto &module = *it;
                                bool is_selected = (ptrModule == module);
                                if (ImGui::Selectable(typeid(*module).name(), is_selected))
                                    indexModule_ = it - ptrEmitter->getModules().begin();
                                if (is_selected)
                                    ImGui::SetItemDefaultFocus();
                            }
                            ImGui::EndCombo();
                        }
                    }
                    ImGui::SetNextItemOpen(true);
                    if (ImGui::TreeNode("Module")) {
                        ImGui::TreePop();
                        if (ptrModule && dynamic_cast<ModuleSPH *>(&(*ptrModule))) {
                            ModuleSPH *sph = dynamic_cast<ModuleSPH *>(&(*ptrModule));
                            ImGui::DragFloat("Pressure", &sph->getModuleParam().pressure, 0.1f, 0, 2500.f);
                            ImGui::DragFloat("SmoothingRadius: ", &sph->getModuleParam().smoothingRadius, 0.05f, 0, 200.f);
                            ImGui::DragFloat("Viscosity: ", &sph->getModuleParam().viscosity, 0.001f, 0, 1.f);
                            ImGui::DragFloat("DensityRef: ", &sph->getModuleParam().densityRef, 0.1f, 0, 10.f);

                            Gui::beginColor(eColor::kRed, eColor::kMedium);
                            if (ImGui::Button("D_Velocity")) {
                                sph->shiftDebugVelocity();
                            }
                            Gui::endColor();
                            ImGui::SameLine();
                            Gui::beginColor(eColor::kPurple, eColor::kMedium);
                            if (ImGui::Button("D_Viscosity")) {
                                sph->shiftDebugViscosity();
                            }
                            Gui::endColor();
                            ImGui::SameLine();
                            Gui::beginColor(eColor::kGreen, eColor::kMedium);
                            if (ImGui::Button("D_Pressure")) {
                                sph->shiftDebugPressure();
                            }
                            Gui::endColor();


                            float *arr = new float[ptrEmitter->getQueue().getEventMap().size()];
                            unsigned int i = 0;
                            float max = 10.0f;
                            ClError err;
                            for (auto &pair: ptrEmitter->getQueue().getEventMap()) {
                                cl::Event &ev = ptrEmitter->getQueue().getEvent(pair.first);

                                cl_ulong start = 0, end = 0;
                                cl_double time = 0;
                                err.err = ev.getProfilingInfo<cl_ulong>(CL_PROFILING_COMMAND_START, &start);
                                err.err |= ev.getProfilingInfo<cl_ulong>(CL_PROFILING_COMMAND_END, &end);
                                err.clCheckError();

                                time = (cl_double)(end - start) * (cl_double)(1e-06);
                                if (time > max) {
                                    max = time;
                                }
                                arr[i] = (float)time;
                                std::cout << arr[i] << " | " << IM_ARRAYSIZE(arr) << std::endl;
                                i++;
                            }
                            ImGui::PlotHistogram("Histogram", arr, ptrEmitter->getQueue().getEventMap().size(),
                                                 0, NULL, 0.0f, max, ImVec2(0, 80));
                        }
                    }
                    ImGui::TreePop();
                }
            }
        }

        ImGui::TreePop();
    }
}

void EditorParticleSystems::reload() {
    keySystem_ = "";
    strcpy(inputName_, "Name");
}
