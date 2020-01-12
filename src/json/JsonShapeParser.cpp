#include "JsonShapeParser.hpp"
#include "Engine/Shape/ShapeManager.hpp"
#include "cl_type.hpp"

#include "Engine/Shape/AABB.hpp"
#include "Engine/Shape/Sphere.hpp"

JsonShapeParser::JsonShapeParser(boost::filesystem::path path)
    : jsonPath_(path),
      currentShape_(nullptr) {
}

void JsonShapeParser::parse() {
    std::cout << "begin JsonShapeParser" << std::endl;
    std::cout << jsonPath_ << std::endl;
    json_ = getJsonFromFileAbsolutePath(jsonPath_.generic_string());
    std::cout << json_ << std::endl;
    for (auto &itShape: json_["Shapes"]) {

        std::cout << "Is " << itShape["type"].get<std::string>() << std::endl;

        if (itShape["type"].get<std::string>() == "Sphere") {
            ShapeManager::Get().addShape<Sphere>(itShape["name"].get<std::string>());
            std::shared_ptr<Sphere> sphere = std::dynamic_pointer_cast<Sphere>(ShapeManager::Get().getShape(itShape["name"].get<std::string>()));

            sphere->setRadius(itShape["options"].find("radius")->get<float>());
        } else if (itShape["type"].get<std::string>() == "AABB") {
            ShapeManager::Get().addShape<AABB>(itShape["name"].get<std::string>());
            std::shared_ptr<AABB> aabb = std::dynamic_pointer_cast<AABB>(ShapeManager::Get().getShape(itShape["name"].get<std::string>()));

            aabb->setSize(jsonToVec3(*itShape["options"].find("size")));
        }

        std::shared_ptr<ADebugObject> debugObject = std::dynamic_pointer_cast<ADebugObject>(ShapeManager::Get().getShape(itShape["name"].get<std::string>()));
        if (itShape["render"].get<bool>())
            debugObject->setDebug(true);

        currentShape_ = ShapeManager::Get().getShape(itShape["name"].get<std::string>());
        currentShape_->setPosition(jsonToVec3(*itShape["options"].find("position")));
        currentShape_->setRender(itShape["render"].get<bool>());

        currentShape_ = nullptr;
    }
    std::cout << "end JsonShapeParser" << std::endl;
}