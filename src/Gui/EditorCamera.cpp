#include "EditorCamera.hpp"
#include <Engine/CameraManager.hpp>
#include <Gui/GuiSettings.hpp>
#include <iostream>
#include <Gui/Gui.hpp>

EditorCamera::EditorCamera() :
itemCurrent(nullptr),
inputCameraName_("Name") {
}

void EditorCamera::render() {
    if (!ImGui::CollapsingHeader("Camera"))
        return;
    ImGui::Separator();
    ImGui::Separator();

    std::list<Camera> &cameras = CameraManager::Get().getListCameras();

	ImGui::Text("Name: ");
	ImGui::SameLine();
	ImGui::InputText("Camera :", inputCameraName_, IM_ARRAYSIZE(inputCameraName_));
	ImGui::SameLine();
	if (ImGui::Button("Add"))
        CameraManager::Get().addCamera(inputCameraName_);

    if (ImGui::BeginCombo("Cameras", (itemCurrent ? itemCurrent->getName().c_str() : ""), NTL_IMGUI_COMBO_FLAGS))
    {
        for (Camera &camera : cameras) {
            bool is_selected = (itemCurrent == &camera);
            if (ImGui::Selectable(camera.getName().c_str(), is_selected))
                itemCurrent = &camera;
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    if (itemCurrent) {
		bool boolGuiDebugCamera = itemCurrent->getFrustum().isDebug();
        if (ImGui::Button("Delete Camera")) {
                    CameraManager::Get().removeCamera(itemCurrent->getName());
            itemCurrent = nullptr;
            return ;
        }
		if (ImGui::Checkbox("checkbox", &boolGuiDebugCamera)) {
			itemCurrent->getFrustum().setDebug(boolGuiDebugCamera);
		}
		if (itemCurrent->getFrustum().isDebug()) {
            ImGui::Text("Color Debug :");
            ImGui::SameLine();
            ImGui::ColorEdit4("Color Frustum", (float*)&itemCurrent->frustum_.color, NTL_IMGUI_COLOR_PICKER_FLAGS);
		}

		if (ImGui::DragFloat("Fov", &itemCurrent->fov_, 0.1f)
            || ImGui::DragFloat("Near", &itemCurrent->near_, 0.1f)
            || ImGui::DragFloat("Far", &itemCurrent->far_, 0.4f))
            itemCurrent->updateProjection();
        if (ImGui::DragFloat3("Position", reinterpret_cast<float *>(&itemCurrent->position_), 0.05f))
            itemCurrent->updateView();
    }
}
