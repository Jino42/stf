#pragma once

#include <NTL.hpp>

struct Rangef {
    cl_int isRange;
    cl_float rangeMin;
    cl_float rangeMax;

    Rangef(bool pIsRange, float pMin, float pMax) :
    isRange(pIsRange),
    rangeMin(pMin),
    rangeMax(pMax)
    {}

    Rangef() :
            isRange(false),
            rangeMin(0.0f),
            rangeMax(1.1f)
    {}
};

struct Rangei {
    cl_int isRange;
    cl_int rangeMin;
    cl_int rangeMax;

    Rangei(bool pIsRange, int pMin, int pMax) :
            isRange(pIsRange),
            rangeMin(pMin),
            rangeMax(pMax)
    {}

    Rangei() :
            isRange(false),
            rangeMin(0),
            rangeMax(100)
    {}
};

struct Rangevec3 {
    cl_int isRange;
    cl_float3 min;
    cl_float3 max;
};
