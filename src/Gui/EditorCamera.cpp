#include "EditorCamera.hpp"
#include <Engine/CameraManager.hpp>
#include <Engine/CameraManager.hpp>
#include <Gui/GuiSettings.hpp>
#include <iostream>

EditorCamera::EditorCamera() :
itemCurrent(nullptr) {

}

void EditorCamera::render() {
    if (!ImGui::CollapsingHeader("Camera"))
        return;
    ImGui::Separator();
    ImGui::Separator();





    std::list<Camera> &cameras = CameraManager::Get().getListCameras();

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
        if (ImGui::DragFloat("Fov", &itemCurrent->fov_, 0.1f)
            || ImGui::DragFloat("Near", &itemCurrent->near_, 0.1f)
            || ImGui::DragFloat("Far", &itemCurrent->far_, 0.4f))
            itemCurrent->updateProjection();
    }
}
