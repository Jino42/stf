#include "SingletonWithInitialisation.hpp"

static void &SingletonWithInitialisation::Initialisation(int argument1 /*, others ...*/) {
    instance_ = std::make_unique<SingletonWithInitialisation>(argument1 /*, others ...*/);
}

SingletonWithInitialisation &SingletonWithInitialisation::Get() {
    assert(!!instance_);
    return *instance_;
}

std::unique_ptr<SingletonWithInitialisation> SingletonWithInitialisation::instance_ = nullptr;