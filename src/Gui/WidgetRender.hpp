#pragma once

#include "AWidget.hpp"
#include <memory>

class WidgetRender : public AWidget {
public:
    WidgetRender();
    ~WidgetRender() override = default;
    WidgetRender &operator=(const WidgetRender &) = delete;
    WidgetRender(const WidgetRender &) = delete;

    static WidgetRender &Get();

    ImVec2 widgetPosition;
    ImVec2 widgetSize;

private:
    void beginContent_() override;

    static std::unique_ptr<WidgetRender> instance_;
};
