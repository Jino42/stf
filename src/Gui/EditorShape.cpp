#include "EditorShape.hpp"
#include "Engine/Shape/AABB.hpp"
#include "Engine/Shape/ShapeManager.hpp"
#include "Engine/Shape/Sphere.hpp"
#include <Gui/Gui.hpp>
#include <Gui/GuiSettings.hpp>
#include <iostream>

EditorShape::EditorShape()
    : indexShape_(0) {
    reload();
}

void EditorShape::render() {
    float speedDrag = 0.1f;

    if (!ImGui::CollapsingHeader("Shape"))
        return;
    ImGui::Separator();
    ImGui::Separator();

    std::map<std::string, std::shared_ptr<AShape>> &mapShape = ShapeManager::Get().getMapShape();

    std::shared_ptr<AShape> shape;

    if (mapShape.find(keySystem_) != mapShape.end())
        shape = mapShape.find(keySystem_)->second;
    if (ImGui::BeginCombo("Shapes", (shape ? keySystem_.c_str() : ""), NTL_IMGUI_COMBO_FLAGS)) {
        for (auto &[name, shape_t]: mapShape) {
            bool is_selected = (shape == shape_t);
            if (ImGui::Selectable(name.c_str(), is_selected))
                keySystem_ = name;
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    if (!shape)
        return;
    ImGui::SetNextItemOpen(true);
    if (ImGui::TreeNode("Shape")) {
        std::shared_ptr<AABB> aabb = std::dynamic_pointer_cast<AABB>(shape);
        std::shared_ptr<Sphere> sphere = std::dynamic_pointer_cast<Sphere>(shape);
        std::shared_ptr<ADebugObject> debugObject = std::dynamic_pointer_cast<ADebugObject>(shape);


        if (aabb) {
            if (ImGui::DragFloat3("Position", reinterpret_cast<float *>(&aabb->getPosition()), speedDrag))
                aabb->updateLines();
            if (ImGui::DragFloat3("Size", reinterpret_cast<float *>(&aabb->getSize()), speedDrag))
                aabb->updateLines();
        } else if (sphere) {
            if (ImGui::DragFloat3("Position", reinterpret_cast<float *>(&sphere->getPosition()), speedDrag))
                sphere->updateLines();
            if (ImGui::DragFloat("Radius: ", &sphere->getRadius(), 0.05f, 0.f, 50.f))
                sphere->updateLines();
            if (ImGui::DragInt("SectorCount: ", &sphere->getSectorCount(), 1, 1, 128))
                sphere->updateLines();
            if (ImGui::DragInt("StackCount: ", &sphere->getStackCount(), 1, 1, 128))
                sphere->updateLines();
            
        }

        if (debugObject) {
            ImGui::Text("Color : ");
            ImGui::SameLine();
            if (ImGui::ColorEdit4("MyColor##3", (float *)&debugObject->color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel))
                debugObject->updateLines();
        }

        ImGui::TreePop();
    }
}

void EditorShape::reload() {
    keySystem_ = "";
    strcpy(inputName_, "Name");
}
