#include "ModuleAttractor.hpp"
#include "Cl/ClKernel.hpp"
#include "Cl/ClProgram.hpp"
#include "Engine/Shape/ShapeManager.hpp"
#include "ModuleResolvePhysic.hpp"
#include "OpenCGL_Tools.hpp"
#include "Particle/PaticleEmitter/AParticleEmitter.hpp"
#include "cl_type.hpp"
#include <Engine/Display/DisplayWindow.hpp>
#include <Engine/ModelEngine/MainGraphicExtendModel.hpp>
#include <Gui/Gui.hpp>
#include <iostream>

ModuleAttractor::ModuleAttractor(AParticleEmitter &emitter)
    : AParticleModule(emitter),
      doAttractor_(true),
      mouse_(false) {
    ClProgram::Get().addProgram(pathKernel_ / "Attractor.cl");

    kernelUpdate_.setKernel(emitter_, "attractor");
    kernelUpdate_.setArgsGPUBuffers(eParticleBuffer::kData);
}

void ModuleAttractor::addNameShapeAttractor(std::string name) {
    namesShapeAttractor_.emplace_back(name);
}

void ModuleAttractor::updateAttractor_() {
    ClError err;

    gpuBufferLocal_ShapeAttractor_ = cl::Buffer(ClContext::Get().context, CL_MEM_WRITE_ONLY, sizeof(cl_Shape) * namesShapeAttractor_.size() + 1);

    for (unsigned int i = 0; i < namesShapeAttractor_.size(); i++) {
        std::shared_ptr<AShape> shape = std::dynamic_pointer_cast<AShape>(ShapeManager::Get().getShape(namesShapeAttractor_[i]));
        cl_Shape *clShape = shape->getCl_Shape();
        err.err = queue_.getQueue().enqueueWriteBuffer(gpuBufferLocal_ShapeAttractor_, GL_TRUE,
                                                       sizeof(cl_Shape) * i, sizeof(cl_Shape),
                                                       clShape);
        err.clCheckError();
        queue_.getQueue().finish();
        delete clShape;
    }
}

void ModuleAttractor::init() {
    if (debug_)
        printf("%s\n", __FUNCTION_NAME__);
    updateAttractor_();
    gpuBufferParticles_Physic_ = emitter_.getClBuffer<ParticleDataPhysic>();
}

void ModuleAttractor::update(float deltaTime) {
    if (!isActive_)
        return;
    if (debug_)
        printf("%s\n", __FUNCTION_NAME__);

    updateAttractor_();

    glm::vec3 attractorPosition = MainGraphicExtendModel::Get().attractorPoint;
    unsigned int flag = mouse_ ? 1 : 0;

    unsigned int nbShape = namesShapeAttractor_.size();
    kernelUpdate_.beginAndSetUpdatedArgs(
        *gpuBufferParticles_Physic_,
        flag,
        glmVec3toClFloat3(attractorPosition),
        gpuBufferLocal_ShapeAttractor_,
        nbShape,
        deltaTime);

    OpenCGL::RunKernelWithMem(queue_.getQueue(), kernelUpdate_, emitter_.getParticleOCGL_BufferData().mem, cl::NullRange, cl::NDRange(nbParticleMax_));
}

void ModuleAttractor::reload() {
    if (debug_)
        printf("%s\n", __FUNCTION_NAME__);
    init();
}

glm::vec3 ModuleAttractor::getCenterOfAllAttractors() const {
    if (!namesShapeAttractor_.size())
        return glm::vec3(0.0f);
    glm::vec3 finalPosition = glm::vec3(0.f);
    glm::vec3 position = std::dynamic_pointer_cast<AShape>(ShapeManager::Get().getShape(namesShapeAttractor_[0]))->getPosition();
    for (unsigned int i = 1; i < namesShapeAttractor_.size(); i++) {
        std::shared_ptr<AShape> shape = std::dynamic_pointer_cast<AShape>(ShapeManager::Get().getShape(namesShapeAttractor_[i]));

        glm::vec3 diff = shape->getPosition() - position;

        finalPosition += diff;
    }
    finalPosition /= namesShapeAttractor_.size();
    finalPosition += position; 
    return finalPosition;
}

void ModuleAttractor::jsonParse(json &itModule) {
    if (itModule.find("options") != itModule.end()) {
        if (itModule["options"].find("shape") != itModule["options"].end()) {
            std::cout << itModule["options"]["shape"] << std::endl;
            for (auto &shape: itModule["options"]["shape"]) {
                addNameShapeAttractor(shape.get<std::string>());
            }
        }
        if (itModule["options"].find("mouse") != itModule["options"].end() && itModule["options"].find("mouse")->get<bool>()) {
            mouse_ = true;
        }
    }
}

void ModuleAttractor::gui() {
    AParticleModule::gui();
    ImGui::Checkbox("Attract With Mouse", &mouse_);
}