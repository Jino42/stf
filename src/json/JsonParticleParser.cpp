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
#include "Particle/ParticleModule/ModulePhysicGravity.hpp"
#include "Particle/ParticleModule/ModulePhysicNoise.hpp"
#include "Particle/ParticleModule/ModuleResolvePhysic.hpp"
#include "Particle/ParticleModule/ModuleSPH.hpp"
#include "Particle/ParticleModule/ModuleSizeOverLifetime.hpp"

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
                std::cout << position << std::endl;
                currentSystem_->setPosition(jsonToVec3(position));
            }
            if (itSystem.find("emitters") != itSystem.end()) {
                for (auto &itEmitter: *itSystem.find("emitters")) {
                    if (itEmitter["active"].get<bool>() == false)
                        continue;
                    std::cout << itEmitter["type"] << std::endl;

                    if (itEmitter["type"].get<std::string>() == "Sprite") {
                        auto name = itEmitter["name"].get<std::string>();
                        currentSystem_->addEmitter<ParticleEmitterSprite>(name, itEmitter["particlesSpawn"].get<int>(),
                                                                          itEmitter["particlesMax"].get<int>());
                        currentEmitter_ = &currentSystem_->getEmitter<ParticleEmitterSprite>(name);

                        std::cout << "Is [Sprite]" << std::endl;
                    } else if (itEmitter["type"].get<std::string>() == "Point") {
                        auto name = itEmitter["name"].get<std::string>();
                        currentSystem_->addEmitter<ParticleEmitterPoint>(name, itEmitter["particles"].get<int>());
                        currentEmitter_ = &currentSystem_->getEmitter<ParticleEmitterPoint>(name);

                        std::cout << "Is [Point]" << std::endl;
                    } else if (itEmitter["type"].get<std::string>() == "Mesh") {
                        auto name = itEmitter["name"].get<std::string>();
                        currentSystem_->addEmitter<ParticleEmitterMesh>(name, itEmitter["particlesSpawn"].get<int>(),
                                                                        itEmitter["particlesMax"].get<int>());
                        currentEmitter_ = &currentSystem_->getEmitter<ParticleEmitterMesh>(name);

                        std::cout << "Is [Mesh]" << std::endl;
                    } else if (itEmitter["type"].get<std::string>() == "SPH") {
                        auto name = itEmitter["name"].get<std::string>();
                        currentSystem_->addEmitter<ParticleEmitterSPH>(name, itEmitter["particles"].get<int>());
                        currentEmitter_ = &currentSystem_->getEmitter<ParticleEmitterSPH>(name);

                        std::cout << "Is [SPH]" << std::endl;
                    } else {
                        currentEmitter_ = nullptr;
                    }

                    if (currentEmitter_ && itEmitter["options"].find("position") != itEmitter["options"].end()) {
                        json &position = *itEmitter["options"].find("position");
                        std::cout << "DJA DJA " << position << std::endl;
                        currentEmitter_->getPosition() = jsonToVec3(position);
                    }

                    if (currentEmitter_ && itEmitter.find("modules") != itEmitter.end()) {
                        for (auto &itModule: *itEmitter.find("modules")) {
                            std::cout << itModule["type"] << std::endl;
                            if (itModule["type"].get<std::string>() == "Physic") {
                                currentEmitter_->addModule<ModuleResolvePhysic>();
                            } else if (itModule["type"].get<std::string>() == "Movement") {
                                currentEmitter_->addModule<ModuleMovement>();
                            } else if (itModule["type"].get<std::string>() == "Target") {
                                currentEmitter_->addModule<ModuleTarget>();
                            } else if (itModule["type"].get<std::string>() == "MeshParticulizer") {
                                currentEmitter_->addModule<ModuleMeshParticulizer>();
                            } else if (itModule["type"].get<std::string>() == "MoveToTarget") {
                                currentEmitter_->addModule<ModuleMoveToTarget>();
                            } else if (itModule["type"].get<std::string>() == "SizeOverLifetime") {
                                currentEmitter_->addModule<ModuleSizeOverLifetime>();
                            } else if (itModule["type"].get<std::string>() == "Attractor") {
                                currentEmitter_->addModule<ModuleAttractor>();
                            } else if (itModule["type"].get<std::string>() == "Gravity") {
                                currentEmitter_->addModule<ModulePhysicGravity>();

                                if (itModule.find("options") != itModule.end()) {
                                    std::shared_ptr<ModulePhysicGravity> &currentModule = currentEmitter_->getModule<ModulePhysicGravity>();
                                    std::cout << "There are options : " << std::endl;
                                    if (itModule["options"].find("gravity") != itModule["options"].end()) {
                                        std::cout << "Gravity" << std::endl;
                                        json &gravity = *itModule["options"].find("gravity");
                                        std::cout << gravity << std::endl;
                                        currentModule->getCpuModuleParam().gravity = jsonToFloat3(gravity);
                                    }
                                }

                            } else if (itModule["type"].get<std::string>() == "PhysicNoise") {
                                currentEmitter_->addModule<ModulePhysicNoise>();
                            } else if (itModule["type"].get<std::string>() == "Color") {
                                currentEmitter_->addModule<ModuleColor>();
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
