#pragma once

#include "imgui.h"

class AEditor {
public:
    virtual ~AEditor() = default;
    virtual void render() = 0;
    virtual void reload(){};
  private:
};
