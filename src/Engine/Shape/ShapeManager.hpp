#pragma once

#include "AShape.hpp"
#include <map>
#include <memory>

class ShapeManager {
  public:
    ShapeManager() = default;
    ShapeManager(ShapeManager const &) = delete;
    ~ShapeManager() = default;
    static ShapeManager &Get();

    template <typename T>
    void addShape(std::string const &name) {
        if (mapShapes_.find(name) == mapShapes_.end())
            mapShapes_.try_emplace(name, std::make_shared<T>());
    }

    std::shared_ptr<AShape> getShape(std::string const &name);

    std::map<std::string, std::shared_ptr<AShape>> &getMapShape();

  private:
    std::map<std::string, std::shared_ptr<AShape>> mapShapes_;

    static std::unique_ptr<ShapeManager> instance_;
};
