#pragma once

#include "Engine/ModelEngine/ActorModel.hpp"
#include "Engine/Graphic/Material.hpp"
#include "Engine/Graphic/Light.hpp"
#include "Engine/Shader.hpp"
#include "Model.hpp"
#include "Mesh.hpp"

class MainGraphicExtendModel {
public:
    MainGraphicExtendModel();

    void update(float deltaTime);

    glm::vec3						attractorPoint;
    float	    					distanceAttractor = 2.0f;
    bool                            updateAttractor = true;

    static MainGraphicExtendModel &Get();

private:
    Shader							shader_;
    Model							block_;
    ActorModel						actor_;
    Light							light_;
    std::vector< Material >		vectorMap_;
    glm::mat4						model_;

    void constructMaterialMap_();
    static std::unique_ptr<MainGraphicExtendModel> instance_;
};
