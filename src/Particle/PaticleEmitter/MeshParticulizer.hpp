#pragma once
#include "AParticleEmitter.hpp"
#include "Engine/ModelEngine/ActorModel.hpp"

struct PointVertexData {
    glm::vec3   position[3];
    glm::vec3   normal[3];
    glm::vec2   texCoords[3];
};

class MeshParticulizer : public AParticleEmitter {
public:
    MeshParticulizer(ParticleSystem &system, ClQueue &queue, std::string const &name, Mesh &mesh);

    void init() override;
    void render() override;
    void update(float deltaTime) override;
    void reload() override;

private:
    unsigned int	VAO;
    Mesh &mesh_;
    DeviceBuffer deviceBufferVertexBasic_;
};
