#include "JsonParticleParser.hpp"
#include "Particle/ParticleSystemManager.hpp"
#include "cl_type.hpp"

#include "Particle/PaticleEmitter/ParticleEmitterMesh.hpp"
#include "Particle/PaticleEmitter/ParticleEmitterPoint.hpp"
#include "Particle/PaticleEmitter/ParticleEmitterSPH.hpp"
#include "Particle/PaticleEmitter/ParticleEmitterSprite.hpp"

#include "Particle/ParticleModule/ModuleAttractor.hpp"
#include "Particle/ParticleModule/ModuleColor.hpp"
#include "Particle/ParticleModule/ModuleMeshParticulizer.hpp"
#include "Particle/ParticleModule/ModuleMoveToTarget.hpp"
#include "Particle/ParticleModule/ModuleMovement.hpp"
#include "Particle/ParticleModule/ModulePhysicConstrainInShape.hpp"
#include "Particle/ParticleModule/ModulePhysicGravity.hpp"
#include "Particle/ParticleModule/ModulePhysicNoise.hpp"
#include "Particle/ParticleModule/ModuleResolvePhysic.hpp"
#include "Particle/ParticleModule/ModuleSPH.hpp"
#include "Particle/ParticleModule/ModuleSizeOverLifetime.hpp"
#include "Particle/ParticleModule/ParticleSpawnModule.hpp"

JsonParticleParser::JsonParticleParser(boost::filesystem::path path)
    : jsonPath_(path),
      currentSystem_(nullptr),
      currentEmitter_(nullptr) {
}

void JsonParticleParser::parse() {
    std::cout << "begin parse" << std::endl;
    std::cout << jsonPath_ << std::endl;
    json_ = getJsonFromFileAbsolutePath(jsonPath_.generic_string());
    std::cout << json_ << std::endl;
    for (auto &itSystem: json_["Systems"]) {
        std::cout << itSystem["name"] << std::endl;
        std::cout << itSystem["options"] << std::endl;

        std::cout << itSystem["emitters"] << std::endl;
        ParticleSystemManager::Get().addParticleSystem(itSystem["name"].get<std::string>());
        currentSystem_ = &ParticleSystemManager::Get().getParticleSystem(itSystem["name"].get<std::string>());

        if (itSystem.find("options") != itSystem.end()) {
            std::cout << "There are options : " << std::endl;
            if (itSystem["options"].find("position") != itSystem["options"].end()) {
                json &position = *itSystem["options"].find("position");
                currentSystem_->setPosition(jsonToVec3(position));
            }
            if (itSystem.find("emitters") != itSystem.end()) {
                for (auto &itEmitter: *itSystem.find("emitters")) {
                    if (itEmitter.find("active") != itEmitter.end() && !itEmitter["active"].get<bool>())
                        continue;
                    std::cout << itEmitter["type"] << std::endl;

                    if (itEmitter["type"].get<std::string>() == "Sprite") {
                        auto name = itEmitter["name"].get<std::string>();
                        currentSystem_->addEmitter<ParticleEmitterSprite>(name, itEmitter["particlesMax"].get<int>());
                        currentEmitter_ = &currentSystem_->getEmitter<ParticleEmitterSprite>(name);
                    } else if (itEmitter["type"].get<std::string>() == "Point") {
                        auto name = itEmitter["name"].get<std::string>();
                        currentSystem_->addEmitter<ParticleEmitterPoint>(name, itEmitter["particlesMax"].get<int>());
                        currentEmitter_ = &currentSystem_->getEmitter<ParticleEmitterPoint>(name);
                    } else if (itEmitter["type"].get<std::string>() == "Mesh") {
                        auto name = itEmitter["name"].get<std::string>();
                        currentSystem_->addEmitter<ParticleEmitterMesh>(name, itEmitter["particlesMax"].get<int>());
                        currentEmitter_ = &currentSystem_->getEmitter<ParticleEmitterMesh>(name);
                    } else if (itEmitter["type"].get<std::string>() == "SPH") {
                        auto name = itEmitter["name"].get<std::string>();
                        currentSystem_->addEmitter<ParticleEmitterSPH>(name, itEmitter["particlesMax"].get<int>());
                        currentEmitter_ = &currentSystem_->getEmitter<ParticleEmitterSPH>(name);
                    } else {
                        currentEmitter_ = nullptr;
                    }

                    if (currentEmitter_ && itEmitter["options"].find("position") != itEmitter["options"].end()) {
                        json &position = *itEmitter["options"].find("position");
                        currentEmitter_->getPosition() = jsonToVec3(position);
                    }

                    if (currentEmitter_ && itEmitter.find("modules") != itEmitter.end()) {
                        for (auto &itModule: *itEmitter.find("modules")) {
                            if (itModule.find("active") != itModule.end() && !itModule["active"].get<bool>())
                                continue;

                            std::cout << itModule["type"] << std::endl;
                            if (itModule["type"].get<std::string>() == "Spawn") {
                                currentEmitter_->addModule<ParticleSpawnModule>();
                                currentEmitter_->getModule<ParticleSpawnModule>()->jsonParse(itModule);
                            } else if (itModule["type"].get<std::string>() == "Physic") {
                                currentEmitter_->addModule<ModuleResolvePhysic>();
                                currentEmitter_->getModule<ModuleResolvePhysic>()->jsonParse(itModule);
                            } else if (itModule["type"].get<std::string>() == "Movement") {
                                currentEmitter_->addModule<ModuleMovement>();
                                currentEmitter_->getModule<ModuleMovement>()->jsonParse(itModule);
                            } else if (itModule["type"].get<std::string>() == "Target") {
                                currentEmitter_->addModule<ModuleTarget>();
                                currentEmitter_->getModule<ModuleTarget>()->jsonParse(itModule);
                            } else if (itModule["type"].get<std::string>() == "MeshParticulizer") {
                                currentEmitter_->addModule<ModuleMeshParticulizer>();
                                currentEmitter_->getModule<ModuleMeshParticulizer>()->jsonParse(itModule);
                            } else if (itModule["type"].get<std::string>() == "MoveToTarget") {
                                currentEmitter_->addModule<ModuleMoveToTarget>();
                                currentEmitter_->getModule<ModuleMoveToTarget>()->jsonParse(itModule);
                            } else if (itModule["type"].get<std::string>() == "SizeOverLifetime") {
                                currentEmitter_->addModule<ModuleSizeOverLifetime>();
                                currentEmitter_->getModule<ModuleSizeOverLifetime>()->jsonParse(itModule);
                            } else if (itModule["type"].get<std::string>() == "Attractor") {
                                currentEmitter_->addModule<ModuleAttractor>();
                                currentEmitter_->getModule<ModuleAttractor>()->jsonParse(itModule);
                            } else if (itModule["type"].get<std::string>() == "PhysicGravity") {
                                currentEmitter_->addModule<ModulePhysicGravity>();
                                currentEmitter_->getModule<ModulePhysicGravity>()->jsonParse(itModule);
                            } else if (itModule["type"].get<std::string>() == "PhysicNoise") {
                                currentEmitter_->addModule<ModulePhysicNoise>();
                                currentEmitter_->getModule<ModulePhysicNoise>()->jsonParse(itModule);
                            } else if (itModule["type"].get<std::string>() == "Color") {
                                currentEmitter_->addModule<ModuleColor>();
                                currentEmitter_->getModule<ModuleColor>()->jsonParse(itModule);
                            } else if (itModule["type"].get<std::string>() == "PhysicConstrainInShape") {
                                currentEmitter_->addModule<ModulePhysicConstrainInShape>(itModule["options"].find("shape")->get<std::string>());
                                currentEmitter_->getModule<ModulePhysicConstrainInShape>()->jsonParse(itModule);
                            }
                        }
                    }
                }
            }
        }
        currentSystem_ = nullptr;
        currentEmitter_ = nullptr;
    }
    std::cout << "end parse" << std::endl;
}

glm::vec3 JsonParticleParser::jsonToVec3(json &json) {
    glm::vec3 vec;

    vec.x = json["x"];
    vec.y = json["y"];
    vec.z = json["z"];

    return vec;
}
