#pragma once

#include "AWidget.hpp"
#include <memory>

class WidgetRayMarch : public AWidget {
  public:
    WidgetRayMarch();
    ~WidgetRayMarch() override = default;
    WidgetRayMarch &operator=(const WidgetRayMarch &) = delete;
    WidgetRayMarch(const WidgetRayMarch &) = delete;

    static WidgetRayMarch &Get();

    ImVec2 widgetPosition;
    ImVec2 widgetSize;

  private:
    void beginContent_() override;

    static std::unique_ptr<WidgetRayMarch> instance_;
};
