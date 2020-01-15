#pragma once

#include <Gui/AEditor.hpp>
#include <string>
#include "Engine/Shape/AShape.hpp"

class EditorShape : public AEditor {
  public:
    EditorShape();
    void render() override;
    void reload() override;

  private:
    std::string keySystem_;
    size_t indexShape_;
    char inputName_[36];
};
