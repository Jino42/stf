#pragma once

#include <INommable.hpp>
#include <string>

class IGuiEntity : public INommable {
public:
    IGuiEntity() = delete;
    IGuiEntity(std::string const &name) :
            INommable(name),
            isSelected_(false) {
    }
    virtual ~IGuiEntity() = default;

private:
    bool isSelected_;
};
