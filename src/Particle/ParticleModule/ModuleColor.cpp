#include "ModuleColor.hpp"
#include "Cl/ClKernel.hpp"
#include "Cl/ClProgram.hpp"
#include "OpenCGL_Tools.hpp"
#include "Particle/PaticleEmitter/ParticleEmitterMesh.hpp"
#include "cl_type.hpp"
#include <Engine/ModelEngine/MainGraphicExtendModel.hpp>
#include <iostream>
#include <Gui/Gui.hpp>
#include "ModuleAttractor.hpp"
#include "Engine/Shape/ShapeManager.hpp"


ModuleColor::ModuleColor(AParticleEmitter &emitter)
    : AParticleModule(emitter),
      radius_(200.f),
      startColor_(0.8f, 0.2f, 0.3f, 1.f),
      endColor_(0.3f, 0.2f, 0.8f, 1.f),
      type_(eSourceAttractor::kPosition),
      positionColor_(0.f) {
    ClProgram::Get().addProgram(pathKernel_ / "Color.cl");

    kernelUpdate_.setKernel(emitter_, "color_radius_from_position");
    kernelUpdate_.setArgsGPUBuffers(eParticleBuffer::kData);
    sphere_.getSectorCount() = 12;
    sphere_.getStackCount() = 6;
    sphere_.color = glm::vec4(0.3f, 0.1f, 0.0f, 1.0f);
}

void ModuleColor::init() {
    if (debug_)
        printf("%s\n", __FUNCTION_NAME__);
}

void ModuleColor::update(float deltaTime) {
    if (debug_)
        printf("%s\n", __FUNCTION_NAME__);

    glm::vec3 centerOfRadius = glm::vec3(0.0f);

    if (type_ == eSourceAttractor::kPosition) {
        centerOfRadius = positionColor_;
    } else if (type_ == eSourceAttractor::kAttractorModule) {
        std::shared_ptr<ModuleAttractor> attractor = emitter_.getModule<ModuleAttractor>();
        if (attractor) {
            centerOfRadius = attractor->getCenterOfAllAttractors();
        }
    } else if (type_ == eSourceAttractor::kShape) {
        std::shared_ptr<AShape> shape = std::dynamic_pointer_cast<AShape>(ShapeManager::Get().getShape(nameShape_));
        if (shape) {
            centerOfRadius = shape->getPosition();
        }
    }

    

    kernelUpdate_.beginAndSetUpdatedArgs(glmVec3toClFloat3(centerOfRadius),
                                         radius_,
                                         glmVec4toClFloat4(startColor_),
                                         glmVec4toClFloat4(endColor_),
                                         deltaTime);

    OpenCGL::RunKernelWithMem(queue_.getQueue(), kernelUpdate_, emitter_.getParticleOCGL_BufferData().mem, cl::NullRange, cl::NDRange(nbParticleMax_));

    if (sphere_.getDebug()) {
        sphere_.setPosition(centerOfRadius);
        sphere_.setRadius(radius_);
    }
    
}

void ModuleColor::reload() {
    if (debug_)
        printf("%s\n", __FUNCTION_NAME__);
    init();
}

void ModuleColor::jsonParse(json &itModule) {
    if (itModule.find("options") != itModule.end()) {
        if (itModule["options"].find("radius") != itModule["options"].end()) {
            radius_ = itModule["options"]["radius"].get<float>();
        }
        if (itModule["options"].find("debugRadius") != itModule["options"].end() && itModule["options"].find("debugRadius")->get<bool>()) {
            sphere_.setDebug(true);
        }

        if (itModule["options"].find("shape") != itModule["options"].end()) {
            nameShape_ = (itModule["options"]["shape"].get<std::string>());
            type_ = eSourceAttractor::kShape;
        }
        if (itModule["options"].find("position") != itModule["options"].end()) {
            positionColor_ = jsonToVec3(itModule["options"]["position"]);
            type_ = eSourceAttractor::kPosition;
        }
        if (itModule["options"].find("attractor") != itModule["options"].end() && itModule["options"].find("attractor")->get<bool>()) {
            type_ = eSourceAttractor::kAttractorModule;
        }
    }
}


void ModuleColor::gui() {

    ImGui::DragFloat("Radius ", &radius_, 0.5f, 0.f, 1000.f);
    if (ImGui::Button("DebugRadius"))
        sphere_.setDebug(!sphere_.getDebug());

    ImGui::Text("ColorStart ");
    ImGui::SameLine();
    ImGui::ColorEdit4("ColorStart", (float *)&startColor_, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
    ImGui::Text("ColorEnd ");
    ImGui::SameLine();
    ImGui::ColorEdit4("ColorEnd", (float *)&endColor_, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
}