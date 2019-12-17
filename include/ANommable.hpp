#pragma once

#include <string>
#include <stdexcept>

class ANommable {
  public:
    class NameAlreadyTaken : public std::invalid_argument {
      public:
        NameAlreadyTaken() noexcept = delete;
        explicit NameAlreadyTaken(std::string const &s) noexcept
            : invalid_argument(s){};
        ~NameAlreadyTaken() noexcept override = default;
        NameAlreadyTaken(NameAlreadyTaken const &src) noexcept = delete;
        NameAlreadyTaken &operator=(NameAlreadyTaken const &rhs) noexcept = delete;
    };

    explicit ANommable(std::string const &name)
        : name_(name) {
    }
    ANommable() = delete;
    virtual ~ANommable() = default;

    std::string const &getName() const {
        return name_;
    }

  protected:
    std::string name_;
};
