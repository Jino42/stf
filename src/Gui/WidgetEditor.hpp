#pragma once

#include "AWidget.hpp"
#include <Gui/EditorCamera.hpp>
#include <Gui/EditorParticleSystems.hpp>
#include <Gui/EditorShape.hpp>

class EditorCamera;

class WidgetEditor : public AWidget {
public:
    WidgetEditor();
    ~WidgetEditor() override = default;
    WidgetEditor &operator=(const WidgetEditor &) = delete;
    WidgetEditor(const WidgetEditor &) = delete;

private:
    void beginContent_() override;

    EditorCamera    editorCamera_;
    EditorParticleSystems editorParticleSystems_;
    EditorShape editorShape_;
};
