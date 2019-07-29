#pragma once

#include <string>

class INommable {
public:
    class NameAlreadyTaken : public std::invalid_argument {
    public:
        NameAlreadyTaken() noexcept = delete;
        explicit NameAlreadyTaken(std::string const &s) noexcept : invalid_argument(s) {};
        ~NameAlreadyTaken() noexcept override = default;
        NameAlreadyTaken(NameAlreadyTaken const &src) noexcept = delete;
        NameAlreadyTaken &operator=(NameAlreadyTaken const &rhs) noexcept = delete;
    };

    explicit INommable(std::string const &name) :
            name(name) {
    }
    INommable() = delete;
    virtual ~INommable() = default;

    std::string name;
};
