#pragma once

#include "AWidget.hpp"

class WidgetOption : public AWidget {
public:
    WidgetOption();
    ~WidgetOption() override = default;
    WidgetOption &operator=(const WidgetOption &) = delete;
    WidgetOption(const WidgetOption &) = delete;

private:

    char nameBuffer_[1024];
    bool sound_;
    bool rNoise_;
    bool rMusique_;

    void beginContent_() override;
};