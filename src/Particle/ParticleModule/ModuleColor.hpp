#pragma once

#include "AParticleModule.hpp"
#include "Cl/ClQueue.hpp"
#include "OCGL_Buffer.hpp"
#include "Engine/Shape/Sphere.hpp"

enum class eSourceAttractor {
    kAttractorModule,
    kShape,
    kPosition
};


class ModuleColor : public AParticleModule {
  public:
    ModuleColor(AParticleEmitter &emitter);
    void init() override;
    void update(float deltaTime) override;
    void reload() override;

    void jsonParse(json &itModule) override;
    void gui() override;

  private:
    Sphere sphere_;

    float radius_;
    glm::vec4 startColor_;
    glm::vec4 endColor_;

    eSourceAttractor type_;
    std::string nameShape_;
    glm::vec3 positionColor_;
};
