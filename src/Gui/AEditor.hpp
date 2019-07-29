#pragma once

#include "imgui.h"

class AEditor {
public:
    virtual ~AEditor() = default;
    virtual void render() = 0;

private:
};
