#pragma once

#include "AWidget.hpp"
#include <Gui/EditorCamera.hpp>

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
};
