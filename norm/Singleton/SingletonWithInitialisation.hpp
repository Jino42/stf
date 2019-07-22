#pragma once

#include <memory>


class SingletonWithInitialisation {
public:

    static void &Initialisation(int argument1 /*, others ...*/);
    static SingletonWithInitialisation &Get();

private:

    static std::unique_ptr<SingletonWithInitialisation> instance_;
};