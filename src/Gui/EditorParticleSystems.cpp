#include "EditorParticleSystems.hpp"
#include <Particle/ParticleSystemManager.hpp>
#include <Gui/GuiSettings.hpp>
#include <iostream>
#include <Gui/Gui.hpp>

EditorParticleSystems::EditorParticleSystems() :
		itemCurrent(nullptr),
		inputName_("Name") {
}

void EditorParticleSystems::render() {
	if (!ImGui::CollapsingHeader("ParticleSystem"))
		return;
	ImGui::Separator();
	ImGui::Separator();

	std::map<std::string, ParticleSystem> &mapSystem = ParticleSystemManager::Get().getMapParticleSystem();

	ImGui::Text("Name: ");
	ImGui::SameLine();
	ImGui::InputText("Camera :", inputName_, IM_ARRAYSIZE(inputName_));
	ImGui::SameLine();
//	if (ImGui::Button("Add"))
//		CameraManager::Get().addCamera(inputCameraName_);

	if (ImGui::BeginCombo("Systems", (itemCurrent ? itemCurrent->getName().c_str() : ""), NTL_IMGUI_COMBO_FLAGS))
	{
		for (auto& [name, system] : mapSystem) {
			bool is_selected = (itemCurrent == &system);
			if (ImGui::Selectable(system.getName().c_str(), is_selected))
				itemCurrent = &system;
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	if (itemCurrent) {
		if (ImGui::DragFloat3("Position", reinterpret_cast<float *>(&itemCurrent->getPosition()), 0.05f))
			;
	}
}
