#include "EditorCamera.hpp"
#include <Engine/CameraManager.hpp>
#include <Engine/CameraManager.hpp>
#include <Gui/GuiSettings.hpp>
#include <iostream>
#include <Gui/Gui.hpp>

EditorCamera::EditorCamera() :
itemCurrent(nullptr) {
}

void EditorCamera::render() {
    if (!ImGui::CollapsingHeader("Camera"))
        return;
    ImGui::Separator();
    ImGui::Separator();


    std::list<Camera> &cameras = CameraManager::Get().getListCameras();

	ImGui::Text("Name: ");
	ImGui::SameLine();
	static char str0[128] = "Hello, world!";
	ImGui::InputText("input text", str0, IM_ARRAYSIZE(str0));
	ImGui::SameLine();
	if (ImGui::Button("Button"))
		std::cout << "Click" << std::endl;

    if (ImGui::BeginCombo("Cameras", (itemCurrent ? itemCurrent->name.c_str() : ""), NTL_IMGUI_COMBO_FLAGS))
    {
        for (Camera &camera : cameras) {
            bool is_selected = (itemCurrent == &camera);
            if (ImGui::Selectable(camera.name.c_str(), is_selected))
                itemCurrent = &camera;
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    if (itemCurrent) {
		bool boolGuiDebugCamera = itemCurrent->getDebugFrustum();
        //Button Delete Camera
        //Checkbox Debug
		if (ImGui::Checkbox("checkbox", &boolGuiDebugCamera)) {
			itemCurrent->setDebugFrustum(boolGuiDebugCamera);
		}
		//if isdebug
		//Color Debug

		if (ImGui::DragFloat("Fov", &itemCurrent->fov_, 0.1f)
            || ImGui::DragFloat("Near", &itemCurrent->near_, 0.1f)
            || ImGui::DragFloat("Far", &itemCurrent->far_, 0.4f))
            itemCurrent->updateProjection();
        if (ImGui::DragFloat3("Position", reinterpret_cast<float *>(&itemCurrent->position_), 0.05f))
            itemCurrent->updateView();
    }
}
