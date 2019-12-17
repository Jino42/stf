#pragma once

#include <ANommable.hpp>
#include <string>

class IGuiEntity : public ANommable {
public:
    IGuiEntity() = delete;
    IGuiEntity(std::string const &name) :
            ANommable(name),
            isSelected_(false) {
    }
    virtual ~IGuiEntity() = default;

private:
    bool isSelected_;
};
