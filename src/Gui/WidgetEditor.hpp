#pragma once

#include "AWidget.hpp"

class WidgetEditor : public AWidget {
public:
    WidgetEditor();
    ~WidgetEditor() override = default;
    WidgetEditor &operator=(const WidgetEditor &) = delete;
    WidgetEditor(const WidgetEditor &) = delete;

private:
    void beginContent_() override;
};
