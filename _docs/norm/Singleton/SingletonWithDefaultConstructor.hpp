#pragma once

#include <memory>


class SingletonWithDefaultConstructor {
public:

    static SingletonWithDefaultConstructor &Get();

private:

    static std::unique_ptr<SingletonWithDefaultConstructor> instance_;
};