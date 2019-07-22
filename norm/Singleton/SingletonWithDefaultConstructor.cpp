#include "SingletonWithDefaultConstructor.hpp"

SingletonWithDefaultConstructor &SingletonWithDefaultConstructor::Get() {
    if (!instance_)
        instance_ = std::make_unique<SingletonWithDefaultConstructor>();
    return *instance_;
}

std::unique_ptr<SingletonWithDefaultConstructor> SingletonWithDefaultConstructor::instance_ = nullptr;