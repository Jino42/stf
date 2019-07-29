#pragma once

#include <Gui/AEditor.hpp>

class Camera;

class EditorCamera : public  AEditor {
public:
    EditorCamera();
    void render() override;

private:
    Camera *itemCurrent;
};
