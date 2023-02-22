//
// Created by xxw on 2023/2/13.
//

#ifndef STARTRENDER_MATH_H
#define STARTRENDER_MATH_H

#include "../platform/def.h"

class Math {
public:

    constexpr static const float float_max=3.402823466e+38F;
    constexpr static const double double_max=1.7976931348623158e+308;

#ifdef USE_DECIMAL_FLOAT64
    constexpr static const double decimal_max=double_max;
#else
    constexpr static const float decimal_max=float_max;
#endif




};


#endif //STARTRENDER_MATH_H
